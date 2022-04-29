#include "SocketNetworkInitializer.h"
#include <iostream>

bool CustomSocket::NetworkHandler::Initialize()
{
    WSADATA data;

    int result = WSAStartup(MAKEWORD(2, 2), &data);
    
    if (result != 0)
    {
        std::cerr << "Failed to start up the winsock API." << std::endl;

        return false;
    }

    if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2)
    {
        std::cerr << "Could not find a correct version of the winsock API." << std::endl;

        return false;
    }


    return true;
}

void CustomSocket::NetworkHandler::Shutdown()
{
    WSACleanup();
}
