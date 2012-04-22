// ===================================================================
// 
//       Filename:  msgMaker.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/10/2012 14:13:26
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ====================================================================

#ifndef __MSG_MAKER_H__
#define __MSG_MAKER_H__

#include "msgParser.h"
#include <string>
#include <cstring>

using namespace std;


class msgMaker
{
private:
    string _ip; /**< sender ip */
    string _name; /**< sender name */
    int _port; /**< sender port */
    int _self_id; /**< sender id */
public:
    /**
     * Default constructor
     */
    msgMaker(){};
    /**
     * Defautl destructor
     */
    ~msgMaker(){};
public:
    /**
     * serialize myMsg to a string for network transportaion
     * @param outMsg output
     * @param outLen message length
     * @param inMsg input
     */
    static void serialize(string &outMsg, int &outLen, const myMsg &inMsg)
    {
        char *tmpMsg;
        outLen = sizeof(myMsg) - sizeof(char *) + inMsg.msgLen;

        tmpMsg = new char[outLen];

        memcpy(tmpMsg, (char*)&inMsg, sizeof(myMsg) - sizeof(char *));

        memcpy(tmpMsg + sizeof(myMsg) - sizeof(char*), inMsg.msgContent, 
                inMsg.msgLen);
        
        outMsg.assign(tmpMsg, outLen);
    }
public:
    /**
     * Set message information, include sender name, ip, port, and id
     * @param name sender name
     * @param ip sender ip
     * @param port sender port number
     * @param id sender id
     */
    void setInfo(const string &name, const string &ip, 
             const int &port, const int &id)
    {
        _ip = ip;
        _port = port;
        _self_id = id;
        _name = name;
    }
    /**
     * produce general ACK
     * @ret formatted message
     */
    myMsg makeACK();
    /**
     * produce join message
     * @param name joiner name
     * @ret formatted message
     */
    myMsg makeJoin(const string &name);
    /**
     * navigation message, redirecting to sequencer
     * @ret formatted message
     */
    myMsg makeNavi(); 
    /**
     * ACK for joiner
     * @param msgMaxCnt current maximum message count
     * @param c_id joiner's id
     * @param peerlist other clients
     * @ret formatted message
     */
    myMsg makeJoinACK(int msgMaxCnt, int c_id, const vector<peer> &peerlist);
    /**
     * join broadcast, info all clients
     * @param name the name of joiner
     * @ret formatted message
     */
    myMsg makeJoinBCast(const string &name);
    /**
     * leaving message, tell sequencer
     * @ret formatted message
     */
    myMsg makeLeave();
    /**
     * leave broadcast, tell all clients
     * @ret formatted message
     */
    myMsg makeLeaveBCast();
    /**
     * chatting message, to sequencer
     * @param msgCnt message content
     * @param msgLen the length of the message
     * @ret formatted message
     */
    myMsg makeMsg(const char *msgCnt, int msgLen);
    /**
     * broadcast the chatting message, from sequencer
     * @param msgCnt message content
     * @param msgLen message length
     * @param seq_num sequence number of the message
     * @ret formatted message
     */
    myMsg makeMsgBCast(const char *msgCnt, int msgLen, int seq_num);
    /**
     * election request message
     * @ret formatted message
     */
    myMsg makeElec();
    /**
     * election OK message
     * @ret formatted message
     */
    myMsg makeElecOK();
    /**
     * leader information
     * @param name the name of the leader
     * @ret formatted message
     */
    myMsg makeLeader(const string &name);
};



#endif
