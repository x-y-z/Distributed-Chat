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

class msgMaker
{
public:
    msgMaker(){};
    ~msgMaker(){};
public:
    static myMsg makeACK();
    static myMsg makeJoin();
    static myMsg makeJoinACK();
    static myMsg makeJoinBCast();
    static myMsg makeLeave();
    static myMsg makeLeaveBCase();
    static myMsg makeMsg(const char *msgCnt, int msgLen);
    static myMsg makeMsgBCast(const char *msgCnt, int msgLen);
    static myMsg makeElec();
    static myMsg makeElecOK();
};



#endif
