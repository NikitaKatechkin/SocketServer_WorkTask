#pragma once

#include "ServiceStructs.h"

namespace CustomSocket
{
	class Socket
	{
	public:
		Socket(SocketHandle handle = INVALID_SOCKET, 
			   IPVersion IPVersion = IPVersion::IPv4);
		~Socket() = default;

		Result create();
		Result close();

		SocketHandle getHandle();
		IPVersion getIPVersion();
	private:
		Result setSocketOption(Option option, BOOL value);
		SocketHandle m_handle = INVALID_SOCKET;
		IPVersion m_IPVersion = IPVersion::IPv4;
	};
}