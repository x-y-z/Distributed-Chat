//
//  
//
//  This is the structure for the message used in the distributed chat program.
//
//  Created by Dong Xin on 12-4-8.
//  Copyright (c) 2012  All rights reserved.
//

#ifndef __MYMSG_H__
#define __MYMSG_H__

#include <string>
#include <vector>
#include <ostream>

using namespace std;



typedef struct neighbor{
    char name[50];
    char ip[20];
    int port;
    int c_id;
} peer;

inline ostream& operator<<(ostream &o, const peer &n)
{
    return o<<"name is:"<<n.name<<endl<<"ip is:"<<n.ip<<endl
            <<"port is:"<<n.port<<endl<<"id is:"<<n.c_id<<endl;

}

typedef struct messageStruct {
    int sendORrev;//0 for send, 1 for ACK
    int chat;//0 for Join, 1 for navi 2 for ACK-Join, 3 for Join-broadcast, 
             //4 for Leave, 5 for Leave-broadcast, 
             //6 for MSG, 7 for MSG-broadcast, 
             //8 for election request, 9 for election OK, 10 leader broadcast.
    char ip[20];
    char name[20];
    int port;
    int self_id;
    int msgLen;
    char *msgContent;
} myMsg;

enum msgType{
    join,
    navi,
    join_ack,
    join_broadcast,
    leave,
    leave_broadcast,
    msg,
    msg_broadcast,
    election_req,
    election_ok,
    leader_broadcast,
    msgError
};
inline ostream& operator<<(ostream &o, const msgType &n)
{
    switch(n)
    {
        case join:
            return o<<"Join";
        case navi:
            return o<<"Navi";
        case join_ack:
            return o<<"Join-ACK";
        case join_broadcast:
            return o<<"Join-Broadcast";
        case leave:
            return o<<"Leave";
        case leave_broadcast:
            return o<<"Leave-Broadcast";
        case msg:
            return o<<"Message";
        case msg_broadcast:
            return o<<"Message-Broadcast";
        case election_req:
            return o<<"Election-Request";
        case election_ok:
            return o<<"Election-OK";
        case leader_broadcast:
            return o<<"I'm the leader";
        case msgError:
            return o<<"Msg Error";
        default:
            return o<<"invalid value";
    }
}

class msgParser
{
private:
    myMsg _content; /**< the message to be parsed */
    int _msgLen; /**< the length of the message*/
    bool init; /**< initiatioin status of the parser*/
public:
    /**
     * Default constructor
     */
    msgParser(){init = false;};
    /**
     * Alternative constructor
     * @param msg message to be parsed
     * @param len the length of the message
     */
    msgParser(const char *msg, int len);
    /**
     * destructor
     */
    ~msgParser(){};
public:
    /**
     * tell whether it is a ACK message
     * @ret is ACK or not
     */
    bool isACK();
    /**
     * get messaage type
     * @ret message type
     */
    msgType msgTypeIs();
    /**
     * get sender information
     * @param ip sender's ip
     * @param name sender's name
     * @param port sender's port number
     * @param id sender's id
     */
    int senderInfo(string &ip, string &name, int &port, int &id);
    /**
     * get the name of who joins
     * @param name the name
     */
    int joinName(string &name);
    /**
     * get current information in the group
     * @param msgMaxCnt maximum message number
     * @param my_id id assigned by sequencer
     * @param peerlist who else are in the chat group
     */
    int joinFeedback(int &msgMaxCnt, int &my_id, vector<peer> &peerlist); 
    /**
     * acquire message
     * @param text message content
     */
    int getMsg(string &text);
    /**
     * get message and the message sequence number
     */
    int getMsg(int &msg_seq, string &text);
    /**
     * get leader's name
     * @param name leader's name
     */
    int leaderName(string &name);
private:
    int msgContent(char *msg, int &msgLen);
};


#endif
