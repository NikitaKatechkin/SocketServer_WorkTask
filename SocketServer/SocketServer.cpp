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

	m_nonEmptyEvent = CreateEvent(nullptr, TRUE, FALSE, L"ConnectionEvent");
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

	std::cout << "[SERVICE INFO]: ";
	std::cout << "Server was stopped." << std::endl;
}

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
	if ((m_connection.empty() == true) &&
		(WaitForSingleObject(m_nonEmptyEvent, 0) == WAIT_OBJECT_0))
	{
		ResetEvent(m_nonEmptyEvent);
	}

	CustomSocket::Result result = m_connection.size() < m_backlog ? CustomSocket::Result::Success : 
																	CustomSocket::Result::Fail;

	if (result == CustomSocket::Result::Success)
	{
		CustomSocket::Socket newConnection;
		if (m_listener.Accept(newConnection) == CustomSocket::Result::Success)
		{
			m_connection.push_back(newConnection);
			SetEvent(m_nonEmptyEvent);
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
