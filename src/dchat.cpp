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


int main(int argc, char *argv[])
{
    string myName, myIP, seqName, seqIP;
    int myPort, seqPort;
    dchatType myType;
    threadArgs tArgs;

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
    tArgs.seqIP = seqIP;
    tArgs.seqPort = seqPort;
    tArgs.mainID = pthread_self();

    pthread_t uiThread;
    int threadRet;

    threadRet = pthread_create(&uiThread, NULL, uiInteract, (void*)&tArgs);

    sequencer aSeq(myName.c_str(), myIP.c_str(), myPort);
    chatClient aClnt(myName.c_str(), myIP.c_str(), myPort);

    //message handling loop
    while (mainRunning)
    {
        if (myType == dServer)
        {
        }
        else if (myType == dClient)
        {
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
    int running = 1;

    while(running)
    {
        std::cin>>running;
        if (running == 0)
        {
            mainRunning = 0;
        }

    }

    return 0;
}
