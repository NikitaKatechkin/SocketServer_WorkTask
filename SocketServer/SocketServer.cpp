#include "SocketServer.h"

#include <iostream>
#include <algorithm>

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

	m_getInfoEvent = CreateEvent(nullptr, TRUE, FALSE, L"ServiceEvent");
	if (m_getInfoEvent == NULL)
	{
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
	if (m_listener.Listen(m_IPConfig, m_backlog) == CustomSocket::Result::Success)
	{
		std::lock_guard<std::mutex> print_lock(m_printLogMutex);

		std::cout << "[SERVICE INFO]: ";
		std::cout << "Server switched to listening state." << std::endl;
	}
	else
	{
		WSAGetLastError();
		throw std::exception();
	}

	m_isRunning = true;
	m_listenThread = std::thread(&SocketServer::listenLoop, this);
	

	std::lock_guard<std::mutex> print_lock(m_printLogMutex);

	std::cout << "[SERVICE INFO]: ";
	std::cout << "Server is running..." << std::endl;
}

void SocketServer::stop()
{
	m_isRunning = false;

	if (m_isFinished == false)
	{
		CustomSocket::Socket fakeConnectionSocket;
		if (fakeConnectionSocket.create() == CustomSocket::Result::Success)
		{
			{
				std::lock_guard<std::mutex> print_lock(m_printLogMutex);

				std::cout << "[SERVICE INFO]: " << "FAKE CONNECTION INITIATED" << std::endl;
			}

			if (fakeConnectionSocket.Connect(m_IPConfig) != CustomSocket::Result::Success)
			{
				throw std::exception();
			}
		}
		else
		{
			throw std::exception();
		}
	}

	m_listenThread.join();

	std::vector<uint16_t> ports = getClientsPortList();

	for (size_t index = 0; index < ports.size(); index++)
	{
		disconnect(ports[index]);
	}

	m_connection.clear();

	m_listener.close();

	std::lock_guard<std::mutex> print_lock(m_printLogMutex);

	std::cout << "[SERVICE INFO]: " << "FAKE CONNECTION TERMINATED" << std::endl;

	std::cout << "[SERVICE INFO]: ";
	std::cout << "Server was stopped." << std::endl;
}

CustomSocket::Result SocketServer::disconnect(const uint16_t port)
{
	/**
	CustomSocket::Result result = CustomSocket::Result::Fail;

	for (uint16_t index = 0; index < m_connection.size(); index++)
	{
		if (m_connection[index].second.GetPort() == port)
		{
			{
				std::lock_guard<std::mutex> print_lock(m_printLogMutex);

				std::cout << "[CLIENT]: " << "{IP = " << m_connection[index].second.GetIPString();
				std::cout << "} {PORT = " << m_connection[index].second.GetPort() << "} ";
				std::cout << "{STATUS = DISCONNECTED}" << std::endl;
			}

			result = CustomSocket::Result::Success;

			m_connection[index].first.close();
			m_connection.erase(m_connection.begin() + index);

			break;
		}
	}

	return result;
	**/

	auto portToDisconnect = std::find_if(
		m_connection.begin(),
		m_connection.end(),
		[&port](ConnectionInfo info) { return info.second.GetPort() == port; });

	CustomSocket::Result result = (portToDisconnect != m_connection.end()) ? CustomSocket::Result::Success :
																	  CustomSocket::Result::Fail;

	if (result == CustomSocket::Result::Success)
	{
		{
			std::lock_guard<std::mutex> print_lock(m_printLogMutex);

			std::cout << "[CLIENT]: " << "{IP = " << portToDisconnect->second.GetIPString();
			std::cout << "} {PORT = " << portToDisconnect->second.GetPort() << "} ";
			std::cout << "{STATUS = DISCONNECTED}" << std::endl;
		}

		//portToDisconnect->first.close();
		m_connection.erase(portToDisconnect);
	}

	return result;
}

