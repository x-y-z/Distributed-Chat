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
        //int ret = server.recvFrom(buf, 255, &client, (socklen_t*)&c_len);
        int ret = server.recvFromNACK(buf, 255, "yanzi", 
                    "127.0.0.1", S_PORT, 1234);
        if (ret == -2)
            std::cerr<<"Timeout\n";
        std::cout<<buf<<"  "<<ret<<std::endl;
        std::cin>>buf;
        int ret1 = server.sendToNACK(buf, strlen(buf));///, &client, (socklen_t)c_len);
        if (ret1 == -2)
            std::cerr<<"Timeout\n";

    }
    else if (type == 1)
    {
        //struct sockaddr_in svr;
        //int s_len;
        char buf[255];
        //svr = UDP::fromAddrToSock("127.0.0.1",S_PORT);
        //s_len = sizeof(svr);

        std::cin>>buf;
        UDP client;
        client.setRemoteAddr("127.0.0.1", S_PORT);

        int ret = client.sendToNACK(buf, strlen(buf));
        if (ret == -1)
        {
            perror("sendto");
            exit(1);
        }
        if (ret == -2)
            std::cerr<<"Timeout\n";

        int ret1 = client.recvFromNACK(buf, 255,
                "ziyan", "127.0.0.1", 23453, 9878);
        std::cout<<buf<<std::endl;
    }
    else//2
    {
        char buf[255];
        peer p1 = {"yyy", "127.0.0.1", 1234, 1},
             p2 = {"zzz", "127.0.0.1", 2345, 2};
        vector<peer> sList;
        sList.push_back(p1);
        sList.push_back(p2);

        UDP client;
        std::cin>>buf;
        sList = client.multiCastNACK(buf, strlen(buf), sList);

        for (int i = 0; i < sList.size(); i++)
        {
            std::cout<<sList[i];
        }
    }

    std::cout<<"Bye bye\n";
    return 0;
}
