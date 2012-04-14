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
    max_id++;
}

int sequencer::processMSG(const char *inMsg, int mlen)
{
    msgParser aParser(inMsg, mlen);

    switch (aParser.msgTypeIs())
    {
        case join:
            {
                string name, ip;
                int port, id;
                aParser.senderInfo(ip, port, id);
                aParser.joinName(name);
                id = newClientId();
                //add to client list
                addToClientList(name, ip, port, id);
                //send join ack
                int msgMaxCnt = getMsgMaxCnt();
                sendJoinACK(ip, port, id, msgMaxCnt);
                //broadcast join
                sendJoinBCast(ip, port, id, name);
            }
            break;
        case leave:
            {
                string ip;
                int port, id;
                aParser.senderInfo(ip, port, id);
                //remove from client list
                findAndDeletePeer(id);
                //send leave broadcast
                sendLeaveBCast(ip, port, id);
            }
            break;
        case msg:
            {
                string ip;
                int port, id;
                aParser.senderInfo(ip, port, id);

                string recvMsg;
                aParser.getMsg(recvMsg);
                //put into local msg queue
                putMsgInQ(ip, port, id, recvMsg);
                sendMsgBCast();
            }
            break;
        case election_req:
            {
                std::cerr<<"Unexpected election, ignored!\n";
            }
            break;
        default:
            return -1;
            break;
    }

    return -1;

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

    return 0;
    
}

int sequencer::sendJoinACK(const string &ip, int port, int id, int msgMaxCnt)
{
    msgMaker aMaker;
    aMaker.setInfo(my_name, my_ip, my_port, my_id);

    string aMsg;
    int aMsg_len;
    msgMaker::serialize(aMsg, aMsg_len, 
                        aMaker.makeJoinACK(msgMaxCnt, id, clientList));

    _udp.setRemoteAddr(ip.c_str(), port);
    _udp.sendTo(aMsg.c_str(), aMsg.size());

    char gMsg[1024];
    int msgLen;
    int finished = -1;

    while (!finished)
    {
        msgLen = _udp.recvFrom(gMsg, 1024);
        msgParser aParser(gMsg, msgLen);
        if (!aParser.isACK())
        {
            string tmp(gMsg, msgLen);
            _MsgQ.push_back(tmp);
        }
        else
        {
            finished = 1;
        }

        if (msgLen < 0)
        {
            //time out
            if (finished == -1)
            {
                _udp.sendTo(aMsg.c_str(), aMsg.size());
                finished++;
            }
            else
            {
                //lose remote
                findAndDeletePeer(id);
                finished = 1;
            }
        }
    }

}

