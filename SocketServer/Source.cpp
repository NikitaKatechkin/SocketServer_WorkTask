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

	system("pause");
	return 0;
}