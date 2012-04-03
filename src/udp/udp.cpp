#include "udp.h"
#include <iostream>


UDP::UDP(int port)
{
    struct protoent *ptrp;

    _type = server;

    memset((char *)&_addr, 0, sizeof(_addr));
    _addr.sin_port = htons((u_short)port);
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;

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

    if (bind(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
    {
        std::cerr<<"bind failed\n";
        exit(1);
    }

}

UDP::UDP(struct sockaddr_in addr)
{
    struct protoent *ptrp;
    
    _type = client;

    _addr = addr;

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

}

int UDP::sendTo(void *msg, size_t size,
                const struct sockaddr *dest, socklen_t dest_len)
{

}

int UDP::recvFrom(void *msg, size_t size,
                  struct sockaddr *src, socklen_t *src_len)
{

}

