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
    
}

int client::processMSG(myMsg msg)
{
    msgParser parser(msg);
    if(parser.isACK()){
        alarm(0); 
    }
    else{
        switch (parser.msgTypeIs()) {
            case navi:
                //the client get a navi message, which means the one he asked is not the sequencer, and the info of the sequencer is returned via this message.
                //get the info of the sequencer and send another join message to it.
                join(msg.ip, msg.port);//use message structure directly.
                break;
            case join_ack:
                //get the peerlist and client_id decided by the sequencer and store them locally for future use.
                break;
                
            case join_broadcast:
                //get the ip ,port name and client ID of the new user and store them locally. 
                break;
            case leave_broadcast:
                //remove the specific user from the peer list
                break;
            case msg_broadcast:
                //show the message to the standard output
                break;
            case election_req:
                //do BULLY
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
    //setupt the UDP socket
    clntUDP.setRemoteAddr(s_ip.c_str(),s_port);
    
    //args: sequencer's ip, port, myIP, myPort,myName;
//    myMsg message = mmaker.makeJoin(s_ip, s_port,IP,port,name);
//    char[100] temp = (char) message;
//    clnt.sendTo(temp,sizeof(temp),saddr,saddr_len);
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


