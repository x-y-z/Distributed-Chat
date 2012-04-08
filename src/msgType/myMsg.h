//
//  myMsg.h
//
//  This is the structure for the message used in the distributed chat program.
//
//  Created by Dong Xin on 12-4-8.
//  Copyright (c) 2012年  All rights reserved.
//

#ifndef _myMsg_h
#define _myMsg_h
#define MAXLENGTH 255

typedef struct messageStruct {
    int sendOrev;//0 for send, 1 for ACK
    int chat;//0 for Join, 1 for ACK-Join, 2 for Join-broadcast, 3 for Leave, 4 for Leave-broadcast, 5 for MSG, 6 for MSG-broadcast.
    int election;//0 for request, 1 for OK.
    char[MAXLENGTH] msgContent;
    
} myMsg;

#endif
