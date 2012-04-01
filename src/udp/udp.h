#ifndef __UDP_H__
#define __UDP_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


class UDP
{
public:
    UDP(){};
    ~UDP(){};

private:
    int _socket;
    struct sockaddr_in _address;
    struct sockaddr_in _peer_list;

}


#endif
