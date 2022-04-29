#include "Socket.h"
#include <iostream>

namespace CustomSocket
{
    Socket::Socket(SocketHandle handle, SocketIPVersion IPVersion):
        m_handle(handle), m_IPVersion(IPVersion)
    {
        if (m_IPVersion != SocketIPVersion::IPv4)
        {
            throw std::exception("USing non implemented socket IP version.");
        }
    }

    SocketResult Socket::create()
    {
        if (m_IPVersion != SocketIPVersion::IPv4)
        {
            throw std::exception("Using non implemented socket IP version.");
        }

        if (m_handle != INVALID_SOCKET)
        {
            return SocketResult::Fail;
        }

        m_handle = socket(AF_INET, 
                          SOCK_STREAM, 
                          IPPROTO_TCP);

        if (m_handle == INVALID_SOCKET)
        {
            WSAGetLastError();

            return SocketResult::Fail;
        }

        return SocketResult::Success;
    }

    SocketResult Socket::close()
    {
        if (m_handle == INVALID_SOCKET)
        {
            return SocketResult::Fail;
        }

        int result = closesocket(m_handle);

        if (result != 0)
        {
            WSAGetLastError();

            return SocketResult::Fail;
        }

        m_handle = INVALID_SOCKET;
        return SocketResult::Success;
    }

    SocketHandle Socket::getHandle()
    {
        return m_handle;
    }

    SocketIPVersion Socket::getIPVersion()
    {
        return m_IPVersion;
    }
}

