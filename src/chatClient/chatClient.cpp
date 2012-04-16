//
//  chatClient.cpp
//  
//
//  Created by Dong Xin on 12-4-13.
//  Copyright (c) 2012年 . All rights reserved.
//

#include <iostream>
#include "chatClient.h"


using namespace std;


chatClient::chatClient(string cname, string cIP,int cport){
    name = cname;
    IP = cIP;
    port = cport;
    next = true;
    C_ID = -1;
    s_port = -1;
    reSendCount = 0;
    status = INIT;
    mmaker.setInfo(cname,cIP, cport,C_ID);
    
}
//if return 10, change to sequencer
//else return -10, stay as user.
int chatClient::msgEnqueue(string msg){
    string tempMsg;
    int toReturn =0;
    bool skip = false;
    inMsgQ.push(msg);
    //process the message queue until it's empty
    //blocking outter messages;
    while (!inMsgQ.empty()) {
        cout<<"processing messages..."<<inMsgQ.size()<<" messages left"<<endl;
        tempMsg = inMsgQ.front();
        //if return 10, change to sequencer
        //else return -10, stay as user.
        if(processMSG(tempMsg.c_str(),tempMsg.size())==10){
            toReturn=10;
        }
        else toReturn=-10;
        inMsgQ.pop();
    }
    return toReturn;
    
}

int chatClient::processMSG(const char* msg, int mlen)
{
    peer newUser;
    string newName, newIP, textmsg, outmsg;
    int newPort, newID;
    int outlen;
    int seqNum =0;
    myMsg tempMsg;
    vector<peer>::iterator aIter;
    msgParser parser(msg, mlen);
        
    if(parser.isACK()){
        //return 0;
        
    }
    else{
        switch (parser.msgTypeIs()) {
            case join:
                if(status!=ELEC){
                    //setup and send a Navi message
                    tempMsg = mmaker.makeNavi();
                    msgMaker::serialize(outmsg,outlen,tempMsg);
                    //clntUDP.sendTo(outmsg.c_str(),outlen);
                    return 1;
                }
                else{
                    inMsgQ.push(msg);
                    return 1;
                }
                break;
            case navi:
                //the chatClient get a navi message, which means the one he asked is not the sequencer, and the info of the sequencer is returned via this message.
                //get the info of the sequencer and send another join message to it.
                
                parser.senderInfo(newIP,newPort,newID);
                dojoin(newIP, newPort);//use message structure directly.
                return 1;
                break;
            case join_ack:
                //get the peerlist and client_id decided by the sequencer and store them locally for future use.
                
                parser.joinFeedback(msgMaxCnt, C_ID, clientList);
                cout<<"There are "<<clientList.size()<<" users in the list"<<endl;
                cout<<"they are: "<<endl;
                cout<<"name: "<<clientList[0].name<<"; IP: "<<clientList[0].ip<<"; C_ID: "<<clientList[0].c_id<<"; port: "<<clientList[0].port<<endl;
                cout<<"name: "<<clientList[1].name<<"; IP: "<<clientList[1].ip<<"; C_ID: "<<clientList[1].c_id<<"; port: "<<clientList[1].port<<endl;
                mmaker.setInfo(name,IP, port,C_ID);
                status = NORMAL;
                return 1;
                break;
                
            case join_broadcast:
                //get the ip ,port name and client ID of the new user and store them locally.
                if(status==NORMAL){
                    parser.senderInfo(newIP,newPort, newID);
                    parser.joinName(newName);
                    memcpy(newUser.name,newName.c_str(),newName.size());
                    memcpy(newUser.ip, newIP.c_str(), newIP.size());
                    newUser.c_id = newID;
                    newUser.port = newPort;
                    clientList.push_back(newUser);
                    return 1;
                }
                else{
                    cerr<<"Unexpected Join Broadcast message @"<<name<<" !"<<endl;
                    inMsgQ.push(string(msg));
                    return -1;
                }
                break;
            case leave_broadcast:
                //remove the specific user from the peer list
                if(status==NORMAL){
                    parser.senderInfo(newIP,newPort,newID);
                    
                    for(aIter = clientList.begin(); aIter != clientList.end(); aIter++){
                        if((*aIter).c_id==newID){
                            clientList.erase(aIter);
                            break;
                        }
                    }
                    //sequencer leaves
                    if (newIP==s_ip) {
                        if(doElection()==1){
                            return 10;
                        }
                        else return -10;
                    }
                    
                }
                else{
                    cerr<<"Unexpected Leave Broadcast message @"<<name<<" !"<<endl;
                    return -1;
                }
                break;
            case msg_broadcast:
                
                //show the message to the standard output
                if(status==NORMAL){
                    if(parser.getMsg(seqNum,textmsg)<0){
                        cerr<<"failed to get the message content! @"<<name<<endl;
                        exit(-1);
                    }
                    cout<<textmsg<<endl;
                    return 1;
                }
                else{
                    cerr<<"Unexpected msg Broadcast message @"<<name<<" !"<<endl;
                    return -1;
                }
                break;
            case election_req:
                //do BULLY
                
                status=ELEC;
                parser.senderInfo(newIP,newPort,newID);
                
                if(C_ID>newID){
                    if(doElection()>0){
                        return 10;
                    }
                    else return -10;
                }
            
                break;
            case election_ok:
                //do BULLY
                if(status!=ELEC){
                    cerr<<"Unexpected msg election ok message @"<<name<<" !"<<endl;
                    exit(-1);
                }
                return 1;
                break;
                
            default: //not used here.
                break;
        }
    }
}

