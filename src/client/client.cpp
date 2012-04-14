//
//  client.cpp
//  
//
//  Created by Dong Xin on 12-4-13.
//  Copyright (c) 2012年 . All rights reserved.
//

#include <iostream>

#include "../udp/udp.h"
#include "client.h"

#include <signal.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>


using namespace std;

//handle time out
//void client::sig_al_handler(int signum){
//    if(reSendCount==0){
//        reSendCount++;
//        signal(SIGALRM,&(this.sig_al_handler));
//        alarm(1)
//        this.resend();
//    }
//    else{
//        alarm(0);
//        if(msgToSend.){
//            
//        }
//        
//    }
//}

client::client(string cname, string cIP,int cport){
    name = cname;
    IP = cIP;
    port = cport;
    C_ID = -1;
    s_port = -1;
    reSendCount = 0;
    mmaker.setInfo(cname,cIP, cport,C_ID);
    
}

int client::processMSG(const char* msg, int mlen)
{
    peer newUser;
    string newName, newIP, textmsg;
    int newPort, newID;
    int i;
    vector<peer>::iterator aIter;
    msgParser parser(msg, mlen);
    if(parser.isACK()){
        alarm(0); 
    }
    else{
        switch (parser.msgTypeIs()) {
            case navi:
                //the client get a navi message, which means the one he asked is not the sequencer, and the info of the sequencer is returned via this message.
                //get the info of the sequencer and send another join message to it.
                parser.senderInfo(newIP,newPort,newID);
                join(newIP, newPort);//use message structure directly.
                break;
            case join_ack:
                //get the peerlist and client_id decided by the sequencer and store them locally for future use.
                parser.joinFeedback(msgMaxCnt, C_ID, clientList);
                mmaker.setInfo(name,IP, port,C_ID);
                break;
                
            case join_broadcast:
                //get the ip ,port name and client ID of the new user and store them locally.
                
                parser.senderInfo(newIP,newPort, newID);
                parser.joinName(newName);
                memcpy(newUser.name,newName.c_str(),newName.size());
                memcpy(newUser.ip, newIP.c_str(), newIP.size());
                newUser.c_id = newID;
                newUser.port = newPort;
                clientList.push_back(newUser);
                break;
            case leave_broadcast:
                //remove the specific user from the peer list
                parser.senderInfo(newIP,newPort,newID);
                
                for(aIter = clientList.begin(); aIter != clientList.end(); aIter++){
                    if((*aIter).c_id==newID){
                        clientList.erase(aIter);
                        break;
                    }
                    
                }
                break;
            case msg_broadcast:
                //show the message to the standard output
                parser.getMsg(textmsg);
                cout<<textmsg<<endl;
                break;
            case election_req:
                //do BULLY
                parser.senderInfo(newIP,newPort,newID);
                if(C_ID>newID){
                    doElection();
                }
                break;
            case election_ok:
                //do BULLY
                break;
                
            default: //not used here.
                break;
        }
    }
}

int client::join(string s_ip, int s_port){
    string outmsg;
    int outlen, saddr_len;
    struct sockaddr_in saddr= clntUDP.fromAddrToSock(s_ip.c_str(), s_port);
    saddr_len = sizeof(saddr);
    //setupt the UDP socket
    clntUDP.setRemoteAddr(s_ip.c_str(),s_port);
    
    //args: sequencer's ip, port, myIP, myPort,myName;
    myMsg message = mmaker.makeJoin(name);
    msgMaker::serlize(outmsg, outlen, message);
    clntUDP.sendTo(outmsg.c_str(),outlen, (struct sockaddr *)&saddr, saddr_len);
    return 1;
}

int client::sendBroadcastMsg(string msgContent){
    
    myMsg message = mmaker.makeMsg(msgContent.c_str(),msgContent.size());
    //char temp = (char) message;
    //send the serialized message out.
    return 1;
}

int client::addNewUser(string name, string newCIP, int newCPort, int newCID){
    peer p;
    strncpy(p.name, name.c_str(),name.size());
    strncpy(p.ip, newCIP.c_str(),newCIP.size());
    p.port = newCPort;
    p.c_id = newCID;
    clientList.push_back(p);
    
    return 1;
}

int client::removeUser(int CID){
    unsigned int i=0;
    for (i=0; i<clientList.size(); i++) {
        if(clientList[i].c_id==CID){
            clientList.erase(clientList.begin()+i);
            break;
        }
    }
    return 1;
}

void client::doElection(){
    
}
