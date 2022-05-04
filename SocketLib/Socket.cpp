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

        Result result = Result::Fail;

        if (m_handle == INVALID_SOCKET)
        {
            m_handle = socket(AF_INET,
                              SOCK_STREAM,
                              IPPROTO_TCP);

            if (m_handle != INVALID_SOCKET)
            {
                result = (setSocketOption(Option::TCP_NoDelay, TRUE) != Result::Success) 
                    ? Result::Fail : Result::Success;
            }
        }

        return result;

        /**
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
        **/
    }

    Result Socket::close()
    {
        Result result = Result::Fail;

        if (m_handle != INVALID_SOCKET)
        {
            result = (closesocket(m_handle) != 0) ? Result::Fail : Result::Success;

            if (result != Result::Success)
            {
                WSAGetLastError();
            }
            else
            {
                m_handle = INVALID_SOCKET;
            }
        }

        return result;

        /**
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
        **/
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

        int code_result = 0;

        switch (option)
        {
        case Option::TCP_NoDelay:
        {
            code_result = setsockopt(m_handle, IPPROTO_TCP, TCP_NODELAY,
                (const char*)&value, sizeof(BOOL));
            break;
        }
        default:
            code_result = -1;
        }

        if (code_result != 0)
        {
            WSAGetLastError();
        }

        return (code_result != 0) ? Result::Fail : Result::Success;
    }
}

