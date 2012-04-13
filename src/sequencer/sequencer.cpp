// ===================================================================
// 
//       Filename:  sequencer.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/13/2012 12:19:23
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ===================================================================
#include "sequencer.h"


sequencer::sequencer(const char* name, const char*ip, int port)
{
    my_name.assign(name);
    my_ip.assign(ip);
    my_port = port;
}

int sequencer::processMSG(myMsg msg)
{
    :

}

