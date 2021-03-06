//
//  chatClient.h
//  
//
//  Created by Dong Xin on 12-4-10.
//  Copyright (c) 2012年 . All rights reserved.
//

#ifndef __chatClient_H__
#define __chatClient_H__

#include "../msgType/msgMaker.h"
#include "../msgType/msgParser.h"
#include "../udp/udp.h"
#include <vector>
#include <queue>
#include <string>

#define INIT 0
#define WAIT_ACK 1
#define NORMAL 2
#define ELEC 3 
#define ELEC_CLIENT 4
#define ELEC_LEADER 5

using namespace std;



class chatClient
{
private:
    string name, s_ip,sname;
    string IP;
    int port, C_ID, msgMaxCnt;
    int  s_port, reSendCount,s_id;
    int status;//0:initial, 1:waiting for Join ACK, 2:normal, 3:election
    bool next, electWin;
    myMsg msgToSend;
    msgMaker mmaker;//set up necassary info by calling the "setInfo" function
    UDP clntUDP;
    vector<peer> clientList; 
    queue<string> localMsgQ;
    queue<string> inMsgQ;
    map<int,string> dspMsg;
    map<int,string>::iterator it;
    pair<map<char,int>::iterator,bool> msgT;
public:
    chatClient(){};
    chatClient(string name, const string IP,const int port);
    ~chatClient(){};

public:
    int processMSG(const char* msg, int mlen);
    int msgEnqueue(const char* msg, int len);
    int sendBroadcastMsg(string msgContent);
    void doLeave();
    void displayClients();
    int dojoin(string s_ip, int s_port);
    int dojoin(string &rs_ip, int &rs_port, UDP &listener);
    int getID();
    vector<peer> getClientList();   
    int getMaxCnt(){return msgMaxCnt;};
    string getSIP(){return s_ip;};
    int getSPort(){return s_port;};
private:
    int addNewUser(string name, string newCIP, int newCPort, int newCID);
    int removeUser(int CID);
    int doElection();
    void resend();
    
};

#endif
