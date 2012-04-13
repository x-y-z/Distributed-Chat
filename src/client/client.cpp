//
//  client.cpp
//  
//
//  Created by Dong Xin on 12-4-13.
//  Copyright (c) 2012年 . All rights reserved.
//

#include <iostream>
#include "client.h"
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

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

int client::processMSG(myMsg msg)
{
    msgParser parser = new msgParser(msg);
    if(parser.isACK()){
        alarm(0); 
    }
    else{
        switch (parser.msgTypeIs()) {
            case join:
                //the client get a Join message, which means the one he asked is not the sequencer, and the info of the sequencer is returned via this message.
                
                break;
            case join_ack:
                
                break;
                
            case join_broadcast:
                break;
            case leave:
                break;
            case leave_broadcast:
                break;
            case msg:
                break;
            case msg_broadcast:
                break;
            case election_req:
                break;
            case election_ok:
                break;
            case msgError:
                break;
                
            default:
                break;
        }
    }
}