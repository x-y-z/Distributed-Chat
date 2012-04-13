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
#include <queue>
#include <string>

using namespace std;



class client
{
private:
    string name, s_ip;
    string IP;
    int port, C_ID;
    int  s_port, reSendCount;
    myMsg msgToSend;
    msgMaker mmaker;//set up necassary info by calling the "setInfo" function
    vector<peer> clientList; 
    queue<string> localMsgQ;
public:
    client(string name, const string IP,const int port);
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
    
};

#endif
