// ===================================================================
// 
//       Filename:  dchat.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/15/2012 19:54:25
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com, Xin Dong
//        Company:  
// 
// ===================================================================
#include <cstdlib>
#include <time.h>
#include <iostream>
#include "dchat.h"
#include "sequencer/sequencer.h"
#include "chatClient/chatClient.h"
#include "udp/udp.h"

int mainRunning = 1;
int uiRunning = 0;
int uiSuspend = 0;
int deQueue = 0;
bool updateUDP =false;
dchatType myType;
UDP msgSender;
#define MAX_MSG_LEN 2048

deque<string> globalMsgQ;

pthread_mutex_t uiMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t qMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t uiCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t qNotFull= PTHREAD_COND_INITIALIZER;
pthread_cond_t qNotEmpty= PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[])
{
    string myName, myIP, seqName, seqIP;
    int myPort, seqPort;
    int myID, seqID;
    
    threadArgs tArgs;
    pthread_t uiThread;
    int threadRet;

    mainArgs mArgs;
    pthread_t mainThread;
    int mainRet;

    if (argc != 2 && argc != 3)
    {
        std::cout<<"Usage: "<<argv[0]<<" USER <IP:PORT>"<<endl;
        exit(1);
    }

    if (argc == 2)//server
    {
        myType = dServer;
        myName.assign(argv[1]);
#ifdef DEBUG
       std::cout<<"my name is:"<<myName<<endl;
#endif
    }
    else if (argc == 3)
    {
        myName.assign(argv[1]);
        myType = dClient;
        string ipNport(argv[2]);
        
        int delim = ipNport.find(":");
        seqIP = ipNport.substr(0, delim);

        seqPort = atoi(ipNport.substr(delim + 1).c_str());
#ifdef DEBUG
        std::cout<<"my name is:"<<myName<<" server IP is:"<<seqIP<<" port is:"
                 <<seqPort<<endl;
#endif
    }
    //get my own ip, port
    myIP = UDP::getMyIP();
    myPort = getAPortNum();

    if (myType == dServer)
    {
        seqIP = myIP;
        seqPort = myPort;
        seqName = myName;
        std::cout<<myName<<" started a new chat, listening on "
                 <<myIP<<":"<<myPort<<endl;
    }
    else if (myType == dClient)
    {
        std::cout<<myName<<" joining a new chat on: "<<seqIP<<":"<<seqPort
                 <<",listening on "<<myIP<<":"<<myPort<<endl;
    }
    else
    {
        std::cerr<<"Undefined machine type\n";
        exit(1);
    }
    //get started
    UDP listener(myPort);


    //ui information
    tArgs.seqIP = seqIP;
    tArgs.seqPort = seqPort;
    tArgs.myIP = myIP;
    tArgs.myPort = myPort;
    tArgs.myName = myName;
    tArgs.mainID = pthread_self();


    threadRet = pthread_create(&uiThread, NULL, uiInteract, (void*)&tArgs);

    sequencer aSeq(myName.c_str(), myIP.c_str(), myPort);
    chatClient aClnt(myName.c_str(), myIP.c_str(), myPort);
    
    tArgs.aSeq = &aSeq;
    tArgs.aClnt = &aClnt;

    if (myType == dServer)
    {
        tArgs.myID = myID = aSeq.getID();
        std::cout<<"Succeeded, current users:"<<endl;
        aSeq.printMemberList();
        std::cout<<"Wait for others to join..."<<endl;

    }
    else // for client
    {
        
        aClnt.dojoin(seqIP,seqPort,listener);
        cout<<"after join, leader is :"<<seqIP<<":"<<seqPort<<endl;
        tArgs.seqIP = seqIP;
        tArgs.seqPort = seqPort;
        tArgs.myID = myID = aClnt.getID();
    }

    
    //main recv msg loop
    mArgs.listener = &listener;
    mArgs.globalQ = &globalMsgQ;
    mainRet = pthread_create(&mainThread, NULL, mainRecv, (void*)&mArgs);

    //release ui to run
    pthread_mutex_lock(&uiMutex);
    uiRunning = 1;
    pthread_cond_signal(&uiCond);
    pthread_mutex_unlock(&uiMutex);

    //message handling loop
    while (mainRunning)
    {
        string recvMsg;

        pthread_mutex_lock(&qMutex);
        while (globalMsgQ.empty())
        {
            pthread_cond_wait(&qNotEmpty, &qMutex);
        }
        recvMsg = globalMsgQ.front();
        globalMsgQ.pop_front();
        pthread_mutex_unlock(&qMutex);
        pthread_cond_signal(&qNotFull);
        
        if (myType == dServer)
        {
            seqStatus pRet = aSeq.processMSG(recvMsg.c_str(), 
                                             recvMsg.size());
        }
        else if (myType == dClient)
        {
            int clientRV=0;
            
            clientRV= aClnt.msgEnqueue(recvMsg.c_str(), recvMsg.size());
            if( clientRV==10){
                myType = dServer;
                myID = aClnt.getID();
                vector<peer> peerList;
                int maxMsgId = aClnt.getMaxCnt();

                peerList = aClnt.getClientList();
                cout<<"About to switch from client to sequencer!"<<endl;
                aSeq.switchFromClient(peerList, myID, maxMsgId);
                
                pthread_mutex_lock(&uiMutex);
                cout<<"about to reset msgSender"<<endl;
                msgSender.updateSocket(myIP.c_str(),myPort);
                cout<<"now the sequencer is: "<<myIP<<":"<<myPort<<endl;
                pthread_mutex_unlock(&uiMutex);
                cout<<"After Election"<<endl;
                aSeq.printMemberList();
            }
            else if(clientRV==9){
                pthread_mutex_lock(&uiMutex);
                msgSender.updateSocket((aClnt.getSIP()).c_str(),aClnt.getSPort());
                pthread_mutex_unlock(&uiMutex);
            }
            
        }

    }


    pthread_join(uiThread, NULL);

    return 0;
}

