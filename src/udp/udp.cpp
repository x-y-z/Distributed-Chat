#include "udp.h"
#include <iostream>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>


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

    init = true;

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

    init = true;
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

string UDP::getMyIP()
{
    char hostname[100];
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        std::cerr<<"get hostname error\n";
        exit(1);
    }

    struct hostent *phe = gethostbyname(hostname);
    if (phe == 0)
    {
        //for mac
        strncpy(hostname + strlen(hostname), ".local", 6);
        phe = gethostbyname(hostname);
        if (phe == 0)
        {
            std::cerr<<"look up for IP address error\n";
            exit(1);
        }
    }

    struct in_addr addr;
    memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));

    string ret(inet_ntoa(addr));

    return ret;
}

void UDP::updateSocket(const char *host, int port)
{
    cout<<"update, close first"<<endl;
    closesocket(_socket);
    init = false;

    setRemoteAddr(host, port);
    cout<<"update finished to IP: "<<host<<":"<<port<<endl;
}

void UDP::setRemoteAddr(const char *host, int port)
{
    if (init == true)
    {
        std::cerr<<"UDP: do not set remote addr twice\n";
        exit(1);
    }
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
    init = true;

}

int UDP::sendTo(const void *msg, size_t size,
                const struct sockaddr_in *dest, socklen_t dest_len)
{
    if (_socket == 0 || init == false)
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
    if (_socket == 0 || init == false)
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
            std::cerr<<"sendToNACK: sending error:"<<strerror(errno)<<endl;
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
        std::cerr<<"recvFromNACK: receive error:"<<strerror(errno)<<endl;
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


void *uniCast(void *arg)
{
    multiCastMsg *myArg = (multiCastMsg *)arg;
    int sRet, rRet;
    char rMsg[255];
    int finished = -1;

    while (finished <= 0)
    {
        struct sockaddr_in remoteAddr;
        int rLen;
        remoteAddr = UDP::fromAddrToSock(myArg->myInfo.ip, 
                                    myArg->myInfo.port);
        rLen = sizeof(remoteAddr);

        sRet = sendto(myArg->mySock, myArg->msg, myArg->mSize, 0,
                      (sockaddr *)&remoteAddr, (socklen_t)rLen);
        if (sRet < 0)
        {
            std::cerr<<"sendToNACK: sending error\n";
            exit(1);
        }

        //wait for ACK
        struct timeval tv;

        tv.tv_sec = 3;  /*  30 Secs Timeout */
        tv.tv_usec = 0;

        fd_set socks;
        FD_ZERO(&socks);
        FD_SET(myArg->mySock, &socks);

        if (select(myArg->mySock + 1, &socks, NULL, NULL, &tv))
        {
            char rMsg[255];
            int msgLen = recvfrom(myArg->mySock, rMsg, 
                                  255, 0, NULL, NULL);

            if (msgLen <= 0)
            {
                std::cerr<<"multiCast: recv ACK error\n";
                exit(1);
            }

            msgParser aParser(rMsg, msgLen);
            if (aParser.isACK())
            {
                finished = 100;
                //ACK received
            }
            else
            {
                std::cerr<<"multiCast: not ACK package, unexpected\n";
                exit(1);
            }
        }
        else
        {
            finished++;
        }
    }

    if (finished == 100)
        myArg->ret = 0;
    else
        myArg->ret = -1;

    closesocket(myArg->mySock);

    pthread_exit((void *)&((*myArg).ret));
}

vector<peer> UDP::multiCastNACK_T(const char *msg, size_t size,
                                const vector<peer> &clntList)
{
    multiCastMsg *argList;
    pthread_t *idList;
    pthread_attr_t attr;
    vector<peer> timeoutList;

    argList = new multiCastMsg[clntList.size()];
    idList = new pthread_t[clntList.size()];

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //initialize all socket
    for (int i = 0; i < clntList.size(); ++i)
    {
        struct protoent *ptrp;

        ptrp = getprotobyname("udp");
        if (ptrp == 0)
        {
            std::cerr<<"cannot map \"udp\" to protocol number\n";
            exit(1);
        }

        argList[i].mySock = socket(PF_INET, SOCK_DGRAM, ptrp->p_proto);
        if (argList[i].mySock < 0)
        {
            std::cerr<<"socket creation failed\n";
            exit(1);
        }

        argList[i].myInfo = clntList[i];
        argList[i].msg = msg;
        argList[i].mSize = size;

        //std::cerr<<"message broadcast:\n"<<"Socket:"<<argList[i].mySock
                 //<<" for "<<clntList[i].name<<" with id:"<<clntList[i].c_id
                 //<<", with ip:"<<clntList[i].ip<<":"<<clntList[i].port<<endl;

        int rc = pthread_create(&idList[i], &attr, uniCast, &(argList[i]));
        if (rc)
        {
            std::cerr<<"multiCast: return code from pthread_create is:"
                     <<rc<<std::endl;
        }
    }
    //create threads, use an array storing clntList
    
    pthread_attr_destroy(&attr);
    //each thread takes in charge of sending, resending, and return result
    //use pthread_exit to return normal/timeout result
    //use pthread_join to collect result
    for (int i = 0; i < clntList.size(); ++i)
    {
        int *status;
        int rc = pthread_join(idList[i], (void **)&status);

        if (*status == -1)
        {
            timeoutList.push_back(argList[i].myInfo);
        }

    }
    //return result
    return timeoutList;

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
//        std::cerr<<"message broadcast:\n"<<"Socket:"<<newSock
//                 <<" for "<<clntList[i].name<<" with id:"<<clntList[i].c_id
//                 <<", with ip:"<<clntList[i].ip<<":"<<clntList[i].port<<endl;
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

        int sRet = sendto((*iter).first, msg, size, 0, 
               (sockaddr*)&remoteAddr, rLen);

        if (sRet < 0)
        {
            std::cerr<<"multiCast: send error\n";
            exit(1);
        }
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
            if (selectNum == -1)
            {
                std::cerr<<"UDP: select error\n";
                exit(1);
            }
//            std::cerr<<"UDP:- There are "<<selectNum<<" out of "
//                     <<expectedNum<<" responses!\n";
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
//                        std::cerr<<"Close socket:"<<(*iter).first
//                                 <<" for ack recved\n";
                        closesocket((*iter).first);
                        FD_CLR((*iter).first, &socks);
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
//                    std::cerr<<"Close socket:"<<(*iter).first
//                             <<" for timeout\n";
                    closesocket(-(*iter).first);
                    FD_CLR(-(*iter).first, &socks);
                    sendList.erase(iter);
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

                    int sRet = sendto((*iter).first, msg, size, 0, 
                           (sockaddr*)&remoteAddr, rLen);
                    if (sRet < 0)
                    {
                        std::cerr<<"multiCast: resend error\n";
                        exit(1);
                    }

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
