#include "pch.h"

#include <SocketClient/SocketClient.h>
#include <thread>

TEST(SocketClientTestCase, ConstructorTest) 
{
	//NEGATIVE PART

	try
	{
		SocketClient client(nullptr, CustomSocket::IPVersion::Unknown);
		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	try
	{
		SocketClient client(&CustomSocket::IPEndpoint("127.0.0.1", 4970), 
							CustomSocket::IPVersion::Unknown);
		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	try
	{
		SocketClient client(&CustomSocket::IPEndpoint("127.0.0.1", 4970));
		SocketClient failing_client(&CustomSocket::IPEndpoint("127.0.0.1", 4970));

		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) { }

	try
	{
		SocketClient client(&CustomSocket::IPEndpoint("327.0.0.1", 4970));

		FAIL() << "Expect constructor to fail.";
	}
	catch (const std::exception&) {}

	//POSITIVE PART

	EXPECT_NO_THROW(SocketClient());
	EXPECT_NO_THROW(SocketClient(&CustomSocket::IPEndpoint("127.0.0.1", 4790)));
}

TEST(SocketClientTestCase, ConnectTest)
{
	//NEGATIVE PART

	SocketClient client;

	EXPECT_EQ(client.connect(CustomSocket::IPEndpoint("127.0.0.1", 4790)),
			  CustomSocket::Result::Fail);

	//POSITIVE PART

	CustomSocket::Socket server;

	EXPECT_EQ(server.create(), CustomSocket::Result::Success);
	EXPECT_EQ(server.Listen(CustomSocket::IPEndpoint("127.0.0.1", 4790)), 
						  CustomSocket::Result::Success);
	
	CustomSocket::Socket newConnection;
	std::thread listeningThread(&CustomSocket::Socket::Accept,
								server,
		std::ref(newConnection),
								nullptr);

	EXPECT_EQ(client.connect(CustomSocket::IPEndpoint("127.0.0.1", 4790)), 
							 CustomSocket::Result::Success);

	listeningThread.join();

}

TEST(SocketClientTestCase, DisconnectTest)
{
	SocketClient client;
	
	CustomSocket::Socket server;

	EXPECT_EQ(server.create(), CustomSocket::Result::Success);
	EXPECT_EQ(server.Listen(CustomSocket::IPEndpoint("127.0.0.1", 4798)),
		CustomSocket::Result::Success);

	CustomSocket::Socket newConnection;
	std::thread listeningThread(&CustomSocket::Socket::Accept,
		server,
		std::ref(newConnection),
		nullptr);

	EXPECT_EQ(client.connect(CustomSocket::IPEndpoint("127.0.0.1", 4798)),
		CustomSocket::Result::Success);

	listeningThread.join();

	EXPECT_EQ(client.disconnect(), CustomSocket::Result::Success);
}

TEST(SocketClientTestCase, RecieveTest)
{
	//NEGATIVE TEST

	SocketClient client;
	EXPECT_EQ(client.recieve(nullptr, 0), CustomSocket::Result::Fail);

	const uint16_t bufferSize = 256;
	char buffer[bufferSize];
	EXPECT_EQ(client.recieve(buffer, bufferSize), CustomSocket::Result::Fail);

	//POSITIVE TEST

	CustomSocket::Socket server;

	EXPECT_EQ(server.create(), CustomSocket::Result::Success);
	EXPECT_EQ(server.Listen(CustomSocket::IPEndpoint("127.0.0.1", 4794)),
		CustomSocket::Result::Success);

	CustomSocket::Socket newConnection;
	std::thread listeningThread(&CustomSocket::Socket::Accept,
		server,
		std::ref(newConnection),
		nullptr);

	EXPECT_EQ(client.connect(CustomSocket::IPEndpoint("127.0.0.1", 4794)),
		CustomSocket::Result::Success);

	listeningThread.join();

	const char sendBuffer[bufferSize] = "Hello world)))\0";
	listeningThread = std::thread(&CustomSocket::Socket::Send,
								  newConnection,
								  std::ref(sendBuffer),
								  static_cast<int>(bufferSize));

	EXPECT_EQ(client.recieve(buffer, bufferSize), CustomSocket::Result::Success);

	listeningThread.join();

	EXPECT_EQ(client.disconnect(), CustomSocket::Result::Success);
	EXPECT_EQ(memcmp(buffer, sendBuffer, bufferSize), 0);
}

TEST(SocketClientTestCase, SendTest)
{
	//NEGATIVE TEST

	SocketClient client;
	EXPECT_EQ(client.send(nullptr, 0), CustomSocket::Result::Fail);

	const uint16_t bufferSize = 256;
	const char buffer[bufferSize] = "Hello world)))";
	EXPECT_EQ(client.send(buffer, bufferSize), CustomSocket::Result::Fail);

	//POSITIVE TEST

	CustomSocket::Socket server;

	EXPECT_EQ(server.create(), CustomSocket::Result::Success);
	EXPECT_EQ(server.Listen(CustomSocket::IPEndpoint("127.0.0.1", 4796)),
		CustomSocket::Result::Success);

	CustomSocket::Socket newConnection;
	std::thread listeningThread(&CustomSocket::Socket::Accept,
		server,
		std::ref(newConnection),
		nullptr);

	EXPECT_EQ(client.connect(CustomSocket::IPEndpoint("127.0.0.1", 4796)),
		CustomSocket::Result::Success);

	listeningThread.join();

	char recieveBuffer[bufferSize];
	listeningThread = std::thread(&CustomSocket::Socket::Recieve,
		newConnection,
		std::ref(recieveBuffer),
		static_cast<int>(bufferSize));

	EXPECT_EQ(client.send(buffer, bufferSize), CustomSocket::Result::Success);

	listeningThread.join();

	EXPECT_EQ(client.disconnect(), CustomSocket::Result::Success);
	EXPECT_EQ(memcmp(buffer, recieveBuffer, bufferSize), 0);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}