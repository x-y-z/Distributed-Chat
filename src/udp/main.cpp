#include <iostream>
#include "udp.h"

#define S_PORT 2345

int main()
{
    int type;
    std::cout<<"hello\n";
    std::cin>>type;


    if (type == 0)
    {
        struct sockaddr client;
        int c_len;
        char buf[255];
        UDP server(S_PORT);
        int ret = server.recvFrom(buf, 255, &client, (socklen_t*)&c_len);
        std::cout<<buf<<std::endl;
        std::cin>>buf;
        int ret1 = server.sendTo(buf, strlen(buf), &client, (socklen_t)c_len);
    }
    else
    {
        struct sockaddr_in svr;
        int s_len;
        char buf[255];
        svr = UDP::fromAddrToSock("127.0.0.1",S_PORT);
        s_len = sizeof(svr);

        std::cin>>buf;
        UDP client(svr);

        int ret = client.sendTo(buf, strlen(buf), (struct sockaddr *)&svr, s_len);
        if (ret == -1)
        {
            perror("sendto");
            exit(1);
        }
        int ret1 = client.recvFrom(buf, 255, (struct sockaddr *)&svr, (socklen_t*)&s_len);
        std::cout<<buf<<std::endl;
    }

    std::cout<<"Bye bye\n";
    return 0;
}
