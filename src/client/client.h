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

#define INIT 0
#define WAIT_ACK 1
#define NORMAL 2
#define ELEC 3 

using namespace std;



class client
{
private:
    string name, s_ip;
    string IP;
    int port, C_ID, msgMaxCnt;
    int  s_port, reSendCount;
    int status;//0:initial, 1:waiting for Join ACK, 2:normal, 3:election
    bool next;
    myMsg msgToSend;
    msgMaker mmaker;//set up necassary info by calling the "setInfo" function
    UDP clntUDP;
    vector<peer> clientList; 
    queue<string> localMsgQ;
    queue<string> inMsgQ;
public:
    client(string name, const string IP,const int port);
    ~client(){};

public:
    int processMSG(const char* msg, int mlen);
    int msgEnqueue(string msg);
private:
    int dojoin(string s_ip, int s_port);
    int sendBroadcastMsg(string msgContent);
    int addNewUser(string name, string newCIP, int newCPort, int newCID);
    int removeUser(int CID);
    int doElection();
    void resend();
    void sig_al_handler(int signum);
    
};

#endif
