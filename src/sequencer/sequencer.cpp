// ===================================================================
// 
//       Filename:  sequencer.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/13/2012 12:19:23
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ===================================================================
#include "sequencer.h"
#include <iostream>

sequencer::sequencer(const char* name, const char*ip, int port)
{
    max_id = 0;
    msg_seq_id = 0;
    my_name.assign(name);
    my_ip.assign(ip);
    my_port = port;
    my_id = max_id;
}

seqStatus sequencer::processMSG(const char *inMsg, int mlen)
{
    msgParser aParser(inMsg, mlen);
    seqStatus status = seqSuccess;

    switch (aParser.msgTypeIs())
    {
        case join:
            {
                string name, ip;
                int port, id;
                aParser.senderInfo(ip, name, port, id);
                aParser.joinName(name);

                std::cout<<"NOTICE "<<name<<" joined on "
                         <<ip<<":"<<port<<endl;

                id = newClientId();
                //add to client list
                addToClientList(name, ip, port, id);
                //send join ack
                int msgMaxCnt = getMsgCurCnt();
                int sendRes = sendJoinACK(ip, port, id, msgMaxCnt);
                //broadcast join
                if (sendRes == 0)
                {
                    sendRes = sendJoinBCast(ip, port, id, name);
                    if (sendRes == 0)
                        status = seqSuccess;
                    else
                        status = seqJoinBCastFail;
                }
                else
                    status = seqJoinACKFail;
            }
            break;
        case leave:
            {
                string ip, name;
                int port, id;
                aParser.senderInfo(ip, name, port, id);
                //remove from client list
                peer findRet = findAndDeletePeer(id);
                if (findRet.port == -1)
                {
                    status = seqLeaveGhost;
                    break;
                }
                //send leave broadcast
                int sendRes = sendLeaveBCast(findRet);
                if (sendRes == 0)
                {
                    status = seqSuccess;
                }
                else if (sendRes > 0)
                {
                    status = seqLeaveBCastTimeout;
                }
                else if (sendRes == -1)
                {
                    status = seqLeaveBCastNoClient;
                }
                else if (sendRes == -2)
                {
                    status = seqLeaveBCastNoName;
                }
                else if (sendRes == -3)
                {
                    status = seqNotReach;
                }
            }
            break;
        case msg:
            {
                string ip, name;
                int port, id;
                aParser.senderInfo(ip, name, port, id);

                string recvMsg;
                aParser.getMsg(recvMsg);

                //put into local msg queue
                putMsgInQ(ip, port, id, recvMsg);
                int sendRes = sendMsgBCast();
                if (sendRes == 0)
                {
                    status = seqSuccess;
                }
                else if (sendRes > 0)
                {
                    status = seqMsgBCastTimeout;
                }
                else if (sendRes == -1)
                {
                    status = seqMsgBCastNoClient;
                }
            }
            break;
        case election_req:
            {
                //for furture use
                //string ip, name;
                //int port, id;
                //aParser.senderInfo(ip, name, port, id);

                //sendLeader(ip, port, id);
                std::cerr<<"Unexpected election, ignored!\n";
                status = seqUnexpectedMsg;
            }
            break;
        default:
            status = seqUnexpectedMsg;
            break;
    }

    return status;

}



int sequencer::addToClientList(const string &name, const string &ip,
                               int port, int id)
{
    peer newPeer;
    memset(&newPeer, 0, sizeof(peer));
    memcpy(newPeer.name, 
           name.c_str(), 
           49>name.size()?name.size():49);

    if (ip.size() > 20)
    {
        std::cerr<<"ip address size too large\n";
        return -1;
    }
    memcpy(newPeer.ip, ip.c_str(), ip.size());
    newPeer.port = port;
    newPeer.c_id = id;

    clientList.push_back(newPeer);

    return 0;
    
}

int sequencer::sendJoinACK(const string &ip, int port, int id, int msgMaxCnt)
{
    int waitRes = 0;
    msgMaker aMaker;
    aMaker.setInfo(my_name, my_ip, my_port, my_id);

    string aMsg;
    int aMsg_len;
    msgMaker::serialize(aMsg, aMsg_len, 
                        aMaker.makeJoinACK(msgMaxCnt, id, clientList));

    UDP joinACKUDP;
    joinACKUDP.setRemoteAddr(ip.c_str(), port);
    waitRes = joinACKUDP.sendToNACK(aMsg.c_str(), aMsg.size());
    
    if (waitRes == -2)//lost remote
    {
        findAndDeletePeer(id);
        return -1;
    }
    return 0;
}


int sequencer::sendJoinBCast(const string &ip, int port, int id, 
        const string &name)
{
    msgMaker aMaker;
    aMaker.setInfo(name, ip, port, id);
    
    string aMsg;
    int aMsg_len;
    msgMaker::serialize(aMsg, aMsg_len, 
                        aMaker.makeJoinBCast(name));

    vector<peer> timeoutList = _udp.multiCastNACK_T(aMsg.c_str(), aMsg.size(),
                                    clientList);
    if (timeoutList.size() == 0)
        return 0;
    else
    {
        vector<peer>::iterator iter;
        for (iter = timeoutList.begin(); iter != timeoutList.end(); iter++)
        {
            peer aTimeOut = findAndDeletePeer((*iter).c_id);
            sendLeaveBCast(aTimeOut);
        }

        return -1;
    }

    return -2;
}


