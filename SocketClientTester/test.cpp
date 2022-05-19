#include "pch.h"

#include <SocketClient/SocketClient.h>
#include <thread>

TEST(SocketClientTestCase, ConstructorTest) 
{
	//NEGATIVE PART

	const CustomSocket::IPEndpoint validIPConfig("127.0.0.1", 4790);

	try
	{
		SocketClient client(nullptr, CustomSocket::IPVersion::Unknown);
		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	try
	{
		SocketClient client(&validIPConfig,
							CustomSocket::IPVersion::Unknown);
		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	try
	{
		SocketClient client(&validIPConfig);
		SocketClient failing_client(&validIPConfig);

		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) { }

	try
	{
		SocketClient client(&CustomSocket::IPEndpoint("327.0.0.1", 4790));

		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	//POSITIVE PART

	EXPECT_NO_THROW(SocketClient());
	EXPECT_NO_THROW(SocketClient(validIPConfig));
}

TEST(SocketClientTestCase, ConnectTest)
{
	const CustomSocket::IPEndpoint serverIPConfig("127.0.0.1", 4790);

	//NEGATIVE PART

	SocketClient client;

	EXPECT_EQ(client.connect(serverIPConfig),
			  CustomSocket::Result::Fail);

	//POSITIVE PART

	CustomSocket::Socket server;

	EXPECT_EQ(server.create(), CustomSocket::Result::Success);
	EXPECT_EQ(server.Listen(serverIPConfig),
						  CustomSocket::Result::Success);
	
	CustomSocket::Socket newConnection;
	std::thread listeningThread(&CustomSocket::Socket::Accept,
								server,
		std::ref(newConnection),
								nullptr);

	EXPECT_EQ(client.connect(serverIPConfig),
							 CustomSocket::Result::Success);

	listeningThread.join();

}

TEST(SocketClientTestCase, DisconnectTest)
{
	const CustomSocket::IPEndpoint serverIPConfig("127.0.0.1", 4798);

	SocketClient client;
	
	CustomSocket::Socket server;

	EXPECT_EQ(server.create(), CustomSocket::Result::Success);
	EXPECT_EQ(server.Listen(serverIPConfig),
		CustomSocket::Result::Success);

	CustomSocket::Socket newConnection;
	std::thread listeningThread(&CustomSocket::Socket::Accept,
		server,
		std::ref(newConnection),
		nullptr);

	EXPECT_EQ(client.connect(serverIPConfig),
		CustomSocket::Result::Success);

	listeningThread.join();

	EXPECT_EQ(client.disconnect(), CustomSocket::Result::Success);
}

TEST(SocketClientTestCase, RecieveTest)
{
	const CustomSocket::IPEndpoint serverIPConfig("127.0.0.1", 4794);

	//NEGATIVE TEST

	SocketClient client;
	EXPECT_EQ(client.recieve(nullptr, 0), CustomSocket::Result::Fail);

	const uint16_t bufferSize = 256;
	char buffer[bufferSize] = { };
	EXPECT_EQ(client.recieve(buffer, bufferSize), CustomSocket::Result::Fail);

	//ADD TEST WITH VALID CONNECTION BUT TRY TO RECIEVE NONVALID PARAMS

	//POSITIVE TEST

	CustomSocket::Socket server;

	EXPECT_EQ(server.create(), CustomSocket::Result::Success);
	EXPECT_EQ(server.Listen(serverIPConfig),
		CustomSocket::Result::Success);

	CustomSocket::Socket newConnection;
	std::thread listeningThread(&CustomSocket::Socket::Accept,
		server,
		std::ref(newConnection),
		nullptr);

	EXPECT_EQ(client.connect(serverIPConfig),
		CustomSocket::Result::Success);

	listeningThread.join();

	const char sendBuffer[bufferSize] = "Hello world)))\0";
	int bytesSent = 0;

	listeningThread = std::thread(&CustomSocket::Socket::Send,
								  newConnection,
								  std::ref(sendBuffer),
								  static_cast<int>(bufferSize),
								  &bytesSent);

	EXPECT_EQ(client.recieve(nullptr, 0), CustomSocket::Result::Fail);
	EXPECT_EQ(client.recieve(buffer, bufferSize), CustomSocket::Result::Success);

	listeningThread.join();

	EXPECT_EQ(client.disconnect(), CustomSocket::Result::Success);
	EXPECT_EQ(memcmp(buffer, sendBuffer, bufferSize), 0);
	EXPECT_EQ(bytesSent, bufferSize);
}

TEST(SocketClientTestCase, SendTest)
{
	const CustomSocket::IPEndpoint serverIPConfig("127.0.0.1", 4796);

	//NEGATIVE TEST

	SocketClient client;
	EXPECT_EQ(client.send(nullptr, 0), CustomSocket::Result::Fail);

	const uint16_t bufferSize = 32;
	const char buffer[bufferSize] = "Hello world)))";
	EXPECT_EQ(client.send(buffer, bufferSize), CustomSocket::Result::Fail);

	//POSITIVE TEST

	CustomSocket::Socket server;

	EXPECT_EQ(server.create(), CustomSocket::Result::Success);
	EXPECT_EQ(server.Listen(serverIPConfig),
		CustomSocket::Result::Success);

	CustomSocket::Socket newConnection;
	std::thread listeningThread(&CustomSocket::Socket::Accept,
		server,
		std::ref(newConnection),
		nullptr);

	EXPECT_EQ(client.connect(serverIPConfig),
		CustomSocket::Result::Success);

	listeningThread.join();

	char recieveBuffer[bufferSize] = { };
	int bytesRecieved = 0;

	listeningThread = std::thread(&CustomSocket::Socket::Recieve,
		newConnection,
		std::ref(recieveBuffer),
		static_cast<int>(bufferSize),
		&bytesRecieved);

	EXPECT_EQ(client.send(buffer, bufferSize), CustomSocket::Result::Success);
	EXPECT_EQ(client.send(nullptr, 0), CustomSocket::Result::Fail);

	listeningThread.join();

	EXPECT_EQ(client.disconnect(), CustomSocket::Result::Success);
	EXPECT_EQ(memcmp(buffer, recieveBuffer, bufferSize), 0);
	EXPECT_EQ(bufferSize, bytesRecieved);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

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