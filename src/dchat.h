// ===================================================================
// 
//       Filename:  dchat.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/15/2012 20:05:42
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ====================================================================
#ifndef __DCHAT_H__
#define __DCHAT_H__

#include <string>
#include <pthread.h>

#include "chatClient/chatClient.h"
#include "sequencer/sequencer.h"

using namespace std;

enum dchatType
{
    dServer,
    dClient
};

typedef struct threadArgs{
    string myIP;
    int myPort;
    int myID;
    string myName;
    string seqIP;
    int seqPort;
    int seqID;
    pthread_t mainID;
    sequencer *aSeq;
    chatClient *aClnt;
} threadArgs;

int getAPortNum();

void * uiInteract(void *args);

#endif
