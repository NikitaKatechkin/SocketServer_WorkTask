#include "pch.h"
#include <SocketServer/SocketServer.h>

TEST(SocketServerTestCase, ConstructorTest) 
{
	try
	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790), 
							CustomSocket::IPVersion::Unknown);
		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}


	try
	{
		SocketServer server(CustomSocket::IPEndpoint("327.0.0.1", 4970));

		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	//POSITIVE PART
	
	EXPECT_NO_THROW(SocketServer(CustomSocket::IPEndpoint("127.0.0.1", 4790)));
}

TEST(SocketServerTestCase, RunTest)
{
	try
	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
		CustomSocket::NetworkAPIInitializer::Shutdown();
		
		server.run();

		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	//POSITIVE PART

	SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
	EXPECT_NO_THROW(server.run());
}

TEST(SocketServerTestCase, StopTest)
{
	try
	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
		server.run();

		server.stop();
		server.stop();

		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	//POSITIVE PART

	SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
	EXPECT_NO_THROW(server.run());
	EXPECT_NO_THROW(server.stop());
}

TEST(SocketServerTestCase, DisconnectTest)
{
	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
		EXPECT_NO_THROW(server.run());

		EXPECT_EQ(server.disconnect(25), CustomSocket::Result::Fail);

		EXPECT_NO_THROW(server.stop());
	}

	//POSITIVE PART

	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
		EXPECT_NO_THROW(server.run());

		CustomSocket::Socket client;
		client.create();

		std::thread newThread(&SocketServer::waitForClientToConnect, std::ref(server));

		client.Bind(CustomSocket::IPEndpoint("127.0.0.1", 4791));
		client.Connect(CustomSocket::IPEndpoint("127.0.0.1", 4790));

		newThread.join();

		EXPECT_EQ(server.disconnect(4791), CustomSocket::Result::Success);
		client.close();
		EXPECT_NO_THROW(server.stop());
	}
}

TEST(SocketServerTestCase, RecieveTest)
{
	const int bufferSize = 256;

	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
		EXPECT_NO_THROW(server.run());

		char recieveBuffer[bufferSize] = {};

		EXPECT_EQ(server.recieve(nullptr, 256, 0), CustomSocket::Result::Fail);
		EXPECT_EQ(server.recieve(recieveBuffer, 0, 0), CustomSocket::Result::Fail);

		EXPECT_EQ(server.recieve(recieveBuffer, bufferSize, 0), CustomSocket::Result::Fail);

		EXPECT_NO_THROW(server.stop());
	}

	//POSITIVE PART
	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
		EXPECT_NO_THROW(server.run());

		CustomSocket::Socket client;
		client.create();

		std::thread newThread(&SocketServer::waitForClientToConnect, std::ref(server));

		client.Bind(CustomSocket::IPEndpoint("127.0.0.1", 4791));
		client.Connect(CustomSocket::IPEndpoint("127.0.0.1", 4790));

		newThread.join();

		const char buffer[bufferSize] = "Hello world)))\0";

		newThread = std::thread(&CustomSocket::Socket::Send, 
								std::ref(client),
								std::ref(buffer), 
								static_cast<int>(bufferSize), 
								nullptr);

		char recieveBuffer[bufferSize] = {};
		EXPECT_EQ(server.recieve(recieveBuffer, bufferSize, 4791), CustomSocket::Result::Success);

		newThread.join();

		EXPECT_EQ(server.disconnect(4791), CustomSocket::Result::Success);
		client.close();
		EXPECT_NO_THROW(server.stop());

		EXPECT_EQ(memcmp(buffer, recieveBuffer, bufferSize), 0);
	}
}

TEST(SocketServerTestCase, SendTest)
{
	const int bufferSize = 256;

	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
		EXPECT_NO_THROW(server.run());

		const char sendBuffer[bufferSize] = "Hello world)))\0";

		EXPECT_EQ(server.send(nullptr, 256, 0), CustomSocket::Result::Fail);
		EXPECT_EQ(server.send(sendBuffer, 0, 0), CustomSocket::Result::Fail);

		EXPECT_EQ(server.send(sendBuffer, bufferSize, 0), CustomSocket::Result::Fail);

		EXPECT_NO_THROW(server.stop());
	}

	//POSITIVE PART
	{
		SocketServer server(CustomSocket::IPEndpoint("127.0.0.1", 4790));
		EXPECT_NO_THROW(server.run());

		CustomSocket::Socket client;
		client.create();

		std::thread newThread(&SocketServer::waitForClientToConnect, std::ref(server));

		client.Bind(CustomSocket::IPEndpoint("127.0.0.1", 4791));
		client.Connect(CustomSocket::IPEndpoint("127.0.0.1", 4790));

		newThread.join();

		char buffer[bufferSize] = { };

		newThread = std::thread(&CustomSocket::Socket::Recieve,
			std::ref(client),
			std::ref(buffer),
			static_cast<int>(bufferSize),
			nullptr);

		const char sendBuffer[bufferSize] = "Hello world)))\0";
		EXPECT_EQ(server.send(sendBuffer, bufferSize, 4791), CustomSocket::Result::Success);

		newThread.join();

		EXPECT_EQ(server.disconnect(4791), CustomSocket::Result::Success);
		client.close();
		EXPECT_NO_THROW(server.stop());

		EXPECT_EQ(memcmp(buffer, sendBuffer, bufferSize), 0);
	}
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

	/**
	const CustomSocket::IPEndpoint IPSettings("127.0.0.1", 4790);
	const uint16_t bufferSize = 256;
	const char message[bufferSize] = "Hello world from server\0";

	SocketServer server(IPSettings);

	server.run();

	std::vector<uint16_t> ports = server.getClientsPortList();

	while (ports.size() < 3)
	{
		server.waitForClientToConnect();
		ports = server.getClientsPortList();
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
	**/
}