#include <iostream>
#include "udp.h"

int main()
{
    std::cout<<"hello\n";
    UDP::fromAddrToSock("www.google.com", 8080);
}
