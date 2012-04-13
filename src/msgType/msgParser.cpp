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
#include <assert.h>
#include <cstring>


msgParser::msgParser(const char *msg, int len)
{
    int head_size = sizeof(myMsg) - sizeof(char*);
    memcpy(&_content, msg, head_size);

    assert(_content.msgLen == len - head_size);

    _content.msgContent = new char[_content.msgLen + 1];
    memset(_content.msgContent, 0, _content.msgLen + 1);
    memcpy(_content.msgContent, msg + head_size, _content.msgLen);
}

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
            return navi;
            break;
        case 2:
            return join_ack;
            break;
        case 3:
            return join_broadcast;
            break;
        case 4:
            return leave;
            break;
        case 5:
            return leave_broadcast;
            break;
        case 6:
            return msg;
            break;
        case 7:
            return msg_broadcast;
            break;
        case 8:
            return election_req;
            break;
        case 9:
            return election_ok;
            break;
        default:
            return msgError;
            break;
    }

    return msgError;

}

int msgParser::senderInfo(string &ip, int &port, int &id)
{
    ip.assign(_content.ip);
    port = _content.port;
    id = _content.self_id;
    
    return 0;
}

int msgParser::joinName(string &name)
{
    if (msgTypeIs() != join || msgTypeIs() != join_broadcast)
    {
        return -1;
    }

    name.assign(_content.msgContent);

    return 0;
}

int msgParser::joinFeedback(int &msgMaxCnt, int &my_id, vector<peer> &peerlist) 
{
    int peerNum = 0;
    peer *pList;
    peer a;
    
    msgMaxCnt = *((int *)_content.msgContent);
    my_id =  *(((int *)_content.msgContent) + 1);
    pList = (peer*)(_content.msgContent + 2*sizeof(int));

    peerNum = (_content.msgLen - 2*sizeof(int))/sizeof(peer);

    for (int i = 0; i < peerNum; ++i)
    {
        peerlist.push_back(pList[i]);
    }

    return 0;
}

int msgParser::getMsg(string &text)
{
    if (msgTypeIs() != msg)
    {
        return -1;
    }

    text.assign(_content.msgContent, _content.msgLen);

}

int msgParser::getMsg(int &msg_seq, string &text)
{
    if (msgTypeIs() != msg_broadcast)
    {
        return -1;
    }
    msg_seq = *((int*)_content.msgContent);

    text.assign(_content.msgContent+sizeof(int), 
                _content.msgLen - sizeof(int));

    return 0;
}

//==============
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

