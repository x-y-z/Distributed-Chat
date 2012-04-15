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
    int processMSG(const char *msg, int mlen);

private:
    //for join
    int newClientId(){ max_id++; return max_id - 1;}
    int addToClientList(const string &name, const string &ip,
                        int port, int id);
    int getMsgMaxCnt(){ msg_seq_id++; return msg_seq_id - 1; }
    int sendJoinACK(const string &ip, int port, int id, int msgMaxCnt);
    int sendJoinBCast(const string &ip, int port, int id, const string &name);
    //for leave
    int findAndDeletePeer(int id);
    int sendLeaveBCast(const string &ip, int port, int id);
    //for msg
    int putMsgInQ(const string &ip, int port, int id, const string &msg);
    int sendMsgBCast();

    int waitForACK(const string &aMsg, int id, UDP &l_udp);
    
};


#endif
