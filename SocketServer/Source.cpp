#include <SocketLib/IncludeMe.h>
#include <iostream>

int main()
{
	if (CustomSocket::NetworkHandler::Initialize())
	{
		std::cout << "The winsock API was successfully initialized." << std::endl;

		CustomSocket::Socket socket;
		if (socket.create() == CustomSocket::SocketResult::Success)
		{
			std::cout << "The socket was successfully created." << std::endl;

			socket.close();
		}
		else
		{
			std::cout << "Failed to create a socket." << std::endl;
		}
	}

	CustomSocket::NetworkHandler::Shutdown();

	system("pause");
	return 0;
}