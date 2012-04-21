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
    /**
     * Default constructor
     */
    UDP():_socket(0),init(false){};
    /**
     * Server constructor
     * @param port server's listening port
     */
    UDP(int port);//server
    /**
     * Client constructor
     * @param addr server address
     */
    UDP(struct sockaddr_in addr);//client
    /**
     * Deconstructor
     *
     * close opened socket
     */
    ~UDP(){ closesocket(_socket);};

public:
    /**
     * static method, convert ip:port to sockaddr_in format
     * @param host point to host string
     * @param port port number
     */
    static struct sockaddr_in fromAddrToSock(const char *host, const int port);
    /**
     * return the machine's ip address
     * @ret machines' ip address
     */
    static string getMyIP();
    /**
     * set server address for client
     * @param host remote host name or ip
     * @param port remote host port number
     */
    void setRemoteAddr(const char *host, int port);
    /**
     * update server address for client
     * @param host remote host name or ip
     * @param port remote host port number
     */
    void updateSocket(const char *host, int port);
    
public:
    /**
     * Send to a remote address
     * @param msg message to be sent
     * @param size size of the message
     * @param dest destination of the message
     * @param dest_len sockaddr_in size
     * @ret the number of bytes sent or error message
     */
    int sendTo(const void *msg, size_t size, 
               const struct sockaddr_in *dest, socklen_t dest_len);
    /**
     * Receive message from outside
     * @param msg message to be sent
     * @param size size of the message
     * @param dest source of the message
     * @param dest_len sockaddr_in size
     * @ret the number of bytes got or error message
     */
    int recvFrom(void *msg, size_t size,
               struct sockaddr_in *src, socklen_t *src_len);
public:
    /**
     * Send to a remote address
     * @param msg message to be sent
     * @param size size of the message
     * @ret the number of bytes sent or error message
     */
    int sendTo(const void *msg, size_t size);
    /**
     * Receive message from outside
     * @param msg message to be sent
     * @param size size of the message
     * @ret the number of bytes got or error message
     */
    int recvFrom(void *msg, size_t size);
    /**
     * Receive a message with 3 second timeout
     * @param msg message to be sent
     * @param size size of the message
     * @ret the number of bytes sent or error message
     */
    int recvFromTimeout(void *msg, size_t size);
    /**
     * Receive a message with timeout
     * @param msg message to be sent
     * @param size size of the message
     * @param timeout the time period for time out
     * @ret the number of bytes sent or error message
     */
    int recvFromTimeout(void *msg, size_t size, int timeout);
public:
    /**
     * Send to a remote address, get time out 3 second, wait for ack
     * @param msg message to be sent
     * @param size size of the message
     * @ret the number of bytes sent or error message
     */
    int sendToNACK(const void *msg, size_t size);
    /**
     * wait for message, send ack immediately, no timeout
     * @param msg message to be sent
     * @param size size of the message
     * @param name my name
     * @param ip my ip
     * @param port my port
     * @param id my id
     * @ret the number of bytes sent or error message
     */
    int recvFromNACK(void *msg, size_t size,
                      const string &name, const string &ip,
                      int port, int id);
    /**
     * broadcast and wait for ACK, use threads
     * @param msg message to be sent
     * @param size size of the message
     * @param clntList the list of machines to be sent
     * @ret timeout machines
     */
    vector<peer> multiCastNACK_T(const char *msg, size_t size, 
                const vector<peer> &clntList);
    /**
     * broadcast and wait for ACK, use select
     * @param msg message to be sent
     * @param size size of the message
     * @param clntList the list of machines to be sent
     * @ret timeout machines
     */
    vector<peer> multiCastNACK(const void *msg, size_t size, 
                const vector<peer> &clntList);

private:
    udp_type _type; /**< 0--client, 1--server */
    bool init; /**< socket initialized or not*/
    SOCKET _socket; /**< internal socket */
    struct sockaddr_in _my_addr; /**< server address */
    struct sockaddr_in _remote; /**< remote address */
    int _r_len; /**< remote address length */
};


#endif
