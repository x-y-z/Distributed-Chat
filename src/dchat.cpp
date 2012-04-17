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

pthread_mutex_t uiMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t uiCond = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[])
{
    string myName, myIP, seqName, seqIP;
    int myPort, seqPort;
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
    tArgs.seqIP = seqIP;
    tArgs.seqPort = seqPort;
    tArgs.myIP = myIP;
    tArgs.myPort = myPort;
    tArgs.myName = myName;
    tArgs.mainID = pthread_self();


    threadRet = pthread_create(&uiThread, NULL, uiInteract, (void*)&tArgs);

    sequencer aSeq(myName.c_str(), myIP.c_str(), myPort);
    chatClient aClnt(myName.c_str(), myIP.c_str(), myPort);


    //release ui to run
    pthread_mutex_lock(&uiMutex);
    uiRunning = 1;
    pthread_cond_signal(&uiCond);
    pthread_mutex_unlock(&uiMutex);

    //message handling loop
    while (mainRunning)
    {
        /*string test;
        if (uiRunning == 0)
        {
            cout<<"main loop:";
            cin>>test;
        }
        if (test == "run")
        {
            pthread_mutex_lock(&uiMutex);
            uiRunning = 1;
            pthread_cond_signal(&uiCond);
            pthread_mutex_unlock(&uiMutex);

        }*/

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
    UDP msgSender;
    string myName = outArgs->myName;
    string myIP = outArgs->myIP;
    int myPort = outArgs->myPort;

    msgSender.setRemoteAddr(outArgs->seqIP.c_str(), outArgs->seqPort);

    pthread_mutex_lock(&uiMutex);
    while (!uiRunning)
        pthread_cond_wait(&uiCond, &uiMutex);
    pthread_mutex_unlock(&uiMutex);

    while(uiRunning)
    {
        string input;

        // get user input
        getline(cin, input);
        if (cin.eof() == 1)
        {
            uiRunning = 0;
            mainRunning = 0;
            continue;
        }

        cout<<myName<<": "<<input<<endl;
        // send message to sequencer
        // handle existing situation
        
    }

    return 0;
}