int chatClient::dojoin(string s_ip, int s_port){
    string outmsg;
    int outlen, saddr_len;

    //setupt the UDP socket
    //clntUDP.setRemoteAddr(s_ip.c_str(),s_port);
    
    //args: sequencer's ip, port, myIP, myPort,myName;
    myMsg message = mmaker.makeJoin(name);
    msgMaker::serialize(outmsg, outlen, message);    
        
//    if(clntUDP.sendToNACK(outmsg.c_str(),outlen)==-2){
//        cerr<<"Error! Not able to join to the group... App is about to exit..."<<endl;
//        exit(-1);
//    }
    
    
    return 1;
}
//return 1 if normal
//return 10 if sequencer chrashes and change to sequencer.
int chatClient::sendBroadcastMsg(string msgContent){
    string tempMsg, outmsg;
    int outlen, i;
    localMsgQ.push(msgContent);
    if(next){
        if(localMsgQ.empty()){
            cerr<<"Unexpected empty message queue in client: "<<name<<endl;
            exit(-1);
        }
        tempMsg = localMsgQ.front();
        localMsgQ.pop();
        myMsg message = mmaker.makeMsg(tempMsg.c_str(),msgContent.size());
        msgMaker::serialize(outmsg, outlen, message);
        next=false;
        //if timeout, clear local message queue and do election.
        if(clntUDP.sendToNACK(outmsg.c_str(),outlen)==-2){
            //localMsgQ.clear();
            for (i=0; i<localMsgQ.size(); i++) {
                localMsgQ.pop();
            }
            next=true;
            if(doElection()){
                return 10; 
            }
            else return -1;
        }
        next = true;
    }
    
    return 1;
}

int chatClient::addNewUser(string name, string newCIP, int newCPort, int newCID){
    peer p;
    strncpy(p.name, name.c_str(),name.size());
    strncpy(p.ip, newCIP.c_str(),newCIP.size());
    p.port = newCPort;
    p.c_id = newCID;
    clientList.push_back(p);
    
    return 1;
}

int chatClient::removeUser(int CID){
    unsigned int i=0;
    for (i=0; i<clientList.size(); i++) {
        if(clientList[i].c_id==CID){
            clientList.erase(clientList.begin()+i);
            break;
        }
    }
    return 1;
}
//return 1 if it's elected to be leader.
//return -1 if not.
int chatClient::doElection(){
    myMsg tempMsg;
    string outmsg;
    int outlen;
    tempMsg = mmaker.makeElec();
    msgMaker::serialize(outmsg,outlen,tempMsg);
//    vector<peer> timeoutClients =  clntUDP.multiCastNACK(outmsg.c_str(), outlen, clientList);
    vector<peer> timeoutClients;
    if(timeoutClients.empty()){
        status= NORMAL;
        return 1; 
    }
    else{
        
        return -1;
    }
}
