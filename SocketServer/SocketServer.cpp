#include "SocketServer.h"

#include <iostream>

SocketServer::SocketServer(CustomSocket::IPEndpoint endpoint, 
						   CustomSocket::IPVersion IPVersion, 
						   uint16_t backlog):
	m_listener(INVALID_SOCKET, IPVersion), m_IPConfig(endpoint), m_backlog(backlog)
{
	if (CustomSocket::NetworkAPIInitializer::Initialize() == false)
	{
		throw std::exception();
	}


	if (m_listener.create() == CustomSocket::Result::Success)
	{
		std::cout << "[SERVICE INFO]: ";
		std::cout << "Server succesfully created." << std::endl;
	}
	else
	{
		WSAGetLastError();
		throw std::exception();
	}

	if (m_listener.Listen(m_IPConfig, m_backlog) == CustomSocket::Result::Success)
	{
		std::cout << "[SERVICE INFO]: ";
		std::cout << "Server switched to listening state." << std::endl;
	}
	else
	{
		WSAGetLastError();
		throw std::exception();
	}
}

SocketServer::~SocketServer()
{
	if (m_isRunning == true)
	{
		stop();
	}

	CustomSocket::NetworkAPIInitializer::Shutdown();
}

void SocketServer::run()
{
	m_isRunning = true;
	m_listenThread = std::thread(&SocketServer::listenLoop, this);

	std::cout << "[SERVICE INFO]: ";
	std::cout << "Server is running..." << std::endl;
}

void SocketServer::stop()
{
	m_isRunning = false;
	m_connection.clear();

	CustomSocket::Socket fakeConnectionSocket;
	if (fakeConnectionSocket.create() == CustomSocket::Result::Success)
	{
		if (fakeConnectionSocket.Connect(m_IPConfig) == CustomSocket::Result::Success)
		{
			/**
			if (fakeConnectionSocket.close() == CustomSocket::Result::Fail)
			{
				throw std::exception();
			}
			**/
		}
		else
		{
			throw std::exception();
		}
	}
	else
	{
		throw std::exception();
	}
	

	m_listenThread.join();

	size_t numOfClients = 0;
	uint16_t* ports = getClientsPortList(numOfClients);

	if (ports != nullptr)
	{
		for (size_t index = 0; index < numOfClients; index++)
		{
			disconnect(ports[index]);
		}
	}

	std::cout << "[SERVICE INFO]: ";
	std::cout << "Server was stopped." << std::endl;
}

CustomSocket::Result SocketServer::disconnect(const uint16_t port)
{
	CustomSocket::Result result = CustomSocket::Result::Fail;

	for (uint16_t index = 0; index < m_backlog; index++)
	{
		if (m_connection[index].second.GetPort() == port)
		{
			std::cout << "[CLIENT]: " << "{IP = " << m_connection[index].second.GetIPString();
			std::cout << "} {PORT = " << m_connection[index].second.GetPort() << "} ";
			std::cout << "{STATUS = DISCONNECTED}" << std::endl;

			result = CustomSocket::Result::Success;

			m_connection[index].first.close();
			m_connection.erase(m_connection.begin() + index);

			break;
		}
	}

	return result;
}

CustomSocket::Result SocketServer::recieve(void* destination,
										   const uint16_t numberOfBytes,
										   const uint16_t port)
{
	CustomSocket::Result result = (destination == nullptr) ? CustomSocket::Result::Fail : 
															 CustomSocket::Result::Success;

	if (result == CustomSocket::Result::Success)
	{
		for (uint16_t index = 0; index < m_backlog; index++)
		{
			result = CustomSocket::Result::Fail;

			if (m_connection[index].second.GetPort() == port)
			{
				result = m_connection[index].first.Recieve(destination, numberOfBytes);

				break;
			}
		}
	}
	

	return result;
}

CustomSocket::Result SocketServer::send(const void* data, const uint16_t numberOfBytes, const uint16_t port)
{
	CustomSocket::Result result = (data == nullptr) ? CustomSocket::Result::Fail :
		CustomSocket::Result::Success;

	if (result == CustomSocket::Result::Success)
	{
		for (uint16_t index = 0; index < m_backlog; index++)
		{
			result = CustomSocket::Result::Fail;

			if (m_connection[index].second.GetPort() == port)
			{
				result = m_connection[index].first.Send(data, numberOfBytes);

				break;
			}
		}
	}


	return result;
}

/**
CustomSocket::Result SocketServer::extractConnection(CustomSocket::Socket& outSocket)
{
	WaitForSingleObject(m_nonEmptyEvent, INFINITE);

	CustomSocket::Result result = m_connection.empty() == true ? 
		CustomSocket::Result::Fail : CustomSocket::Result::Success;

	if (result == CustomSocket::Result::Success)
	{
		CustomSocket::Socket tmpSocket = m_connection.front();
		m_connection.erase(m_connection.begin());

		outSocket.setHandle(tmpSocket.getHandle());
		outSocket.setIPVersion(tmpSocket.getIPVersion());
	}
	else
	{
		std::cout << "[SERVICE INFO]: ";
		std::cout << "Connection pull is empty. Failed to extract new connection.";
		std::cout << std::endl;
	}

	return result;
}
**/

uint16_t* SocketServer::getClientsPortList(size_t& numOfClients)
{
	uint16_t* l_buffer = m_connection.empty() == true ? nullptr : 
														new uint16_t[m_connection.size()];
	numOfClients = m_connection.size();

	if (l_buffer != nullptr)
	{
		for (size_t index = 0; index < m_connection.size(); index++)
		{
			l_buffer[index] = m_connection[index].second.GetPort();
		}
	}

	return l_buffer;
}

bool SocketServer::hasConnectedClients()
{
	return !(m_connection.empty());
}

size_t SocketServer::getNumOfClients()
{
	return m_connection.size();
}

void SocketServer::listenLoop()
{
	while (m_isRunning == true)
	{
		if (waitForConnection() == CustomSocket::Result::Fail)
		{
			break;
		}
	}
}

CustomSocket::Result SocketServer::waitForConnection()
{
	/**
	if ((m_connection.empty() == true) &&
		(WaitForSingleObject(m_nonEmptyEvent, 0) == WAIT_OBJECT_0))
	{
		ResetEvent(m_nonEmptyEvent);
	}
	**/

	CustomSocket::Result result = m_connection.size() < m_backlog ? CustomSocket::Result::Success : 
																	CustomSocket::Result::Fail;

	if (result == CustomSocket::Result::Success)
	{
		CustomSocket::Socket newConnection;
		CustomSocket::IPEndpoint newConnectionIP;

		if (m_listener.Accept(newConnection, &newConnectionIP) == CustomSocket::Result::Success)
		{
			m_connection.push_back(CONNECTION_INFO(newConnection, newConnectionIP));

			std::cout << "[CLIENT]: " << "{IP = " << newConnectionIP.GetIPString();
			std::cout << "} {PORT = " << newConnectionIP.GetPort() << "} ";
			std::cout << "{STATUS = CONNECTED}" << std::endl;
		}
		else
		{
			std::cerr << "[SERVICE INFO]: ";
			std::cerr << "Failed to accept new connection. Socket error occured." << std::endl;
		}
	}
	else
	{
		std::cerr << "[SERVICE INFO]: ";
		std::cerr << "Failed to accept new connection. Server capacity exceeded." << std::endl;
	}

	return result;
}
