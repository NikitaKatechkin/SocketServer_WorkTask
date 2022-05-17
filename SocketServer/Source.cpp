//#include <SocketLib/IncludeMe.h>
#include <SocketServer/SocketServer.h>
#include <iostream>

int main()
{
	const CustomSocket::IPEndpoint IPSettings("127.0.0.1", 4790);
	const uint16_t bufferSize = 256;
	const char message[bufferSize] = "Hello world from server\0";

	SocketServer server(IPSettings);

	server.run();

	std::vector<uint16_t> ports = server.getClientsPortList();

	while (ports.size() < 3)
	{
		ports = server.getActualClientsPortList();

		//std::cout << "[SERVICE INFO]: Not enough connected clients." << std::endl;
	}
	
	for (size_t index = 0; index < ports.size(); index++)
	{
		
			for (size_t index = 0; index < ports.size(); index++)
			{
				char recieveBuffer[bufferSize];
				CustomSocket::Result result = server.recieve(recieveBuffer,
					bufferSize,
					ports[index]);

				result = server.send(message, bufferSize, ports[index]);

				server.disconnect(ports[index]);
			}
	}

	server.stop();

	system("pause");
	return 0;
}