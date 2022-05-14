#pragma once

#include <SocketLib/IncludeMe.h>
#include <iostream>

class SocketClient
{
public:
	SocketClient(CustomSocket::IPEndpoint endpoint,
				 CustomSocket::IPVersion IPVersion = CustomSocket::IPVersion::IPv4);
	~SocketClient();

public:
	CustomSocket::Result Connect(CustomSocket::IPEndpoint outEndpoint);
	CustomSocket::Result Disconnect();

	CustomSocket::Result recieve(void* destination,
								 const uint16_t numberOfBytes);
	CustomSocket::Result send(const void* data,
							  const uint16_t numberOfBytes);
private:
	CustomSocket::Socket m_socket;
	CustomSocket::IPEndpoint m_IPConfig;
};