// ===================================================================
// 
//       Filename:  crypt.h
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/20/2012 16:42:05
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Zi Yan (yz), zi.yan@gmx.com
//        Company:  
// 
// ====================================================================
#ifndef __CRYPT_H__
#define __CRYPT_H__

#include <openssl/ssl.h>
#include <openssl/rsa.h>

class msgCrypt
{
private:
    RSA *_myRSA;
public:
    msgCrypt();
    ~msgCrypt();
};

#endif
