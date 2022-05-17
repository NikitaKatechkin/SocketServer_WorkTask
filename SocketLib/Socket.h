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
		~Socket(); //Make socket close after using

		Result create();
		Result close();

		Result Bind(IPEndpoint endpooint); //May be private
		Result Listen(IPEndpoint endpoint, int backlog = 5);

		Result Accept(Socket& outSocket, IPEndpoint* outEndpoint = nullptr);
		Result Connect(IPEndpoint endpoint);

		//Check if nullptr in data or negative numOfBytes TO-DO
		//uint16_t to NumOfBytes

		Result Send(const void* data, int numberOfBytes);
		Result Recieve(void* data, int numberOfBytes);

		SocketHandle getHandle();
		IPVersion getIPVersion();

		void setHandle(SocketHandle handle);
		void setIPVersion(IPVersion ipVersion);
	protected:
		Result ServiceSend(const void* data, const int numberOfBytes, int& bytesSent); // should be private
		Result ServiceRecieve(void* destination, int numberOfBytes, int& bytesRecieved); // should be private

		Result setSocketOption(Option option, BOOL value);

	protected:
		SocketHandle m_handle = INVALID_SOCKET;
		IPVersion m_IPVersion = IPVersion::IPv4;
	};
}