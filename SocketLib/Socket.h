#pragma once

#include "SocketHandle.h"
#include "SocketResult.h"
#include "IPVersion.h"
#include "SocketOption.h"

namespace CustomSocket
{
	class Socket
	{
	public:
		Socket(SocketHandle handle = INVALID_SOCKET, 
			   SocketIPVersion IPVersion = SocketIPVersion::IPv4);
		~Socket() = default;

		SocketResult create();
		SocketResult close();

		SocketHandle getHandle();
		SocketIPVersion getIPVersion();
	private:
		SocketResult setSocketOption(SocketOption option, BOOL value);
		SocketHandle m_handle = INVALID_SOCKET;
		SocketIPVersion m_IPVersion = SocketIPVersion::IPv4;
	};
}