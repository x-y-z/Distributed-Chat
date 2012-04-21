//
//  chatClient.cpp
//  
//
//  Created by Dong Xin on 12-4-13.
//  Copyright (c) 2012 . All rights reserved.
//

#include <iostream>
#include "chatClient.h"


using namespace std;

pthread_mutex_t udpMutex = PTHREAD_MUTEX_INITIALIZER;

//public constructor 
chatClient::chatClient(string cname, string cIP,int cport){
    name = cname;
    IP = cIP;
    port = cport;
    next = true;
    electWin = true;
    C_ID = -1;
    s_port = -1;
    reSendCount = 0;
    status = INIT;
    mmaker.setInfo(cname,cIP, cport,C_ID);
    
}
//if return 10, change to sequencer
//else return -10, stay as user.
//use a message queue to handle multiple inputs at the same time
int chatClient::msgEnqueue(const char* msg, int len){
    string tempMsg(msg,len);
    int toReturn =0;
    bool skip = false;
    inMsgQ.push(tempMsg);
    //process the message queue until it's empty
    //blocking outter messages;
    while (!inMsgQ.empty()) {
        tempMsg = inMsgQ.front();
        //if return 10, change to sequencer
        //else return -10, stay as user.
        int tempRV = processMSG(tempMsg.c_str(),tempMsg.size());
        if(tempRV==10){
            toReturn=10;
        }
        else if(tempRV==9){
            toReturn = 9;
        }
        else toReturn=-10;
        inMsgQ.pop();
    }
    return toReturn;
    
}


