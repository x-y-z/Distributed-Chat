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

using namespace std;

int main()
{
    msgMaker aMaker;
    myMsg aMsg;
    string msg;
    int mLen;

    string ip, name;
    int port, id;

    aMaker.setInfo("yanzi", "127.0.0.1", 1234, 9999);

    //aMsg = aMaker.makeACK();
    aMsg = aMaker.makeJoin("mm");
    msgMaker::serlize(msg, mLen, aMsg);

    cout<<"msg:"<<msg<<endl;

    msgParser aParser(msg.c_str(), mLen);
    aParser.senderInfo(ip, port, id);
    int a = aParser.joinName(name);
    cout<<"status:"<<a<<endl;

    cout<<"name is:"<<name<<"ip is:"<<ip<<", port:"<<port<<", id:"<<id<<endl;   

}
