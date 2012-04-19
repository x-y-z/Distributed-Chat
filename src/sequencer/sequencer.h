// ===================================================================
// 
//       Filename:  sequencer.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/13/2012 12:18:43
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ====================================================================
#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__

#include "../msgType/msgParser.h"
#include "../msgType/msgMaker.h"
#include "../udp/udp.h"

#include <string>
#include <deque>


using namespace std;

enum seqStatus
{
    seqSuccess,
    seqJoinACKFail,
    seqJoinBCastFail,
    seqLeaveGhost,
    seqLeaveBCastTimeout,
    seqLeaveBCastNoClient,
    seqLeaveBCastNoName,
    seqNotReach
};

class sequencer
{
private:
    string my_ip;
    string my_name;
    int my_port;
    int my_id;
    vector<peer> clientList;

    int max_id;
    int msg_seq_id;
private:
    UDP _udp;
    deque<string> _MsgQ;

public:
    sequencer(const char* name, const char*ip, int port);
    ~sequencer(){};
public:
    seqStatus processMSG(const char *msg, int mlen);

    int getID(){ return my_id;};
    void setID(int aID){ my_id = aID; max_id = aID + 1;};

    void setMaxMsgID(int aID) {msg_seq_id = aID; };

    void printMemberList();
    
    int switchFromClient(const vector<peer> &aList, int myID, int maxMsgID);

private:
    //for join
    int newClientId(){ max_id++; return max_id;}
    int addToClientList(const string &name, const string &ip,
                        int port, int id);
    int nextMsgCnt(){ msg_seq_id++; return msg_seq_id; }
    int getMsgCurCnt(){ return msg_seq_id; }
    int sendJoinACK(const string &ip, int port, int id, int msgMaxCnt);
    int sendJoinBCast(const string &ip, int port, int id, const string &name);
    //for leave
    int findAndDeletePeer(int id);
    int sendLeaveBCast(const string &ip, int port, int id);
    //for msg
    int putMsgInQ(const string &ip, int port, int id, const string &msg);
    int sendMsgBCast();
    //for switch
    int setClientList(const vector<peer> &aList) {clientList = aList;};
    int sendLeaderBCast();
    //int waitForACK(const string &aMsg, int id, UDP &l_udp);
    
};


#endif
