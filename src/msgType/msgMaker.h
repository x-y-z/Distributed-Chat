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

using namespace std;

class msgMaker
{
private:
    string _ip;
    int _port;
    int _self_id;
public:
    msgMaker(const string &ip, const int &port, const int &id)
    {
        _ip = ip;
        _port = port;
        _self_id = id;
    }
    ~msgMaker(){};
public:
    myMsg makeACK();
    myMsg makeJoin();
    
    myMsg makeJoinACK();
    myMsg makeJoinBCast();
    myMsg makeLeave();
    myMsg makeLeaveBCase();
    myMsg makeMsg(const char *msgCnt, int msgLen);
    myMsg makeMsgBCast(const char *msgCnt, int msgLen);
    myMsg makeElec();
    myMsg makeElecOK();
};



#endif
