// ===================================================================
// 
//       Filename:  crypt.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/20/2012 16:42:02
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ===================================================================
#include "crypt.h"


msgCrypt::msgCrypt()
{
    _myRSA = RSA_generate_key(1024, 65537, NULL, NULL);
}


msgCrypt::~msgCrypt()
{
    RSA_free(_myRSA);
}

