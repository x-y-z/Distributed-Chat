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
#include <cstring>

myMsg msgMaker::makeACK()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    
    tmp.sendORrev = 1;
    return tmp;
}

myMsg msgMaker::makeJoin()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 0;
    return tmp;
}

myMsg msgMaker::makeJoinACK()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 1;
    return tmp;
}


myMsg msgMaker::makeJoinBCast()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 2;
    return tmp;
}

myMsg msgMaker::makeLeave()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 3;
    return tmp;
}

myMsg msgMaker::makeLeaveBCase()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 4;
    return tmp;
}

myMsg msgMaker::makeMsg(const char *msgCnt, int msgLen)
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 5;
    tmp.msgLen = msgLen;
    
    strncpy(tmp.msgContent, msgCnt, msgLen);

    return tmp;
}

myMsg msgMaker::makeMsgBCast(const char *msgCnt, int msgLen)
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 6;
    tmp.msgLen = msgLen;
    
    strncpy(tmp.msgContent, msgCnt, msgLen);

    return tmp;
}

myMsg msgMaker::makeElec()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 7;
    return tmp;
}

myMsg msgMaker::makeElecOK()
{
    myMsg tmp;
    memset(&tmp, 0, sizeof(myMsg));
    tmp.sendORrev = 0;

    tmp.chat = 8;
    return tmp;
}

