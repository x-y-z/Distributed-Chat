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
    //max_id++;

    /*peer self;
    strncpy(self.name, my_name.c_str(), my_name.size());
    strncpy(self.ip, my_ip.c_str(), my_ip.size());
    self.port = my_port; 
    self.c_id = my_id;
    clientList.push_back(self);*/
}

int sequencer::processMSG(const char *inMsg, int mlen)
{
    msgParser aParser(inMsg, mlen);
    int status = 0;
    std::cerr<<"got a message:"<<aParser.msgTypeIs()<<endl;

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
                    status = sendJoinBCast(ip, port, id, name);
                    status = 0;
                }
                else
                    status = -1;
            }
            break;
        case leave:
            {
                string ip, name;
                int port, id;
                aParser.senderInfo(ip, name, port, id);
                //remove from client list
                findAndDeletePeer(id);
                //send leave broadcast
                int sendRes = sendLeaveBCast(ip, port, id);
                if (sendRes == 0)
                {
                    status = 0;
                }
                else
                {
                    status = -1;
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
                    status = 0;
                }
                else
                {
                    status = -1;
                }
            }
            break;
         /*case msg_broadcast:
            {
                string ip;
                int port, id;
                aParser.senderInfo(ip, port, id);

                string recvMsg;
                aParser.getMsg(recvMsg);

                std::cout<<recvMsg;

                status = 0;
            }
            break;*/
        case election_req:
            {
                std::cerr<<"Unexpected election, ignored!\n";
                status = -2;
            }
            break;
        default:
            status = -2;
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

    _udp.setRemoteAddr(ip.c_str(), port);
    waitRes = _udp.sendToNACK(aMsg.c_str(), aMsg.size());
    
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

    vector<peer> timeoutList = _udp.multiCastNACK(aMsg.c_str(), aMsg.size(),
                                    clientList);
    if (timeoutList.size() == 0)
        return 0;
    else
    {
        vector<peer>::iterator iter;
        for (iter = timeoutList.begin(); iter != timeoutList.end(); iter++)
        {
            findAndDeletePeer((*iter).c_id);
            sendLeaveBCast((*iter).ip, (*iter).port, (*iter).c_id);
        }

        return -1;
    }

    return -2;
}


int sequencer::findAndDeletePeer(int id)
{
    vector<peer>::iterator iter;

    for (iter = clientList.begin(); iter != clientList.end(); iter++)
    {
        if ((*iter).c_id == id)
        {
            std::cout<<"NOTICE "<<(*iter).name<<"left the chat or crashed"
                     <<endl;
            clientList.erase(iter);
            return 0;
        }
    }

    return -1;//not found

}


int sequencer::sendLeaveBCast(const string &ip, int port, int id)
{
    string name("");
    msgMaker aMaker;

    if (clientList.size() == 0)
        return 0;
    
    vector<peer>::iterator iter;
    for (iter = clientList.begin(); iter != clientList.end(); iter++)
    {
        if ((*iter).c_id == id)
            name = (*iter).name;
    }

    if (name.empty())
        return -4;
    
    aMaker.setInfo(name, ip, port, id);

    string aMsg;
    int aMsg_len;
    msgMaker::serialize(aMsg, aMsg_len, 
                        aMaker.makeLeaveBCast());

    vector<peer> timeoutList = _udp.multiCastNACK(aMsg.c_str(), aMsg.size(),
                                    clientList);
    if (timeoutList.size() == 0)
        return 0;
    else
    {
        for (iter = timeoutList.begin(); iter != timeoutList.end(); iter++)
        {
            findAndDeletePeer((*iter).c_id);
            sendLeaveBCast((*iter).ip, (*iter).port, (*iter).c_id);
        }

        return -1;
    }

    return -2;

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

    vector<peer> timeoutList = _udp.multiCastNACK(aMsg.c_str(), aMsg.size(),
                                    clientList);
    if (timeoutList.size() == 0)
        status = 0;
    else
    {
        vector<peer>::iterator iter;
        for (iter = timeoutList.begin(); iter != timeoutList.end(); iter++)
        {
            findAndDeletePeer((*iter).c_id);
        }

        status = -1;
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
    msg_seq_id = maxMsgID;
    setClientList(aList);

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

    vector<peer> timeoutList = _udp.multiCastNACK(aMsg.c_str(), aMsg.size(),
                                    clientList);
    if (timeoutList.size() == 0)
        return 0;
    else
    {
        for (iter = timeoutList.begin(); iter != timeoutList.end(); iter++)
        {
            findAndDeletePeer((*iter).c_id);
        }

        return -1;
    }

    return -2;


}
/*int sequencer::waitForACK(const string &aMsg, int id, UDP &l_udp)
{
        char gMsg[1024];
        int msgLen;
        int finished = -1;

        while (finished < 0)
        {
            msgLen = l_udp.recvFrom(gMsg, 1024);
            if (msgLen > 0)
            {
                msgParser aParser(gMsg, msgLen);
                if (!aParser.isACK())
                {
                    //string tmp(gMsg, msgLen);
                    //_MsgQ.push_back(tmp);
                    std::cerr<<"wait for ack, but get unexpected msg\n";
                    exit(1);
                    finished = 3;
                }
                else
                {
                    finished = 1;//ACK recved
                }
            }
            else //if (msgLen < 0)
            {
                //time out
                if (finished == -1)
                {
                    l_udp.sendTo(aMsg.c_str(), aMsg.size());
                    finished++;
                }
                else
                {
                    //lose remote
                    finished = 2;//remote lost
                }
            }
        }

    return finished;
}*/
