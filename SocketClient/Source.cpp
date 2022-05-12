#include <SocketLib/IncludeMe.h>
#include <iostream>

int main()
{
	if (CustomSocket::NetworkAPIInitializer::Initialize())
	{
		std::cout << "The winsock API was successfully initialized." << std::endl;

		CustomSocket::Socket socket;
		if (socket.create() == CustomSocket::Result::Success)
		{
			std::cout << "The socket was successfully created." << std::endl;

			if (socket.Connect(CustomSocket::IPEndpoint("127.0.0.1", 4790))
				== CustomSocket::Result::Success)
			{
				std::cout << "Succesfully connected to server." << std::endl;
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

	system("pause");
	return 0;
}