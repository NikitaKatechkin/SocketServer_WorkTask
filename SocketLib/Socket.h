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
		~Socket(); 

		Result create();
		Result close();

		Result Bind(IPEndpoint endpooint); 
		Result Listen(IPEndpoint endpoint, int backlog = 5);

		Result Accept(Socket& outSocket, IPEndpoint* outEndpoint = nullptr);
		Result Connect(IPEndpoint endpoint);

		Result Send(const void* data, int numberOfBytes, int* bytesSent = nullptr);
		Result Recieve(void* data, int numberOfBytes, int* bytesRecieved = nullptr);

		SocketHandle getHandle();
		IPVersion getIPVersion();

		void setHandle(SocketHandle handle);
		void setIPVersion(IPVersion ipVersion);
	protected:
		Result ServiceSend(const void* data, const int numberOfBytes, int& bytesSent); 
		Result ServiceRecieve(void* destination, int numberOfBytes, int& bytesRecieved); 

		Result setSocketOption(Option option, BOOL value);

	protected:
		SocketHandle m_handle = INVALID_SOCKET;
		IPVersion m_IPVersion = IPVersion::IPv4;
	};
}