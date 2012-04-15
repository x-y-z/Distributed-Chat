// ===================================================================
// 
//       Filename:  test.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/14/2012 14:07:15
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ===================================================================
#include "sequencer.h"
#include <string>

using namespace std;

int main()
{
    sequencer aSeq("yanzi", "127.0.0.1", 4567);
    string aMsg;
    int amLen;
    msgMaker aMaker;

    aMaker.setInfo("yanzi", "127.0.0.1", 1234, 9999);
    msgMaker::serialize(aMsg, amLen, aMaker.makeJoin("yanzi"));

    aSeq.processMSG(aMsg.c_str(), aMsg.size());
}

