//
//  client.h
//  
//
//  Created by Dong Xin on 12-4-10.
//  Copyright (c) 2012年 . All rights reserved.
//

#ifndef _CLIENT_H
#define _CLIENT_H

#include "../msgType/msgMaker.h"
#include "../msgType/msgParser.h"

#define DEFUAL_CLIENT_CAPACITY 100

typedef struct neighbor{
    char name[50],IP[20];
    int port,C_ID;
} peer;

class client
{
private:
    const char* IP, name, s_ip;
    int port, C_ID, s_port, reSendCount;
    myMsg msgToSend;
    vector<peer> clientList(DEFUAL_CLIENT_CAPACITY); 
public:
    client(const char* name, const char* IP, int port);
    ~client(){};

public:
    int processMSG(myMsg msg);
    
private:
    int join(const char* s_ip, int s_port);
    int sendBroadcastMsg(const char* msgContent);
    int addNewUser(const char* name, const char* newCIP, int newCPort, int newCID);
    int removeUser(int CID);
    void setCID(int ID);
    void doElection(myMsg msg);
    void resend();
    
}

#endif
