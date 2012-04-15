#ifndef __UDP_H__
#define __UDP_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>


#define closesocket close
#define SOCKET int

enum udp_type
{
    client,
    server
};

enum udp_state
{
    wait_ack,
    all_done
};

class UDP
{
public:
    UDP():_socket(0){};
    UDP(int port);//server
    UDP(struct sockaddr_in addr);//client
    ~UDP(){ closesocket(_socket);};

public:
    static struct sockaddr_in fromAddrToSock(const char *host, const int port);
    void setRemoteAddr(const char *host, int port);
private:
    void setListenPort(int port);
    void setRemoteAddr(struct sockaddr_in addr);
    

    //int close(){};
public:
    int sendTo(const void *msg, size_t size, 
               const struct sockaddr_in *dest, socklen_t dest_len);
    int recvFrom(void *msg, size_t size,
               struct sockaddr_in *src, socklen_t *src_len);
public:
    int sendTo(const void *msg, size_t size);
    int recvFrom(void *msg, size_t size);

    int recvFromTimeout(void *msg, size_t size);

    int recvFromTimeout(void *msg, size_t size, int timeout);

private:
    udp_type _type;//0--client, 1--server
//    udp_state _state;
    SOCKET _socket;
    struct sockaddr_in _my_addr;
    struct sockaddr_in _remote;
    int _r_len;
};


#endif
