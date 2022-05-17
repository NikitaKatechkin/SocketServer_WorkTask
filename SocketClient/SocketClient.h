#pragma once

#include <SocketLib/IncludeMe.h>
#include <iostream>

class SocketClient
{
public:
	SocketClient(const CustomSocket::IPEndpoint* endpoint = nullptr,
				 CustomSocket::IPVersion IPVersion = CustomSocket::IPVersion::IPv4);
	~SocketClient();

public:
	CustomSocket::Result connect(CustomSocket::IPEndpoint outEndpoint);
	CustomSocket::Result disconnect();

	CustomSocket::Result recieve(void* destination,
								 const uint16_t numberOfBytes);
	CustomSocket::Result send(const void* data,
							  const uint16_t numberOfBytes);
private:
	CustomSocket::Socket m_socket;
	CustomSocket::IPEndpoint m_IPConfig;
};