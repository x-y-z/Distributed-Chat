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

int msgCrypt::public_encrypt(const char *from, char *to, int msgLen)
{

}

int msgCrypt::private_decrypt(const char *from, char *to, int msgLen)
{

}

int msgCrypt::private_encrypt(const char *from, char *to, int msgLen)
{

}

int msgCrypt::public_decrypt(const char *from, char *to, int msgLen)
{

}

static int msgCrypt::public_encrypt(RSA *rsa, const char *from, char *to, int msgLen)
{
    int en_max_size = RSA_size(rsa) - 11;//for RSA_PKCS1_PADDING
    int residualLen = msgLen;

    while (residualLen > 0)
    {
        int curLen = msgLen - residualLen;
        int en_size = en_max_size > residualLen?residualLen:en_max_size;
        RSA_public_encrypt(en_size,
                           from + curLen,
                           to + curLen,
                           rsa,
                           RSA_PKCS1_PADDING);
    }

}

static int msgCrypt::private_decrypt(RSA *rsa, const char *from, char *to,
                              int msgLen)
{

}

static int msgCrypt::private_encrypt(RSA *rsa, const char *from, char *to,
                               int msgLen)
{

}

static int msgCrypt::public_decrypt(RSA *rsa, const char *from, char *to,
                               int msgLen)
{

}

