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
//         Author:  Zi Yan (yz), zi.yan@gmx.com
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

#define MAX_MSG_LEN 2048

pthread_mutex_t uiMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t uiCond = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[])
{
    string myName, myIP, seqName, seqIP;
    int myPort, seqPort;
    int myID, seqID;
    dchatType myType;
    threadArgs tArgs;
    pthread_t uiThread;
    int threadRet;

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


    tArgs.seqIP = seqIP;
    tArgs.seqPort = seqPort;
    tArgs.myIP = myIP;
    tArgs.myPort = myPort;
    tArgs.myName = myName;
    tArgs.mainID = pthread_self();


    threadRet = pthread_create(&uiThread, NULL, uiInteract, (void*)&tArgs);

    sequencer aSeq(myName.c_str(), myIP.c_str(), myPort);
    chatClient aClnt(myName.c_str(), myIP.c_str(), myPort);

    if (myType == dServer)
    {
        tArgs.myID = myID = aSeq.getID();
        std::cout<<"Succeeded, current users:"<<endl;
        aSeq.printMemberList();
        std::cout<<"Wait for others to join..."<<endl;

    }
    else // for client
    {
        
        aClnt.dojoin(seqIP,seqPort);
        tArgs.myID = myID = aClnt.getID();
        aClnt.displayClients();
    }

    

    //release ui to run
    pthread_mutex_lock(&uiMutex);
    uiRunning = 1;
    pthread_cond_signal(&uiCond);
    pthread_mutex_unlock(&uiMutex);

    //message handling loop
    while (mainRunning)
    {
        char recvMsg[MAX_MSG_LEN];
        int recvMsgLen;
        
        recvMsgLen = listener.recvFromNACK(recvMsg, MAX_MSG_LEN, myName, myIP, 
                              myPort, myID);

        if (myType == dServer)
        {
            int pRet = aSeq.processMSG(recvMsg, recvMsgLen);
            if (pRet != 0)
                std::cerr<<"something wrong\n";
        }
        else if (myType == dClient)
        {
            int clientRV=0;
            clientRV= aClnt.msgEnqueue(recvMsg,recvMsgLen);
            
	        if( clientRV==10){
                myType = dServer;
                //switch to sequencer, and broadcast the " I am the leader " message
                //(handle time out by deleting and broadcasting leave messages)
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
    portNum = rand() % 500 + 1025;

    return portNum;
}

void * uiInteract(void *args)
{
    threadArgs *outArgs = (threadArgs *)args;
    UDP msgSender;
    string myName = outArgs->myName;
    string myIP = outArgs->myIP;
    int myPort = outArgs->myPort;
    int myID = outArgs->myID;
    pthread_t mainPID = outArgs->mainID;
    msgMaker aMaker;

    aMaker.setInfo(myName, myIP, myPort, myID);
    msgSender.setRemoteAddr(outArgs->seqIP.c_str(), outArgs->seqPort);

    //wait for main loop signal
    pthread_mutex_lock(&uiMutex);
    while (!uiRunning)
        pthread_cond_wait(&uiCond, &uiMutex);
    pthread_mutex_unlock(&uiMutex);

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
            pthread_cancel(mainPID);
            continue;
        }

        input = myName + ":: " + input;
        //cout<<myName<<": "<<input<<endl;
        // send message to sequencer
        // no msg sending while election or special situation
        if (!uiSuspend)
        {
            msgMaker::serialize(outMsg, outMsgLen,
                            aMaker.makeMsg(input.c_str(), input.size())); 
            msgSender.sendToNACK(outMsg.c_str(), outMsg.size());
        }
    }

    return 0;
}
