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

#include <string>

#include "../client/client.h"

using namespace std;

class sequencer
{
private:
    string my_ip;
    string my_name[50];
    int my_port;
    vector<peer> clientList(DEFAULT_CLIENT_CAPACITY);

private:
    int msg_seq_id;
public:
    sequencer(const char* name, const char*ip, int port);
    ~sequencer(){};
public:
    int processMSG(myMsg msg);
};


#endif