//fuction actually used to process the message.
//it will analyze the header of the message and take actions accordingly.
int chatClient::processMSG(const char* msg, int mlen)
{
    peer newUser;
    string newName, newIP, textmsg, outmsg;
    int newPort, newID;
    int outlen,i;
    int seqNum =0;
    myMsg tempMsg;
    vector<peer>::iterator aIter;
    msgParser parser(msg, mlen);
    
        
    if(parser.isACK()){
        //return 0;
        //since all the UDP_ACKs are handled in the underlaying UDP-wrapper layer, we simply
        //ignore the this message.
    }
    else{
        switch (parser.msgTypeIs()) {
            case join:
                if(status!=ELEC){
                    status=NORMAL;
                    //setup and send a Navi message
                    mmaker.setInfo(sname,s_ip, s_port,s_id);
                    tempMsg = mmaker.makeNavi();
                    msgMaker::serialize(outmsg,outlen,tempMsg);
                    parser.senderInfo(newIP,newName,newPort,newID);
                    
                    UDP joinUDP;
                    joinUDP.setRemoteAddr(newIP.c_str(),newPort);
                    joinUDP.sendToNACK(outmsg.c_str(),outlen);
                    
                    mmaker.setInfo(name,IP, port,C_ID);
                    return 1;
                }
                else{
                    //receive a Join request message during the election, 
                    //push it into message queue and
                    //handle it later on.
                    inMsgQ.push(msg);
                    return 1;
                }
                break;
            case navi:
                //the chatClient get a navi message, which means the one 
                //he asked is not the sequencer, and the info of the 
                //sequencer is returned via this message.
                //get the info of the sequencer and send another join 
                //message to it.
                
                parser.senderInfo(newIP, newName, newPort,newID);
                dojoin(newIP, newPort);
                return 1;
                break;
            case join_ack:
                //get the peerlist and client_id decided by the sequencer 
                //and store them locally for future use.
                
                parser.joinFeedback(msgMaxCnt, C_ID, clientList);
                parser.senderInfo(newIP,newName,newPort,newID);
                s_id = newID;
                s_ip = newIP;
                s_port = newPort;
                sname = newName;
                clntUDP.setRemoteAddr(s_ip.c_str(), s_port);
                //call the setInfo again so as to set the C_ID field
                mmaker.setInfo(name,IP, port,C_ID);
                displayClients();
                status = NORMAL;
                return 1;
                break;
                
            case join_broadcast:
                //get the ip ,port name and client ID of the new user 
                //and store them locally.
                
                if(status!=ELEC){
                    status=NORMAL;
                    parser.senderInfo(newIP, newName, newPort, newID);
                    if(newID==C_ID){
                        return 1;
                    }
                    parser.joinName(newName);
                    memset(&newUser, 0, sizeof(peer));
                    memcpy(newUser.name,newName.c_str(),newName.size());
                    memcpy(newUser.ip, newIP.c_str(), newIP.size());
                    newUser.c_id = newID;
                    newUser.port = newPort;
                    clientList.push_back(newUser);
                    cout<<"NOTICE "<<newUser.name<<" joined on "
                        <<newUser.ip<<":"<<newUser.port<<endl;
                    return 1;
                }
                else{
                    //in wrong state, push the message into message queue 
                    //and handle later on
                    cerr<<"Unexpected Join Broadcast message @"<<name
                        <<" !"<<endl;
                    string temps(msg,mlen);
                    inMsgQ.push(temps);
                    return -1;
                }
                break;
            case leave_broadcast:{
                //remove the specific user from the peer list
                vector<peer> timeoutList;
                if(status!=ELEC){
                    status=NORMAL;
                    parser.senderInfo(newIP, newName, newPort,newID);
                    
                    for(aIter = clientList.begin(); 
                        aIter != clientList.end(); aIter++){
                        if((*aIter).c_id==newID){
                            cout<<"NOTICE "<<aIter->name
                                <<" left the chat or crashed"<<endl;
                            clientList.erase(aIter);
                            break;
                        }
                    }
                    //sequencer leaves
                    if (newID==s_id) {
                        for(aIter = clientList.begin(); 
                            aIter != clientList.end(); aIter++){
                            if((*aIter).c_id==newID){
                                clientList.erase(aIter);
                                cout<<"NOTICE "<<aIter->name
                                    <<" left the chat or crashed"<<endl;
                                break;
                            }
                        }
                        if(doElection()==1){
                            
                            return 10;
                        }
                        else return -10;
                    }
                    
                }
            
                else{
                    cerr<<"Unexpected Leave Broadcast message @"<<name
                        <<" !"<<endl;
                    string temps(msg,mlen);
                    inMsgQ.push(temps);
                    return -1;
                }
                break;
            }
            case msg_broadcast:
                
                //show the message to the standard output
                if(status!=ELEC){
                    status =NORMAL;
                    if(parser.getMsg(seqNum,textmsg)<0){
                        cerr<<"failed to get the message content! @"
                            <<name<<endl;
                        exit(-1);
                    }
                    
                    //normal case
                    if(seqNum==(msgMaxCnt+1)){
                            cout<<textmsg<<endl;
                            msgMaxCnt++;
                        }
                    //receive duplicated message, ignore.
                    else if(seqNum<(msgMaxCnt+1)){
                    //ignore;
                    }
                    //missed earlier messages, search & wait for retransimit.
                    else if(seqNum>(msgMaxCnt+1)){
                            dspMsg.insert(pair<int,string>(seqNum,textmsg));	
                            it = dspMsg.find((msgMaxCnt+1));			
                            while(it!=dspMsg.end()){
                                    cout<<(*it).second<<endl;
                                    dspMsg.erase(it);
                                    msgMaxCnt++;
                                    it = dspMsg.find((msgMaxCnt+1));
                            }	
                    }		    
                    return 1;
                }
                else{
                    cerr<<"Unexpected msg Broadcast message @"<<name
                        <<" !"<<endl;
                    string temps(msg,mlen);
                    inMsgQ.push(temps);
                    return -1;
                }
                break;
            case election_req:{
                //do BULLY
                if(status==NORMAL){
                    parser.senderInfo(newIP, newName, newPort,newID);
                    
                    if(C_ID>=newID){
                        if(doElection()>0){
                            
                            return 10;
                        }
                        else return -10;
                    }
                }
                else return -1;
                break;
            }
            case election_ok:
                //do BULLY
                if(status!=ELEC){
                    cerr<<"Unexpected msg election ok message @"<<name
                        <<" !"<<endl;
                    exit(-1);
                }
                else{
                    electWin=false;
                }
                return 1;
                break;
            case leader_broadcast:
                //get and setup the info of the new sequencer
                //here we just ignore the state which this client is in. 
                //Cause it's safe 
                //to change the sequencer here.
                if(status==ELEC_CLIENT||status==NORMAL){
                    status = NORMAL;
                    parser.senderInfo(newIP, newName, newPort,newID);
                    if(newID!=C_ID&&newID!=s_id){
                        
                        s_ip = newIP;
                        s_port = newPort;
                        s_id = newID;
                        sname = newName;
                        pthread_mutex_lock(&udpMutex);
                        clntUDP.updateSocket(s_ip.c_str(),s_port);
                        pthread_mutex_unlock(&udpMutex);
                        return 9;
                    }
                }return 1;
                break;
            default: //not used here.
                break;
        }
    }
}


//function used to communicate with the sequencer so as to join the chat group.
int chatClient::dojoin(string rs_ip, int rs_port){
    string outmsg;
    int outlen, saddr_len;
    s_ip = rs_ip;
    s_port = rs_port;
    //setupt the UDP socket
    UDP joinUDP;
    joinUDP.setRemoteAddr(s_ip.c_str(),s_port);
    
    //args: sequencer's ip, port, myIP, myPort,myName;
    myMsg message = mmaker.makeJoin(name);
    msgMaker::serialize(outmsg, outlen, message);    
    
    
    
    
    if(joinUDP.sendToNACK(outmsg.c_str(),outlen)==-2){
        cerr<<"Error! Not able to join to the group... App is about to exit..."
            <<endl;
        exit(-1);
    }
    
    
    return 1;
}

