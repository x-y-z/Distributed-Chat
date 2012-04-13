//
//  myMsg.h
//
//  This is the structure for the message used in the distributed chat program.
//
//  Created by Dong Xin on 12-4-8.
//  Copyright (c) 2012年  All rights reserved.
//

#ifndef _myMsg_h
#define _myMsg_h
#define MAXLENGTH 255

typedef struct neighbor{
    char name[50],IP[20];
    int port,C_ID;
} peer;

typedef struct messageStruct {
    int sendORrev;//0 for send, 1 for ACK
    int chat;//0 for Join, 1 for ACK-Join, 2 for Join-broadcast, 
             //3 for Leave, 4 for Leave-broadcast, 
             //5 for MSG, 6 for MSG-broadcast, 
             //7 for election request, 8 for election OK.
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

class msgParser
{
private:
    myMsg _content;
    char *_rawMsg;
    int _msgLen;
    bool init;
public:
    msgParser(){init = false;};
    msgParser(myMsg aMsg){ _content = aMsg; init = true;};
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
