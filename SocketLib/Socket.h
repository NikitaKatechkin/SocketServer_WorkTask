#pragma once

#include "ServiceStructs.h"
#include "IPEndpoint.h"

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

		Result Bind(IPEndpoint endpooint);
		Result Listen(IPEndpoint endpoint, int backlog = 5);

		Result Accept(Socket& outSocket);
		Result Connect(IPEndpoint endpoint);

		SocketHandle getHandle();
		IPVersion getIPVersion();
	private:
		Result setSocketOption(Option option, BOOL value);
		SocketHandle m_handle = INVALID_SOCKET;
		IPVersion m_IPVersion = IPVersion::IPv4;
	};
}