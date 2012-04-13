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
                
                break;
            
                
            default:
                break;
        }
    }
}