int getAPortNum()
{
    int portNum = 0;
    srand(time(NULL));
    portNum = rand() % 500 + 5000;

    return portNum;
}

void * mainRecv(void *args)
{
    mainArgs *mArgs = (mainArgs *)args;
    string myName = mArgs->myName;
    string myIP = mArgs->myIP;
    int myPort = mArgs->myPort;
    int myID = mArgs->myID;
    char recvMsg[MAX_MSG_LEN];
    string inMsg;

    int recvLen;
   
    while (1)
    {
        recvLen = mArgs->listener->recvFromNACK(recvMsg, MAX_MSG_LEN,
                                       myName, myIP, myPort, myID);
        if (recvLen < 0)
        {
            std::cerr<<"recv error, retrying..."<<endl;
            continue;
        }
 
        pthread_mutex_lock(&qMutex);
        while (mArgs->globalQ->size() >= 10)
        {
            pthread_cond_wait(&qNotFull, &qMutex);
        }


        inMsg.assign(recvMsg, recvLen);
        mArgs->globalQ->push_back(inMsg);

        pthread_mutex_unlock(&qMutex);
        pthread_cond_signal(&qNotEmpty);
    }
}


void * uiInteract(void *args)
{
    threadArgs *outArgs = (threadArgs *)args;
    
    string myName = outArgs->myName;
    string myIP = outArgs->myIP;
    int myPort = outArgs->myPort;
    int myID = outArgs->myID;
    pthread_t mainPID = outArgs->mainID;
    msgMaker aMaker;

    

    //wait for main loop signal
    pthread_mutex_lock(&uiMutex);
    while (!uiRunning)
        pthread_cond_wait(&uiCond, &uiMutex);    
    pthread_mutex_unlock(&uiMutex);
    
    aMaker.setInfo(myName, myIP, myPort, myID);
    msgSender.setRemoteAddr(outArgs->seqIP.c_str(), outArgs->seqPort);
    //running ui
    while(uiRunning)
    {
        string input;
        string outMsg;
        int outMsgLen;
        
        
        // get user input
        getline(cin, input);
        // handle existing situation
        if (cin.eof() == 1)
        {
            uiRunning = 0;
            mainRunning = 0;
            //send leave message
            if(myType==dServer){    
                msgMaker::serialize(outMsg, outMsgLen,
                                    aMaker.makeLeave());
                msgSender.sendToNACK(outMsg.c_str(), outMsg.size());
            }
            else{
                outArgs->aClnt->doLeave();
            }
            exit(0);
        }

        input = myName + ":: " + input;
        // send message to sequencer
        // no msg sending while election or special situation
        if (!uiSuspend)
        {
            
            if(myType==dClient){
                msgMaker::serialize(outMsg, outMsgLen,
                aMaker.makeMsg(input.c_str(), input.size())); 
                pthread_mutex_lock(&uiMutex);
                int tempRV = msgSender.sendToNACK(outMsg.c_str(), 
                                        outMsg.size());
                pthread_mutex_unlock(&uiMutex);
                if(tempRV==-2){
                    aMaker.setInfo(myName, myIP, myPort, 
                                   outArgs->aClnt->getID());
                    myMsg tempMsg = aMaker.makeElec();
                    string tempoutmsg;
                    int templen =0;
                    msgMaker::serialize(tempoutmsg,templen,tempMsg);
                    
                    UDP tmpSender;
                    tmpSender.setRemoteAddr(myIP.c_str(), myPort);
                    tmpSender.sendToNACK(tempoutmsg.c_str(), templen);
                }
                
            }
            else{
                msgMaker::serialize(outMsg, outMsgLen,
                                    aMaker.makeMsg(input.c_str(), 
                                    input.size())); 
                msgSender.sendToNACK(outMsg.c_str(), outMsg.size());
            }
            
        }
    }

    return 0;
}
