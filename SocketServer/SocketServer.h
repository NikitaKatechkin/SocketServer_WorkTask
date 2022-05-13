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

	CustomSocket::Result extractConnection(CustomSocket::Socket& outSocket);
private:
	void listenLoop();
	CustomSocket::Result waitForConnection();

private:
	CustomSocket::Socket m_listener;
	CustomSocket::IPEndpoint m_IPConfig;

	uint16_t m_backlog = 0;
	std::vector<CustomSocket::Socket> m_connection;

	bool m_isRunning = false;
	std::thread m_listenThread;

	HANDLE m_nonEmptyEvent;
};