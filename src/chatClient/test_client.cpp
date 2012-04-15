//
//  test_client.cpp
//  
//
//  Created by Dong Xin on 12-4-15.
//  Copyright (c) 2012年. All rights reserved.
//

#include <iostream>
#include "chatClient.h"

int main()
{
    chatClient a("xindon","127.0.0.1",12345);
    msgMaker mmaker;
    const char *temp = "hello";
    vector<peer> clientList;
    peer user1;
    user1.name="xindon";
    user1.ip = "123.456.789.111";
    user1.port= 12;
    user1.c_id = 10;
    peer user2;
    user2.name="yanzi";
    user2.ip = "127.0.0.1";
    user2.port = 14;
    user2.c_id = 11;
    myMsg msg  = mmaker.makeJoinACK(155,15,);
    string outmsg;
    int outlen; 
    msgMaker::serialize(outmsg,outlen,msg);
    cout<<outmsg<<endl;
    a.msgEnqueue(outmsg);
    
    return 0;
}