#include "udp.h"
#include <iostream>
#include <sys/time.h>


UDP::UDP(int port)
{
    struct protoent *ptrp;

    _type = server;

    memset((char *)&_my_addr, 0, sizeof(_my_addr));
    memset((char *)&_remote, 0, sizeof(_remote));
    _r_len = sizeof(_remote);
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
                const struct sockaddr_in *dest, socklen_t dest_len)
{
    if (_socket == 0)
    {
        std::cerr<<"sendTo: socket not initialized\n";
        return -1;
    }

    return sendto(_socket, msg, size, 0, (const struct sockaddr*)dest, dest_len);
}

int UDP::sendTo(const void *msg, size_t size)
{
    return sendTo(msg, size, &_remote, _r_len);
}

int UDP::recvFrom(void *msg, size_t size,
                  struct sockaddr_in *src, socklen_t *src_len)
{
    if (_socket == 0)
    {
        std::cerr<<"recvFrom: socket not initialized\n";
        return -1;
    }

    int ret = recvfrom(_socket, msg, size, 0, (struct sockaddr*)src, src_len);

    return ret;
}

int UDP::recvFrom(void *msg, size_t size)
{
   return recvFrom(msg, size, &_remote, 
                   (socklen_t*)&_r_len); 

}

int UDP::recvFromTimeout(void *msg, size_t size)
{
    return recvFromTimeout(msg, size, 3);
}

int UDP::recvFromTimeout(void *msg, size_t size, int timeout)
{
    struct timeval tv;

    tv.tv_sec = timeout;  /*  30 Secs Timeout */
    tv.tv_usec = 0;

    fd_set socks;
    FD_ZERO(&socks);
    FD_SET(_socket, &socks);

    if (select(_socket + 1, &socks, NULL, NULL, &tv))
    {
        int msgLen = recvFrom(msg, size);
        return msgLen;
    }
    else
    {
        return SOCK_TIMEOUT;//for timeout
    }

}

int UDP::sendToNACK(const void *msg, size_t size)
{
    int sRet, rRet;
    char rMsg[255];
    int finished = -1;

    while (finished <= 0)
    {
        sRet = sendTo(msg, size);
        if (sRet < 0)
        {
            std::cerr<<"sendToNACK: sending error\n";
            exit(1);
        }

        //wait for ACK
        rRet = recvFromTimeout(rMsg, 255);
        if (rRet == SOCK_TIMEOUT)
        {
            finished++;
        }
        else if (rRet > 0)
        {
            msgParser aParser(rMsg, rRet);
            if (aParser.isACK())
            {
                finished = 100;
                //ACK received
            }
            else
            {
                std::cerr<<"sendToNACK: not ACK package, unexpected\n";
                exit(1);
            }
        }
    }

    if (finished == 100)
        return sRet;
    else
        return -2;

}

int UDP::recvFromNACK(void *msg, size_t size, 
                      const string &name, const string &ip,
                      int port, int id)
{
    int rRet, sRet;

    rRet = recvFrom(msg, size);
    if (rRet < 0)
    {
        std::cerr<<"recvFromNACK: receive error\n";
        exit(1);
    }
    msgMaker aMaker;
    int mLen;
    aMaker.setInfo(name, ip, port, id);
    string ackMsg;
    msgMaker::serialize(ackMsg, mLen, aMaker.makeACK());

    sRet = sendTo(ackMsg.c_str(), ackMsg.size());
    if (sRet < 0)
    {
        std::cerr<<"recvFromNACK: send ACK error\n";
        exit(1);
    }

    return rRet;
}

vector<peer> UDP::multiCastNACK(const void *msg, size_t size,
                                const vector<peer> &clntList)
{
    map<SOCKET, peer> sendList;
    vector<peer> timeoutList;

    //initialize all socket
    for (int i = 0; i < clntList.size(); ++i)
    {
        SOCKET newSock;
        struct protoent *ptrp;

        ptrp = getprotobyname("udp");
        if (ptrp == 0)
        {
            std::cerr<<"cannot map \"udp\" to protocol number\n";
            exit(1);
        }

        newSock= socket(PF_INET, SOCK_DGRAM, ptrp->p_proto);
        if (newSock < 0)
        {
            std::cerr<<"socket creation failed\n";
            exit(1);
        }

        sendList[newSock] = clntList[i];
    }

    map<SOCKET, peer>::iterator iter;
    //sending
    for (iter = sendList.begin(); iter != sendList.end(); iter++)
    {
        struct sockaddr_in remoteAddr;
        int rLen;
        remoteAddr = fromAddrToSock((*iter).second.ip, 
                                    (*iter).second.port);
        rLen = sizeof(remoteAddr);

        sendto((*iter).first, msg, size, 0, 
               (sockaddr*)&remoteAddr, rLen);
    }
    //wait for ack
    struct timeval tv;

    tv.tv_sec = 3;  /*  3 Secs Timeout */
    tv.tv_usec = 0;

    int nfds = 0;
    fd_set socks;
    FD_ZERO(&socks);
    for (iter = sendList.begin(); iter != sendList.end(); iter++)
    {
        FD_SET((*iter).first, &socks);
        if ((*iter).first > nfds)
            nfds = (*iter).first;
    }

    int ackNum = 0, expectedNum = clntList.size();
    while (ackNum < expectedNum)
    {
        char rackMsg[256];
        int selectNum;

        if ((selectNum = select(nfds + 1, &socks, NULL, NULL, &tv)) != 0)
        {
            for (iter = sendList.begin(); iter != sendList.end(); iter++)
            {
                if (FD_ISSET((*iter).first, &socks))
                {
                    int msgLen = recvfrom((*iter).first, rackMsg, 256, 0,
                                          NULL, NULL);
                    if (msgLen < 0)
                    {
                        std::cerr<<"multicast: recvfrom error\n";
                        exit(1);
                    }

                    msgParser aParser(rackMsg, msgLen);
                    if (aParser.isACK())
                    {
                        //this client is clear
                        sendList.erase(iter);
                        ackNum++;
                    }
                    else
                    {
                        std::cerr<<"multicast: not ACK, unexpected message\n";
                        exit(1);
                        /*struct sockaddr_in remoteAddr;
                        int rLen;
                        remoteAddr = fromAddrToSock((*iter).second.ip, 
                                                    (*iter).second.port);
                        rLen = sizeof(remoteAddr);

                        sendto((*iter).first, msg, size, 0, 
                               (sockaddr*)&remoteAddr, rLen);
                        */
                    }
                }
            }
        }
        else//time out
        {
            for (iter = sendList.begin(); iter != sendList.end(); iter++)
            {
                if ((*iter).first < 0)
                {
                    //this client is time out
                    timeoutList.push_back((*iter).second);
                    ackNum++;
                }
                else
                {
                    struct sockaddr_in remoteAddr;
                    int rLen;
                    remoteAddr = fromAddrToSock((*iter).second.ip, 
                                                (*iter).second.port);
                    rLen = sizeof(remoteAddr);

                    sendto((*iter).first, msg, size, 0, 
                           (sockaddr*)&remoteAddr, rLen);

                    int tmpS = -(*iter).first;
                    peer tmpP = (*iter).second;
                    sendList.erase(iter);
                    sendList[tmpS] = tmpP;
                }
            }
             
        }
    }

    return timeoutList;
}