//overloaded function used so as to ensure all the info are correctly 
//rechieved before we
//actually starts the chatting.

int chatClient::dojoin(string &rs_ip, int &rs_port, UDP &listener){
    string outmsg;
    int outlen, saddr_len, recvlen, MAX_MSG_LEN=2048;
    char recvMsg[2048];
    const string myName,myIP;
    int myPort,myID;
    s_ip = rs_ip;
    s_port = rs_port;
    
    //setupt the UDP socket
    UDP joinUDP;
    joinUDP.setRemoteAddr(s_ip.c_str(),s_port);
    
    //args: sequencer's ip, port, myIP, myPort,myName;
    myMsg message = mmaker.makeJoin(name);
    msgMaker::serialize(outmsg, outlen, message);    
    if(joinUDP.sendToNACK(outmsg.c_str(),outlen)==-2){
        cerr<<"Error! Not able to join to the group... App is about to exit..."
            <<endl;
        exit(-1);
    }
   
    while(1){
        recvlen = listener.recvFromNACK(recvMsg, MAX_MSG_LEN, myName, myIP, 
                                        myPort, myID);
        string reMsg(recvMsg,recvlen);
        msgParser tempParser(reMsg.c_str(),recvlen);
        processMSG(reMsg.c_str(),recvlen); 
        if(tempParser.msgTypeIs()==join_ack){
            rs_ip = s_ip;
            rs_port = s_port;
            break;
        }
        
    }
    
    
    
    
    return 1;
}

//add new users into the client list.

int chatClient::addNewUser(string name, string newCIP, int newCPort, int newCID){
    peer p;
    strncpy(p.name, name.c_str(),name.size());
    strncpy(p.ip, newCIP.c_str(),newCIP.size());
    p.port = newCPort;
    p.c_id = newCID;
    clientList.push_back(p);
    
    return 1;
}

//remove the specific user from the cientList
int chatClient::removeUser(int CID){
    unsigned int i=0;
    vector<peer>::iterator it;
    for (it=clientList.begin(); it!=clientList.end(); it++) {
        if((*it).c_id==CID){
            clientList.erase(it);
            return 1;
        }
    }
    return -1;
}
//function used to do the election.
//***NOTE:not bully here.***
//return 1 if it's elected to be leader.
//return -1 if not.
int chatClient::doElection(){
    myMsg tempMsg;
    string outmsg;
    int outlen;
    int i =0, maxID=0;
    peer nextLeader;
    vector<peer>::iterator tempit;
    if(status==NORMAL||status==ELEC_CLIENT){
        status = ELEC;
        for(tempit=clientList.begin();tempit!=clientList.end();tempit++){
            if ((*tempit).c_id>maxID) {
                maxID = (*tempit).c_id;
                nextLeader = (*tempit);
            }
        }
        if (maxID==C_ID) {
            status =ELEC_LEADER;            
            return 1;
        }
        else{
            //first notice all the peers that old sequencer is dead
            //then return -1 to indicate that itself is not the sequencer.
            tempMsg = mmaker.makeElec();
            status = ELEC_CLIENT;
            msgMaker::serialize(outmsg,outlen,tempMsg);
            pthread_mutex_lock(&udpMutex);
            clntUDP.updateSocket(nextLeader.ip,nextLeader.port);
            pthread_mutex_unlock(&udpMutex);

            if((clntUDP.sendToNACK(outmsg.c_str(),outlen))==-2){
                removeUser(maxID);
                return doElection();
            }
        }
        return -1;
    }
    return -1;
}

//function used to tell the sequencer this user is about to leave.

void chatClient::doLeave(){
    myMsg tempMsg = mmaker.makeLeave();
    int outlen =0;
    string outmsg ;
    msgMaker::serialize(outmsg,outlen,tempMsg);
    clntUDP.sendTo(outmsg.c_str(),outlen);
    cout<<"user: "<<name<<" is about to exit..."<<endl;
    exit(1);
}

//print out current members of client list, along with the leader, 
//i.e. the sequencer
//leader info need to be added into the clientList the sequencer 
//returns via the Join_ACK message.
void chatClient::displayClients(){
	int i=0;
	bool printleader = false;
	cout<<"Succeed, current users("<<(clientList.size()+1)<<" users):"<<endl;
    cout<<sname<<" "<<s_ip<<":"<<s_port<<" (leader)"<<endl;
	for(i=0;i<clientList.size();i++){    
        if(clientList[i].c_id==s_id){
            cout<<"c_id: "<<clientList[i].c_id<<" s_id: "<<s_id<<endl;
            cout<<"not print sequencer"<<endl;
            continue;
        }
        else{
            cout<<clientList[i].name<<" "<<clientList[i].ip<<":"<<clientList[i].port<<endl;
        }		
	}
}
//return the user's UID
int chatClient::getID(){
    return C_ID;
}
//return the current clientlist.
vector<peer> chatClient::getClientList(){
    return clientList;
}
