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
    seqNotReach,
    seqMsgBCastTimeout,
    seqMsgBCastNoClient,
    seqUnexpectedMsg
};
inline ostream& operator<<(ostream &o, const seqStatus &n)
{
    

    switch(n)
    {
        case seqSuccess:
            return o<<"Seq: Success";
        case seqJoinACKFail:
            return o<<"Seq: Join ACK Fail";
        case seqJoinBCastFail:
            return o<<"Seq: Join BCast Fail";
        case seqLeaveGhost:
            return o<<"Seq: A Ghost Left";
        case seqLeaveBCastTimeout:
            return o<<"Seq: Leave BCast some time out";
        case seqLeaveBCastNoClient:
            return o<<"Seq: Leave BCast No Client";
        case seqLeaveBCastNoName:
            return o<<"Seq: Leave BCast No name for leaver";
        case seqNotReach:
            return o<<"Seq: Not reachable code";
        case seqMsgBCastTimeout:
            return o<<"Seq: Msg BCast some time out";
        case seqMsgBCastNoClient:
            return o<<"Seq: Msg BCast No Client";
        case seqUnexpectedMsg:
            return o<<"Seq: Unexpected Message";
        //case msgError:
        //    return o<<"Msg Error";
        default:
            return o<<"invalid value";
    }
}
class sequencer
{
private:
    string my_ip; /**< sequencer's ip */
    string my_name; /**< sequncer's name*/
    int my_port; /**< sequencer's port*/
    int my_id; /**< sequencer's id*/
    vector<peer> clientList; /**< the list of all clients, 
                               sequencer not included */

    int max_id; /**< current assigned max id*/
    int msg_seq_id; /**< maximum message number*/
private:
    UDP _udp; /**< message sending udp*/
    deque<string> _MsgQ; /**< used to store not processed message*/

public:
    /**
     * constructor, assigning name, ip, and port
     */
    sequencer(const char* name, const char*ip, int port);
    /**
     * destructor
     */
    ~sequencer(){};
public:
    /**
     * processing all kinds of message received by sequencer
     */
    seqStatus processMSG(const char *msg, int mlen);
    /**
     * get the id of the sequencer
     */
    int getID(){ return my_id;};
    /**
     * set the id of the sequencer
     */
    void setID(int aID){ my_id = aID; max_id = aID + 1;};
    /**
     * set maximum message number
     */
    void setMaxMsgID(int aID) {msg_seq_id = aID; };
    /**
     * print out current member list
     */
    void printMemberList();
    /**
     * switching from client to sequencer, preset some necessary information
     */
    int switchFromClient(const vector<peer> &aList, int myID, int maxMsgID);

private:
    //below are all for message processing, they are self-explained
    //for join
    int newClientId(){ max_id++; return max_id;}
    int addToClientList(const string &name, const string &ip,
                        int port, int id);
    int nextMsgCnt(){ msg_seq_id++; return msg_seq_id; }
    int getMsgCurCnt(){ return msg_seq_id; }
    int sendJoinACK(const string &ip, int port, int id, int msgMaxCnt);
    int sendJoinBCast(const string &ip, int port, int id, const string &name);
    //for leave
    peer findAndDeletePeer(int id);
    int sendLeaveBCast(const peer &someOne);
    //for msg
    int putMsgInQ(const string &ip, int port, int id, const string &msg);
    int sendMsgBCast();
    //for switch
    int setClientList(const vector<peer> &aList) {clientList = aList;};
    int sendLeaderBCast();
    int sendLeader(const string &ip, int port, int id);
    
};


#endif
