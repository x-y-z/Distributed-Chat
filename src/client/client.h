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
#include <vector>
#include <string>

using namespace std;

#define DEFAULT_CLIENT_CAPACITY 100

typedef struct neighbor{
    char name[50],IP[20];
    int port,C_ID;
} peer;

class client
{
private:
    string IP, name, s_ip;
    int port, C_ID, s_port, reSendCount;
    myMsg msgToSend;
    vector<peer> clientList(DEFAULT_CLIENT_CAPACITY); 
public:
    client(string name, string IP, int port);
    ~client(){};

public:
    int processMSG(myMsg msg);
    
private:
    int join(string s_ip, int s_port);
    int sendBroadcastMsg(string msgContent);
    int addNewUser(string name, string newCIP, int newCPort, int newCID);
    int removeUser(int CID);
    void setCID(int ID);
    void doElection(myMsg msg);
    void resend();
    
}

#endif
