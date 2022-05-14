//#include <SocketLib/IncludeMe.h>
#include <SocketServer/SocketServer.h>
#include <iostream>

int main()
{
	/**
	if (CustomSocket::NetworkAPIInitializer::Initialize())
	{
		std::cout << "The winsock API was successfully initialized." << std::endl;

		CustomSocket::Socket socket;
		if (socket.create() == CustomSocket::Result::Success)
		{
			std::cout << "The socket was successfully created." << std::endl;

			if (socket.Listen(CustomSocket::IPEndpoint("127.0.0.1", 4790))
				== CustomSocket::Result::Success)
			{
				std::cout << "Socket successfully listening on port 4790." << std::endl;

				CustomSocket::Socket newConnection;
				if (socket.Accept(newConnection) == CustomSocket::Result::Success)
				{
					char buffer[256];
					//int bytesRecieved = 0;

					CustomSocket::Result result = CustomSocket::Result::Fail;
					while (result != CustomSocket::Result::Success)
					{
						//result = newConnection.Recieve(buffer, 256, bytesRecieved);
						result = newConnection.Recieve(buffer, 256);

						if (result == CustomSocket::Result::Success)
						{
							std::cout << "[CLIENT]: " << buffer << std::endl;
							break;
						}
					}

					newConnection.close();
				}
				else
				{
					std::cerr << "Failed to accept connection on a socket." << std::endl;
				}
			}
			else
			{
				std::cerr << "Failed to listen a socket on port 4790." << std::endl;
			}

			if (socket.close() == CustomSocket::Result::Success)
			{
				std::cout << "The socket was successfully closed." << std::endl;
			}
			else
			{
				std::cout << "Failed to close a socket." << std::endl;
			}
		}
		else
		{
			std::cout << "Failed to create a socket." << std::endl;
		}
	}

	CustomSocket::NetworkAPIInitializer::Shutdown();
	**/

	const CustomSocket::IPEndpoint IPSettings("127.0.0.1", 4790);
	const uint16_t bufferSize = 256;
	const char message[bufferSize] = "Hello world from server\0";

	SocketServer server(IPSettings);

	server.run();

	for (size_t index = 0; index < 3; index++)
	{
		size_t numOfClients = 0;
		uint16_t* ports = server.getClientsPortList(numOfClients);

		if (ports != nullptr)
		{
			for (size_t index = 0; index < numOfClients; index++)
			{
				char recieveBuffer[bufferSize];
				CustomSocket::Result result = server.recieve(recieveBuffer,
					bufferSize,
					ports[index]);

				/**
				if (result == CustomSocket::Result::Success)
				{
					std::cout << "[CLIENT]: " << recieveBuffer << std::endl;
				}
				else
				{
					std::cout << "[CLIENT]: " << "{ERROR WHILE RECIEVING MESSAGE}" << std::endl;
				}
				**/

				result = server.send(message, bufferSize, ports[index]);

				/**
				if (result == CustomSocket::Result::Success)
				{
					std::cout << "[SERVICE INFO]: " << "{ SENT MESSAGE = " << message;
					std::cout << " } { NUMBER OF BYTES = " << static_cast<int>(bufferSize);
					std::cout << " }" << std::endl;
				}
				else
				{
					std::cout << "[CLIENT]: " << "{ERROR WHILE SENDING MESSAGE}" << std::endl;
				}
				**/

				server.disconnect(ports[index]);
			}

			//operationCounter++;
			delete[] ports;
		}
	}

	server.stop();

	system("pause");
	return 0;
}