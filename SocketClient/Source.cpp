#include <SocketLib/IncludeMe.h>
#include <SocketClient/SocketClient.h>
#include <iostream>
#include <thread>

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

			
			//if (socket.Bind(CustomSocket::IPEndpoint("127.0.0.1", 4791)) 
			//	== CustomSocket::Result::Fail)
			//{
			//	throw std::exception();
			//}

			if (socket.Connect(CustomSocket::IPEndpoint("127.0.0.1", 4790))
				== CustomSocket::Result::Success)
			{
				std::cout << "Succesfully connected to server." << std::endl;

				
				char buffer[256];
				strcpy_s(buffer, "Hello world from client)))\0");

				//int bytesSent = 0;
				CustomSocket::Result sendFlag = CustomSocket::Result::Fail;

				while (sendFlag == CustomSocket::Result::Fail)
				{
					std::cout << "[SERVICE INFO]: ";
					std::cout << "Attempting to send data to server..." << std::endl;
					//sendFlag = socket.Send(buffer, 256, bytesSent);
					sendFlag = socket.Send(buffer, 256);

					if (sendFlag != CustomSocket::Result::Success)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(500));
					}
					else
					{
						std::cout << "[SERVICE INFO]: ";
						std::cout << "Data were successfully sent." << std::endl;
					}
				}

				
				CustomSocket::Result result = CustomSocket::Result::Fail;
				while (result != CustomSocket::Result::Success)
				{
					//result = newConnection.Recieve(buffer, 256, bytesRecieved);
					result = socket.Recieve(buffer, 256);

					if (result == CustomSocket::Result::Success)
					{
						std::cout << "[SERVER]: " << buffer << std::endl;
						break;
					}
				}
			}
			else
			{
				std::cerr << "Failed to connect to server." << std::endl;
			}

			socket.close();
		}
		else
		{
			std::cout << "Failed to create a socket." << std::endl;
		}
	}

	CustomSocket::NetworkAPIInitializer::Shutdown();
	**/

	/**
	const CustomSocket::IPEndpoint bindIPendpoint("127.0.0.1", 4791);
	const CustomSocket::IPEndpoint serverIPendpoint("127.0.0.1", 4790);

	const uint16_t bufferSize = 256;
	const char message[bufferSize] = "Hello world from client\0";

	//SocketClient client(&bindIPendpoint);
	SocketClient client;

	client.connect(serverIPendpoint);

	client.send(message, bufferSize);

	char recieveBuffer[bufferSize];
	client.recieve(recieveBuffer, bufferSize);

	client.disconnect();

	system("pause");
	return 0;

	**/
}