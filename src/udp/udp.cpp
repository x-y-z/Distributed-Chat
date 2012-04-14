#include "udp.h"
#include <iostream>


UDP::UDP(int port)
{
    struct protoent *ptrp;

    _type = server;

    memset((char *)&_my_addr, 0, sizeof(_my_addr));
    _my_addr.sin_port = htons((u_short)port);
    _my_addr.sin_family = AF_INET;
    _my_addr.sin_addr.s_addr = INADDR_ANY;

    ptrp = getprotobyname("udp");
    if (ptrp == 0)
    {
        std::cerr<<"cannot map \"udp\" to protocol number\n";
        exit(1);
    }

    _socket = socket(PF_INET, SOCK_DGRAM, ptrp->p_proto);
    if (_socket < 0)
    {
        std::cerr<<"socket creation failed\n";
        exit(1);
    }

    if (bind(_socket, (struct sockaddr *)&_my_addr, sizeof(_my_addr)) < 0)
    {
        std::cerr<<"bind failed\n";
        exit(1);
    }

}

UDP::UDP(struct sockaddr_in addr)
{
    struct protoent *ptrp;
    
    _type = client;
    _remote = addr;
    _r_len = sizeof(_remote);

    ptrp = getprotobyname("udp");
    if (ptrp == 0)
    {
        std::cerr<<"cannot map \"udp\" to protocol number\n";
        exit(1);
    }

    _socket = socket(PF_INET, SOCK_DGRAM, ptrp->p_proto);
    if (_socket < 0)
    {
        std::cerr<<"socket creation failed\n";
        exit(1);
    }


}

struct sockaddr_in UDP::fromAddrToSock(const char *host, const int port)
{
    struct sockaddr_in tmp_addr;
    struct hostent *ptrh;

    ptrh = gethostbyname(host);
    if (ptrh == NULL)
    {
        std::cerr<<"invalid host: "<<host<<std::endl;
        exit(1);
    }

    memcpy(&tmp_addr.sin_addr, ptrh->h_addr, ptrh->h_length);
    tmp_addr.sin_port = htons((u_short)port);
    tmp_addr.sin_family = AF_INET;

    return tmp_addr;
}

void UDP::setListenPort(int port)
{

}

void UDP::setRemoteAddr(struct sockaddr_in addr)
{

}

void UDP::setRemoteAddr(const char *host, int port)
{
    _remote = fromAddrToSock(host, port);
    _r_len = sizeof(_remote);
    struct protoent *ptrp;
    
    _type = client;

    ptrp = getprotobyname("udp");
    if (ptrp == 0)
    {
        std::cerr<<"cannot map \"udp\" to protocol number\n";
        exit(1);
    }

    _socket = socket(PF_INET, SOCK_DGRAM, ptrp->p_proto);
    if (_socket < 0)
    {
        std::cerr<<"socket creation failed\n";
        exit(1);
    }

}

int UDP::sendTo(const void *msg, size_t size,
                const struct sockaddr *dest, socklen_t dest_len)
{
    if (_socket == 0)
    {
        std::cerr<<"sendTo: socket not initialized\n";
        return -1;
    }

    return sendto(_socket, msg, size, 0, dest, dest_len);
}

int UDP::sendTo(const void *msg, size_t size)
{
    return sendTo(msg, size,(struct sockaddr *)&_remote, _r_len);
}

int UDP::recvFrom(void *msg, size_t size,
                  struct sockaddr *src, socklen_t *src_len)
{
    if (_socket == 0)
    {
        std::cerr<<"recvFrom: socket not initialized\n";
        return -1;
    }

    return recvfrom(_socket, msg, size, 0, src, src_len);
}

int UDP::recvFrom(void *msg, size_t size)
{
   return recvFrom(msg, size, (struct sockaddr *)&_remote, 
                   (socklen_t*)&_r_len); 

}

