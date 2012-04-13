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

typedef struct messageStruct {
    int sendORrev;//0 for send, 1 for ACK
    int chat;//0 for Join, 1 for ACK-Join, 2 for Join-broadcast, 
             //3 for Leave, 4 for Leave-broadcast, 
             //5 for MSG, 6 for MSG-broadcast, 
             //7 for election request, 8 for election OK.
    char IP[20];
    int port;
    int self_id;
    int msgLen;
    char *msgContent;
    
} myMsg;

enum msgType{
    join,
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
    bool init;
public:
    msgParser(){init = false;};
    msgParser(myMsg aMsg){ _content = aMsg; init = true;};
    ~msgParser(){};
public:
    bool isACK();
    msgType msgTypeIs();
    int msgContent(char *msg, int &msgLen);
};


#endif
