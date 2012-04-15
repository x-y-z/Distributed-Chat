// ===================================================================
// 
//       Filename:  test.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/13/2012 16:33:50
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ===================================================================
#include "msgMaker.h"
#include "msgParser.h"
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    msgMaker aMaker;
    myMsg aMsg;
    string msg;
    int mLen;

    string ip, name;
    int port, id;

    vector<peer> aList;
    peer first = {"zzz", "23.45.2.1", 987, 767};
    peer second = {"yyy", "33.23.11.33", 789, 53};
    aList.push_back(first);
    aList.push_back(second);

    aMaker.setInfo("yanzi", "127.0.0.1", 1234, 9999);

    //aMsg = aMaker.makeACK();
    //aMsg = aMaker.makeJoin("mm");
    //aMsg = aMaker.makeNavi();
    //aMsg = aMaker.makeJoinACK(34, 99, aList);
    //aMsg = aMaker.makeLeave();
    //aMsg = aMaker.makeLeaveBCast();
    //aMsg = aMaker.makeMsg("ni hao ma", 9);
    aMsg = aMaker.makeMsgBCast("ni hao ma", 9, 11);
    //aMsg = aMaker.makeElec();
    //aMsg = aMaker.makeElecOK();
    msgMaker::serialize(msg, mLen, aMsg);

    if (mLen != msg.size())
        cerr<<"alert!!\n";
    else
        cout<<"OK\n";

    cout<<"msg:"<<msg<<endl;

    msgParser aParser(msg.c_str(), mLen);
    int b = aParser.senderInfo(ip, port, id);
    msgType type = aParser.msgTypeIs();
    int a = aParser.joinName(name);
    
    int mmc, mid;
    vector<peer> jList;

    //aParser.joinFeedback(mmc, mid, jList);
    string gMsg;
    int seq_num;
    aParser.getMsg(seq_num, gMsg);
    cout<<"msg max cnt:"<<mmc<<", my id:"<<mid<<", list size:"<<jList.size()<<endl;
    
    cout<<"type is:"<<type<<", status:"<<b<<"  "<<a<<endl;

    cout<<"name is:"<<name<<", ip is:"<<ip<<", port:"<<port<<", id:"<<id<<", msg is:"<<gMsg<<"  "<<seq_num<<endl;   

}
