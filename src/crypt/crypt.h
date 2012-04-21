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
public:
    int public_encrypt(const char *from, char *to, int msgLen);
    int private_decrypt(const char *from, char *to, int msgLen);

    int private_encrypt(const char *from, char *to, int msgLen);
    int public_decrypt(const char *from, char *to, int msgLen);

public:
    static int public_encrypt(RSA *rsa, const char *from, char *to, int msgLen);
    static int private_decrypt(RSA *rsa, const char *from, char *to, 
                              int msgLen);

    static int private_encrypt(RSA *rsa, const char *from, char *to, 
                               int msgLen);
    static int public_decrypt(RSA *rsa, const char *from, char *to, 
                               int msgLen);
};

#endif
