#ifndef __UDP_H__
#define __UDP_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
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
    UDP(int port);
    UDP(struct sockaddr_in addr);
    ~UDP(){ closesocket(_socket);};

public:
    static struct sockaddr_in fromAddrToSock(const char *host, const int port);
public:
    void setListenPort(int port);
    void setRemoteAddr(struct sockaddr_in addr);
    void setRemoteAddr(const char *host, int port);

    //int close(){};
    int sendTo(void *msg, size_t size, 
               const struct sockaddr *dest, socklen_t dest_len);
    int recvFrom(void *msg, size_t size,
               struct sockaddr *src, socklen_t *src_len);

private:
    udp_type _type;//0--client, 1--server
    udp_state _state;
    SOCKET _socket;
    struct sockaddr_in _addr;
    struct sockaddr_in _peer_list;

};


#endif
