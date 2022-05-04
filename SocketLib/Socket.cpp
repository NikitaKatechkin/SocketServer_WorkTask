#include "Socket.h"
#include <iostream>

namespace CustomSocket
{
    Socket::Socket(SocketHandle handle, IPVersion IPVersion):
        m_handle(handle), m_IPVersion(IPVersion)
    {
        if (m_IPVersion != IPVersion::IPv4)
        {
            throw std::exception("USing non implemented socket IP version.");
        }
    }

    Result Socket::create()
    {
        if (m_IPVersion != IPVersion::IPv4)
        {
            throw std::exception("Using non implemented socket IP version.");
        }

        if (m_handle != INVALID_SOCKET)
        {
            return Result::Fail;
        }

        m_handle = socket(AF_INET, 
                          SOCK_STREAM, 
                          IPPROTO_TCP);

        if (m_handle == INVALID_SOCKET)
        {
            WSAGetLastError();

            return Result::Fail;
        }

        if (setSocketOption(Option::TCP_NoDelay, TRUE) != Result::Success)
        {
            return Result::Fail;
        }

        return Result::Success;
    }

    Result Socket::close()
    {
        if (m_handle == INVALID_SOCKET)
        {
            return Result::Fail;
        }

        int result = closesocket(m_handle);

        if (result != 0)
        {
            WSAGetLastError();

            return Result::Fail;
        }

        m_handle = INVALID_SOCKET;
        return Result::Success;
    }

    SocketHandle Socket::getHandle()
    {
        return m_handle;
    }

    IPVersion Socket::getIPVersion()
    {
        return m_IPVersion;
    }
    
    Result Socket::setSocketOption(Option option, BOOL value)
    {

        int result = 0;

        switch (option)
        {
        case Option::TCP_NoDelay:
            result = setsockopt(m_handle, IPPROTO_TCP, TCP_NODELAY, 
                                (const char*)& value, sizeof(BOOL));
            break;
        default:
            return Result::Fail;
        }

        if (result != 0)
        {
            WSAGetLastError();

            return Result::Fail;
        }

        return Result::Success;
    }
}

