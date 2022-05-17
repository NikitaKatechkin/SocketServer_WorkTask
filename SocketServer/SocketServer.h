#pragma once

#include <SocketLib/IncludeMe.h>
#include <thread>
#include <mutex>

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

	std::vector<uint16_t> getActualClientsPortList();
	std::vector<uint16_t> getClientsPortList();

private:
	void listenLoop();
	CustomSocket::Result waitForConnection();

private:
	using ConnectionInfo = std::pair<CustomSocket::Socket, CustomSocket::IPEndpoint>;

private:
	CustomSocket::Socket m_listener;
	CustomSocket::IPEndpoint m_IPConfig;

	uint16_t m_backlog = 0;
	std::vector<ConnectionInfo> m_connection;

	bool m_isRunning = false;
	bool m_isFinished = false;

	std::mutex m_printLogMutex;
	std::thread m_listenThread;

	HANDLE m_getInfoEvent;
};