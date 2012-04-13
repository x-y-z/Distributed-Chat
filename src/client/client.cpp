//
//  client.cpp
//  
//
//  Created by Dong Xin on 12-4-13.
//  Copyright (c) 2012年 . All rights reserved.
//

#include <iostream>

#include "client.h"
#include "../udp/udp.h"

#include <string>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

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

client::client(char* name, const char* IP, int port){
    
}

int client::processMSG(myMsg msg)
{
    msgParser parser(msg);
    if(parser.isACK()){
        alarm(0); 
    }
    else{
        switch (parser.msgTypeIs()) {
//            case navi:
//                //the client get a Join message, which means the one he asked is not the sequencer, and the info of the sequencer is returned via this message.
//                //get the info of the sequencer and send another join message to it.
//                
//                break;
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

int client::join(const char* s_ip, int s_port){
    //setupt the UDP socket
    struct sockaddr_in saddr;
    int saddr_len = 0;
    saddr = UDP::fromAddrToSock(s_ip,port);
    saddr_len = sizeof(saddr);
    UDP client(saddr);
    
    msgMaker mmaker();
    myMsg message = mmaker.makeJoin();
}