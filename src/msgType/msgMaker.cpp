// ===================================================================
// 
//       Filename:  msgMaker.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/10/2012 14:13:30
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ===================================================================
#ifndef DEBUG
#define DEUBG
#endif

#include "msgMaker.h"
#include <cstring>
#include <assert.h>


myMsg msgMaker::makeACK()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    
    tmp.sendORrev = 1;
    tmp.chat = -1;
 
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;

    tmp.msgLen = 0;
    return tmp;
}

myMsg msgMaker::makeJoin(const string &name)
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 0;

    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;
    
    tmp.msgLen = name.length();

    tmp.msgContent = new char[tmp.msgLen];
    memcpy(tmp.msgContent, name.c_str(), tmp.msgLen);

    return tmp;
}

myMsg msgMaker::makeNavi()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 1;
  
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;

    tmp.msgLen = 0;
    return tmp;
}

myMsg msgMaker::makeJoinACK(int msgMaxCnt, int c_id, 
                            const vector<peer> &peerlist)
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 2;
    
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;
    
    tmp.msgLen = sizeof(int)*2 + peerlist.size()*sizeof(peer);

    tmp.msgContent = new char[tmp.msgLen];

    *((int*)tmp.msgContent) = msgMaxCnt;
    *(((int*)tmp.msgContent) + 1) = c_id;

    peer *pList = (peer *)(tmp.msgContent + sizeof(int)*2);
    for (int i = 0; i < peerlist.size(); ++i)
    {
        //peer aP = peerlist[i];
        //memcpy(pList + i, (char *)&aP, sizeof(peer));
        pList[i] = peerlist[i];
    }


    return tmp;
}


myMsg msgMaker::makeJoinBCast(const string &name)
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 3;

    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;
    
    tmp.msgLen = name.length();

    tmp.msgContent = new char[tmp.msgLen];
    memcpy(tmp.msgContent, name.c_str(), tmp.msgLen);

   return tmp;
}

myMsg msgMaker::makeLeave()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 4;

    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;

    return tmp;
}

myMsg msgMaker::makeLeaveBCast()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 5;
 
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;

    return tmp;
}

myMsg msgMaker::makeMsg(const char *msgCnt, int msgLen)
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 6;
    tmp.msgLen = msgLen;
 
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;
   
    tmp.msgContent = new char[msgLen];
    memcpy(tmp.msgContent, msgCnt, msgLen);

    return tmp;
}

myMsg msgMaker::makeMsgBCast(const char *msgCnt, int msgLen, int seq_num)
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 7;
    tmp.msgLen = msgLen + sizeof(int);
  
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;
   
    tmp.msgContent = new char[tmp.msgLen];

    *((int*)tmp.msgContent) = seq_num;
   
    memcpy(tmp.msgContent + sizeof(int), msgCnt, msgLen);

    return tmp;
}

myMsg msgMaker::makeElec()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 8;
   
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;

    return tmp;
}

myMsg msgMaker::makeElecOK()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 9;
   
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;

    return tmp;
}

myMsg msgMaker::makeLeader(const string &name)
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 10;
   
    assert(_ip.size() < 20);
    memcpy(tmp.ip, _ip.c_str(), _ip.size());
    
    assert(_name.size() < 20);
    memcpy(tmp.name, _name.c_str(), _name.size());

    tmp.port = _port;
    tmp.self_id = _self_id;

    tmp.msgLen = name.length();

    tmp.msgContent = new char[tmp.msgLen];
    memcpy(tmp.msgContent, name.c_str(), tmp.msgLen);

    return tmp;
}

/* static void msgMaker::serlize(string &outMsg, int &outLen, const myMsg &inMsg)
{
    char *tmpMsg;
    outLen = sizeof(myMsg) - sizeof(char *) + inMsg.msgLen;

    tmpMsg = new char[outLen];

    memcpy(tmpMsg, (char*)&inMsg, sizeof(myMsg) - sizeof(char *));

    memcpy(tmpMsg + sizeof(myMsg) - sizeof(char*), inMsg.msgContent, 
            inMsg.msgLen);
    
    outMsg.assign(tmpMsg, outLen);
}*/
