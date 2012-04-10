// ===================================================================
// 
//       Filename:  myMsg.cpp
// 
//    Description:  message parser
// 
//        Version:  1.0
//        Created:  04/10/2012 13:37:45
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ===================================================================
#include "msgParser.h"
#include <cstring>


bool msgParser::isACK()
{
    if (init == false)
    {
        //throw
        return false;
    }


    int sendORrev;//0 for send, 1 for ACK
    if (_content.sendORrev == 0)
        return false;
    else if (_content.sendORrev == 1)
        return true;
    else
    {
        //throw
        return false;
    }

    //never reach here
    //assert(0);
    return false;
}

msgType msgParser::msgTypeIs()
{
    if (init == false)
    {
        //throw
        return msgError;
    }

    switch (_content.chat)
    {
        case 0:
            return join;
            break;
        case 1:
            return join_ack;
            break;
        case 2:
            return join_broadcast;
            break;
        case 3:
            return leave;
            break;
        case 4:
            return leave_broadcast;
            break;
        case 5:
            return msg;
            break;
        case 6:
            return msg_broadcast;
            break;
        case 7:
            return election_req;
            break;
        case 8:
            return election_ok;
            break;
        default:
            return msgError;
            break;
    }

    return msgError;

}

int msgParser::msgContent(char *msgOut, int &msgLen)
{
    if (init == false)
    {
        //throw
        return -1;
    }

    if (msgTypeIs() == msg && msgTypeIs() == msg_broadcast)
    {
        strncpy(msgOut, _content.msgContent, _content.msgLen);
        msgLen = _content.msgLen;
        return 0;
    }  
    else
    {
        msgOut = NULL;
        msgLen = -1;
        return -1;
    }
  

    //should not be here
    return 1;
}

