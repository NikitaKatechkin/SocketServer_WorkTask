#pragma once

#include <SocketLib/IncludeMe.h>
#include <thread>

class SocketServer final
{
public:
	SocketServer(CustomSocket::IPEndpoint endpoint, 
				 CustomSocket::IPVersion IPVersion = CustomSocket::IPVersion::IPv4,
				 uint16_t backlog = 5);
	~SocketServer();

public:
	void run();
	void stop();

	CustomSocket::Result disconnect(const uint16_t port);

	CustomSocket::Result recieve(void* destination, 
								 const uint16_t numberOfBytes,
								 const uint16_t port);
	CustomSocket::Result send(const void* data,
							  const uint16_t numberOfBytes,
							  const uint16_t port);

	//CustomSocket::Result extractConnection(CustomSocket::Socket& outSocket);
	uint16_t* getClientsPortList(size_t& numOfClients);
	bool hasConnectedClients();
	size_t getNumOfClients();
private:
	void listenLoop();
	CustomSocket::Result waitForConnection();

private:
	using CONNECTION_INFO = std::pair<CustomSocket::Socket, CustomSocket::IPEndpoint>;

private:
	CustomSocket::Socket m_listener;
	CustomSocket::IPEndpoint m_IPConfig;

	uint16_t m_backlog = 0;
	std::vector<CONNECTION_INFO> m_connection;

	bool m_isRunning = false;
	std::thread m_listenThread;
};