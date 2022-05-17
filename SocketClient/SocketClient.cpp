#include "SocketClient.h"

SocketClient::SocketClient(const CustomSocket::IPEndpoint* endpoint,
						   CustomSocket::IPVersion IPVersion):
	m_socket(INVALID_SOCKET, IPVersion)
{
	if (CustomSocket::NetworkAPIInitializer::Initialize() == false)
	{
		throw std::exception();
	}

	if (CustomSocket::NetworkAPIInitializer::Initialize() == false)
	{
		throw std::exception();
	}

	if (m_socket.create() == CustomSocket::Result::Success)
	{
		std::cout << "[SERVICE INFO]: ";
		std::cout << "Client successfully created.";

		if (endpoint != nullptr)
		{
			m_IPConfig = new CustomSocket::IPEndpoint(*endpoint);
			if (m_socket.Bind(*m_IPConfig) == CustomSocket::Result::Success)
			{
				std::cout << "[SERVICE INFO]: ";
				std::cout << "Client successfully started from ip = ";
				std::cout << m_IPConfig->GetIPString();
				std::cout << " on port " << m_IPConfig->GetPort() << "." << std::endl;
			}
			else
			{
				throw std::exception();
			}
		}
	}
	else
	{
		throw std::exception();
	}
}

SocketClient::~SocketClient()
{
	if (m_socket.getHandle() != INVALID_SOCKET)
	{
		m_socket.close();
	}

	if (m_IPConfig != nullptr)
	{
		delete m_IPConfig;
	}

	CustomSocket::NetworkAPIInitializer::Shutdown();
}

CustomSocket::Result SocketClient::connect(CustomSocket::IPEndpoint outEndpoint)
{
	CustomSocket::Result result = m_socket.Connect(outEndpoint);

	if (result == CustomSocket::Result::Success)
	{
		std::cout << "[SERVICE INFO]: ";
		std::cout << "Client was successfully connected to point ";
		std::cout << "{IP = " << outEndpoint.GetIPString() << "} {PORT = ";
		std::cout << outEndpoint.GetPort() << "}." << std::endl;
	}
	else
	{
		std::cout << "[SERVICE INFO]: ";
		std::cout << "Failed to connect client to point ";
		std::cout << "{IP = " << outEndpoint.GetIPString() << "} {PORT = ";
		std::cout << outEndpoint.GetPort() << "}." << std::endl;
	}

	return result;
}

CustomSocket::Result SocketClient::disconnect()
{
	CustomSocket::Result result = m_socket.close();

	
	if (result == CustomSocket::Result::Success)
	{
		result = m_socket.create();

		if (result == CustomSocket::Result::Success)
		{
			if (m_IPConfig != nullptr)
			{
				result = m_socket.Bind(*m_IPConfig);

				if (result == CustomSocket::Result::Success)
				{
					std::cout << "[SERVICE INFO]: ";
					std::cout << "Client was successfully disconnected." << std::endl;
				}
			}
		}
	}

	if (result == CustomSocket::Result::Fail)
	{
		std::cout << "[SERVICE INFO]: ";
		std::cout << "Failed to disconnect client." << std::endl;
	}
	else
	{
		std::cout << "[SERVICE INFO]: ";
		std::cout << "Client successfully disconnected." << std::endl;
	}

	return result;
}

CustomSocket::Result SocketClient::recieve(void* destination, 
										   const uint16_t numberOfBytes)
{
	CustomSocket::Result result = (destination == nullptr) ? CustomSocket::Result::Fail :
		CustomSocket::Result::Success;

	if (result == CustomSocket::Result::Success)
	{
		result = m_socket.Recieve(destination, numberOfBytes);

		if (result == CustomSocket::Result::Success)
		{
			std::cout << "[SERVER]: " << static_cast<char*>(destination) << std::endl;
		}
		else
		{
			std::cout << "[SERVER]: " << "{ERROR WHILE RECIEVING MESSAGE}" << std::endl;
		}

	}

	return result;
}

CustomSocket::Result SocketClient::send(const void* data, const uint16_t numberOfBytes)
{
	CustomSocket::Result result = (data == nullptr) ? CustomSocket::Result::Fail :
		CustomSocket::Result::Success;

	if (result == CustomSocket::Result::Success)
	{
		result = m_socket.Send(data, numberOfBytes);

		if (result == CustomSocket::Result::Success)
		{
			std::cout << "[SERVICE INFO]: " << "{ SENT MESSAGE = ";
			std::cout << static_cast<const char*>(data);
			std::cout << " } { NUMBER OF BYTES = " << static_cast<int>(numberOfBytes);
			std::cout << " }" << std::endl;
		}
		else
		{
			std::cout << "[SERVICE INFO]: " << "{ERROR WHILE SENDING MESSAGE}" << std::endl;
		}

	}

	return result;
}