CustomSocket::Result SocketServer::recieve(void* destination,
										   const uint16_t numberOfBytes,
										   const uint16_t port)
{
	CustomSocket::Result result = ((destination == nullptr) || (numberOfBytes == 0)) ?
																		CustomSocket::Result::Fail :
																		CustomSocket::Result::Success;

	if (result == CustomSocket::Result::Success)
	{
		auto find_iter = std::find_if(
			m_connection.begin(),
			m_connection.end(),
			[&port](ConnectionInfo info) { return info.second.GetPort() == port; });

		result = (find_iter != m_connection.end()) ? CustomSocket::Result::Success :
			CustomSocket::Result::Fail;


		if (result == CustomSocket::Result::Success)
		{
			result = find_iter->first.Recieve(destination, numberOfBytes);

			if (result == CustomSocket::Result::Success)
			{
				std::lock_guard<std::mutex> print_lock(m_printLogMutex);

				std::cout << "[CLIENT]: " << static_cast<char*>(destination) << std::endl;
			}
			else
			{
				std::lock_guard<std::mutex> print_lock(m_printLogMutex);

				std::cout << "[CLIENT]: " << "{ERROR WHILE RECIEVING MESSAGE}" << std::endl;
			}

		}
	}

	return result;
}

CustomSocket::Result SocketServer::send(const void* data, 
										const uint16_t numberOfBytes, 
										const uint16_t port)
{
	CustomSocket::Result result = ((data == nullptr) || (numberOfBytes == 0)) ? 
																	CustomSocket::Result::Fail :
																	CustomSocket::Result::Success;

	if (result == CustomSocket::Result::Success)
	{
		auto find_iter = std::find_if(
							m_connection.begin(),
							m_connection.end(),
							[&port](ConnectionInfo info) { return info.second.GetPort() == port; });

		result = (find_iter != m_connection.end()) ? CustomSocket::Result::Success : 
													 CustomSocket::Result::Fail;

		if (result == CustomSocket::Result::Success)
		{
			result = find_iter->first.Send(data, numberOfBytes);

			if (result == CustomSocket::Result::Success)
			{
				std::lock_guard<std::mutex> print_lock(m_printLogMutex);

				std::cout << "[SERVICE INFO]: " << "{ SENT MESSAGE = ";
				std::cout << static_cast<const char*>(data);
				std::cout << " } { NUMBER OF BYTES = " << static_cast<int>(numberOfBytes);
				std::cout << " }" << std::endl;
			}
			else
			{
				std::lock_guard<std::mutex> print_lock(m_printLogMutex);

				std::cout << "[CLIENT]: " << "{ERROR WHILE SENDING MESSAGE}" << std::endl;
			}
		}
	}

	return result;
}

void SocketServer::waitForClientToConnect()
{
	WaitForSingleObject(m_getInfoEvent, INFINITE);

	ResetEvent(m_getInfoEvent);
}

std::vector<uint16_t> SocketServer::getClientsPortList()
{
	std::vector<uint16_t> result;

	for (const auto & connection : m_connection)
	{
		result.push_back(connection.second.GetPort());
	}

	return result;
}

void SocketServer::listenLoop()
{
	while (m_isRunning == true)
	{
		if (m_connection.empty())
		{
			ResetEvent(m_getInfoEvent);
		}

		if ((waitForConnection() == CustomSocket::Result::Fail) && 
			(m_connection.size() < m_backlog))
		{
			ResetEvent(m_getInfoEvent);
			break;
		}
	}

	m_isFinished = true;
}

CustomSocket::Result SocketServer::waitForConnection()
{
	CustomSocket::Result result = m_connection.size() < m_backlog ? CustomSocket::Result::Success : 
																	CustomSocket::Result::Fail;

	if (result == CustomSocket::Result::Success)
	{
		CustomSocket::Socket newConnection;
		CustomSocket::IPEndpoint newConnectionIP;
		
		result = m_listener.Accept(newConnection, &newConnectionIP);

		if (result == CustomSocket::Result::Success)
		{
			m_connection.emplace_back(newConnection, newConnectionIP);
			SetEvent(m_getInfoEvent);

			std::lock_guard<std::mutex> print_lock(m_printLogMutex);

			std::cout << "[CLIENT]: " << "{IP = " << newConnectionIP.GetIPString();
			std::cout << "} {PORT = " << newConnectionIP.GetPort() << "} ";
			std::cout << "{STATUS = CONNECTED}" << std::endl;
		}
		else
		{
			std::lock_guard<std::mutex> print_lock(m_printLogMutex);

			std::cerr << "[SERVICE INFO]: ";
			std::cerr << "Failed to accept new connection. Socket error occured." << std::endl;
		}
	}
	else
	{
		std::lock_guard<std::mutex> print_lock(m_printLogMutex);

		std::cerr << "[SERVICE INFO]: ";
		std::cerr << "Failed to accept new connection. Server capacity exceeded." << std::endl;
	}

	return result;
}
