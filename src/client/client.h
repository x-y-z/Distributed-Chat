//
//  client.h
//  
//
//  Created by Dong Xin on 12-4-10.
//  Copyright (c) 2012年 . All rights reserved.
//

#ifndef _CLIENT_H
#define _CLIENT_H

#include "msgMaker.h"
#include "msgParser.h"

#define DEFUAL_CLIENT_CAPACITY 100

class client
{
private:
    const char* IP, name, s_ip;
    int port, C_ID, s_port;
    vector<client> clientList(DEFUAL_CLIENT_CAPACITY); 
public:
    client(const char* name, const char* IP, int port){};
    ~client(){};

public:
    int join(const char* s_ip, int s_port){};
    int sendBroadcastMsg(const char* msgContent){};
    int addNewUser(const char* name, const char* newCIP, int newCPort, int newCID){};
    int removeUser(int CID);
    
private:
    void setCID(int ID){};
    void doElection(myMsg msg){};
    
}

#endif