peer sequencer::findAndDeletePeer(int id)
{
    vector<peer>::iterator iter;
    peer aRet = {"ghost", "hell", -1, -999};

    for (iter = clientList.begin(); iter != clientList.end(); iter++)
    {
        if ((*iter).c_id == id)
        {
            std::cout<<"NOTICE "<<(*iter).name<<" left the chat or crashed"
                     <<endl;
            aRet = (*iter);
            clientList.erase(iter);
            return aRet;
        }
    }

    return aRet;//not found

}


int sequencer::sendLeaveBCast(const peer &someOne)
{
    msgMaker aMaker;

    if (clientList.size() == 0)
        return -1;//no clients
    //std::cout<<"I send leave bcast:"<<someOne;
    aMaker.setInfo(someOne.name, someOne.ip, someOne.port, someOne.c_id);

    string aMsg;
    int aMsg_len;
    msgMaker::serialize(aMsg, aMsg_len, 
                        aMaker.makeLeaveBCast());

    vector<peer>::iterator iter;
    vector<peer> timeoutList = _udp.multiCastNACK_T(aMsg.c_str(), aMsg.size(),
                                    clientList);
    if (timeoutList.size() == 0)
        return 0;//OK
    else
    {
        for (iter = timeoutList.begin(); iter != timeoutList.end(); iter++)
        {
            peer aTimeOut = findAndDeletePeer((*iter).c_id);
            sendLeaveBCast(aTimeOut);
        }

        return timeoutList.size();//how many time out
    }

    return -3;//not reachable

}


int sequencer::putMsgInQ(const string &ip, int port, int id, const string &msg)
{
    _MsgQ.push_back(msg);
}

int sequencer::sendMsgBCast()
{
    //remember to add msg_seq_num
    int msgGlobalNum = nextMsgCnt();
    string bMsg = _MsgQ.front();
    _MsgQ.pop_front();
    int status = 0;

    msgMaker aMaker;
    aMaker.setInfo(my_name, my_ip, my_port, my_id);
    
    string aMsg;
    int aMsg_len;
    msgMaker::serialize(aMsg, aMsg_len, 
                        aMaker.makeMsgBCast(bMsg.c_str(), bMsg.size(),
                                            msgGlobalNum));

    if (clientList.size() == 0)
    {
        std::cout<<bMsg<<endl;
        std::cout.flush();
        return -1;
    }

    vector<peer> timeoutList = _udp.multiCastNACK_T(aMsg.c_str(), aMsg.size(),
                                    clientList);
    if (timeoutList.size() == 0)
    {
        status = 0;
    }
    else
    {
        vector<peer>::iterator iter;
        for (iter = timeoutList.begin(); iter != timeoutList.end(); iter++)
        {
            peer aTimeOut = findAndDeletePeer((*iter).c_id);
            sendLeaveBCast(aTimeOut);
        }
        status = timeoutList.size();
    }

    std::cout<<bMsg<<endl;
    std::cout.flush();
    
    return status;
}

void sequencer::printMemberList()
{
    std::cout<<my_name<<" "<<my_ip<<":"<<my_port<<" (Leader)"<<endl;
    //for clients
    vector<peer>::iterator iter;
    for (iter = clientList.begin(); iter != clientList.end(); iter++)
    {
        std::cout<<(*iter).name<<" "<<(*iter).ip<<":"<<(*iter).port<<endl;
    }
}

int sequencer::switchFromClient(const vector<peer> &aList, int myID, int maxMsgID)
{
    my_id = myID;
    max_id = myID;
    msg_seq_id = maxMsgID;
    setClientList(aList);

    vector<peer>::iterator iter;
    for (iter = clientList.begin(); iter != clientList.end(); iter++)
    {
        if ((*iter).c_id == my_id)
        {
            clientList.erase(iter);
            break;
        }
    }

    return sendLeaderBCast();
    
}
int sequencer::sendLeaderBCast()
{
    msgMaker aMaker;
    
    vector<peer>::iterator iter;
    
    aMaker.setInfo(my_name, my_ip, my_port, my_id);

    string aMsg;
    int aMsg_len;
    msgMaker::serialize(aMsg, aMsg_len, 
                        aMaker.makeLeader(my_name));

    vector<peer> timeoutList = _udp.multiCastNACK_T(aMsg.c_str(), aMsg.size(),
                                    clientList);
    if (timeoutList.size() == 0)
        return 0;
    else
    {
        cout<<timeoutList.size()<<"users timeout"<<endl;
        for (iter = timeoutList.begin(); iter != timeoutList.end(); iter++)
        {
            peer aTimeOut = findAndDeletePeer((*iter).c_id);
            sendLeaveBCast(aTimeOut);
        }

        return -1;
    }

    return -2;


}
int sequencer::sendLeader(const string &ip, int port, int id)
{
    int waitRes = 0;
    msgMaker aMaker;
    aMaker.setInfo(my_name, my_ip, my_port, my_id);

    string aMsg;
    int aMsg_len;
    msgMaker::serialize(aMsg, aMsg_len, 
                        aMaker.makeLeader(my_name));

    UDP joinACKUDP;
    joinACKUDP.setRemoteAddr(ip.c_str(), port);
    waitRes = joinACKUDP.sendToNACK(aMsg.c_str(), aMsg.size());
    
    if (waitRes == -2)//lost remote
    {
        findAndDeletePeer(id);
        return -1;
    }
    return 0;

}
