//
//  
//
//  This is the structure for the message used in the distributed chat program.
//
//  Created by Dong Xin on 12-4-8.
//  Copyright (c) 2012年  All rights reserved.
//

#ifndef __MYMSG_H__
#define __MYMSG_H__

#include <string>
#include <vector>
#include <ostream>

using namespace std;



typedef struct neighbor{
    char name[50];
    char ip[20];
    int port;
    int c_id;
} peer;

typedef struct messageStruct {
    int sendORrev;//0 for send, 1 for ACK
    int chat;//0 for Join, 1 for navi 2 for ACK-Join, 3 for Join-broadcast, 
             //4 for Leave, 5 for Leave-broadcast, 
             //6 for MSG, 7 for MSG-broadcast, 
             //8 for election request, 9 for election OK, 10 leader broadcast.
    char ip[20];
    int port;
    int self_id;
    int msgLen;
    char *msgContent;
} myMsg;

enum msgType{
    join,
    navi,
    join_ack,
    join_broadcast,
    leave,
    leave_broadcast,
    msg,
    msg_broadcast,
    election_req,
    election_ok,
    msgError
};
 
inline ostream& operator<<(ostream &o, const msgType &n)
{
    switch(n)
    {
        case join:
            return o<<"Join";
        case navi:
            return o<<"Navi";
        case join_ack:
            return o<<"Join-ACK";
        case join_broadcast:
            return o<<"Join-Broadcast";
        case leave:
            return o<<"Leave";
        case leave_broadcast:
            return o<<"Leave-Broadcast";
        case msg:
            return o<<"Message";
        case msg_broadcast:
            return o<<"Message-Broadcast";
        case election_req:
            return o<<"Election-Request";
        case election_ok:
            return o<<"Election-OK";
        case msgError:
            return o<<"Msg Error";
        default:
            return o<<"invalid value";
    }
}

class msgParser
{
private:
    myMsg _content;
    char *_rawMsg;
    int _msgLen;
    bool init;
public:
    msgParser(){init = false;};
//    msgParser(myMsg aMsg){ _content = aMsg; init = true;};
    msgParser(const char *msg, int len);
    ~msgParser(){};
public:
    bool isACK();
    msgType msgTypeIs();
    int senderInfo(string &ip, int &port, int &id);
    int joinName(string &name);
    int joinFeedback(int &msgMaxCnt, int &my_id, vector<peer> &peerlist); 
    int getMsg(string &text);
    int getMsg(int &msg_seq, string &text);
private:
    int msgContent(char *msg, int &msgLen);
};


#endif
