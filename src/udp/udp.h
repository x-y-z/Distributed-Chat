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

#include <map>

#include "../msgType/msgMaker.h"
#include "../msgType/msgParser.h"

using namespace std;

#define closesocket close
#define SOCKET int
#define SOCK_TIMEOUT -2

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

typedef struct multiCastMsg{
    SOCKET mySock;
    peer myInfo;
    const char *msg;
    int mSize;
    int ret;
} multiCastMsg;

class UDP
{
public:
    UDP():_socket(0),init(false){};
    UDP(int port);//server
    UDP(struct sockaddr_in addr);//client
    ~UDP(){ closesocket(_socket);};

public:
    static struct sockaddr_in fromAddrToSock(const char *host, const int port);
    static string getMyIP();
    void setRemoteAddr(const char *host, int port);
    void updateSocket(const char *host, int port);
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
public:
    //get time out 3 second, wait for ack
    int sendToNACK(const void *msg, size_t size);
    //wait for message, send ack immediately, no timeout
    int recvFromNACK(void *msg, size_t size,
                      const string &name, const string &ip,
                      int port, int id);
    //broadcast and wait for ACK
    vector<peer> multiCastNACK_T(const char *msg, size_t size, 
                const vector<peer> &clntList);
    vector<peer> multiCastNACK(const void *msg, size_t size, 
                const vector<peer> &clntList);

private:
    udp_type _type;//0--client, 1--server
    bool init;
//    udp_state _state;
    SOCKET _socket;
    struct sockaddr_in _my_addr;
    struct sockaddr_in _remote;
    int _r_len;
};


#endif
