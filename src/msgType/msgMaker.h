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
    string _ip;
    string _name;
    int _port;
    int _self_id;
public:
    msgMaker(){};
    ~msgMaker(){};
public:
    static void serlize(string &outMsg, int &outLen, const myMsg &inMsg)
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
    void setInfo(const string &name, const string &ip, 
             const int &port, const int &id)
    {
        _ip = ip;
        _port = port;
        _self_id = id;
        _name = name;
    }
    myMsg makeACK();
    myMsg makeJoin(const string &name);
    myMsg makeNavi(); 
    myMsg makeJoinACK(int msgMaxCnt, int c_id, const vector<peer> &peerlist);
    myMsg makeJoinBCast(const string &name);
    myMsg makeLeave();
    myMsg makeLeaveBCast();
    myMsg makeMsg(const char *msgCnt, int msgLen);
    myMsg makeMsgBCast(const char *msgCnt, int msgLen, int seq_num);
    myMsg makeElec();
    myMsg makeElecOK();
};



#endif
