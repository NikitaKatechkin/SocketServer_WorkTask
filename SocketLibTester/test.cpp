#include "pch.h"

#include <SocketLib/IncludeMe.h>
#include <thread>

void newThreadConnection(int& code, CustomSocket::IPEndpoint ip_info, SOCKET& socketHandle)
{
	code = connect(socketHandle,
				   reinterpret_cast<sockaddr*>(&(ip_info.GetSockaddrIPv4())),
				   sizeof(sockaddr_in));
}

void newThreadCSConnection(CustomSocket::Socket& socket, CustomSocket::IPEndpoint ip_info)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	EXPECT_EQ(socket.Connect(ip_info), CustomSocket::Result::Success);
}

void newThreadCSSend(CustomSocket::Socket& socket)
{
	char buffer[256];
	strcpy_s(buffer, "Hello, world)\0");

	int bytesSend = 0;
	EXPECT_EQ(socket.Send(buffer, 256, bytesSend), CustomSocket::Result::Success);
	EXPECT_EQ(bytesSend, 256);
}

void newThreadCSSendAll(CustomSocket::Socket& socket)
{
	char buffer[256];
	strcpy_s(buffer, "Hello, world)\0");

	EXPECT_EQ(socket.SendAll(buffer, 256), CustomSocket::Result::Success);
}

TEST(SocketTestCase, ConstructorTest) 
{
	CustomSocket::Socket socket;

	EXPECT_EQ(socket.getHandle(), INVALID_SOCKET);
	EXPECT_EQ(socket.getIPVersion(), CustomSocket::IPVersion::IPv4);

	try 
	{
		socket = CustomSocket::Socket(INVALID_SOCKET, CustomSocket::IPVersion::Unknown);
		FAIL();
	}
	catch (const std::exception& e)
	{
		EXPECT_EQ(e.what(), std::string("Using unknown socket IP version."));
	}
}

TEST(SocketTestCase, CreateTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket;

	EXPECT_EQ(socket.create(), CustomSocket::Result::Success);
	EXPECT_EQ(socket.create(), CustomSocket::Result::Fail);

	CustomSocket::NetworkAPIInitializer::Shutdown();
}

TEST(SocketTestCase, CloseTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket;

	EXPECT_EQ(socket.close(), CustomSocket::Result::Fail);
	EXPECT_EQ(socket.create(), CustomSocket::Result::Success);

	EXPECT_EQ(socket.close(), CustomSocket::Result::Success);

	CustomSocket::NetworkAPIInitializer::Shutdown();
}

TEST(SocketTestCase, BindTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket;

	socket.create();
	
	try
	{
		socket.Bind(CustomSocket::IPEndpoint("256.256.256.256", 4970));
		FAIL();
	}
	catch (const std::exception& error)
	{
		EXPECT_EQ(error.what(),
				  std::string("Invalid data was provided. Can not create an IPEndpoint object;"));
	}

	EXPECT_EQ(socket.Bind(CustomSocket::IPEndpoint("127.0.0.1", 4970)), 
			  CustomSocket::Result::Success);
	EXPECT_EQ(socket.Bind(CustomSocket::IPEndpoint("192.168.13.14", 4970)),
			  CustomSocket::Result::Fail);

	socket.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();
}

TEST(SocketTestCase, ListenTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket;

	socket.create();

	try
	{
		socket.Listen(CustomSocket::IPEndpoint("256.256.256.256", 4970));
		FAIL();
	}
	catch (const std::exception& error)
	{
		EXPECT_EQ(error.what(),
			std::string("Invalid data was provided. Can not create an IPEndpoint object;"));
	}

	EXPECT_EQ(socket.Listen(CustomSocket::IPEndpoint("127.0.0.1", 4970)),
		CustomSocket::Result::Success);
	EXPECT_EQ(socket.Listen(CustomSocket::IPEndpoint("192.168.13.14", 4970), 4),
		CustomSocket::Result::Fail);

	socket.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();
}

TEST(SocketTestCase, AcceptTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket_test;

	socket_test.create();

	CustomSocket::IPEndpoint ip_info("127.0.0.1", 4790);
	socket_test.Listen(ip_info);

	SOCKET socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_NE(socketHandle, INVALID_SOCKET);
	
	int connectCode = 0;
	std::thread ConnectThread = std::thread(newThreadConnection, 
											std::ref(connectCode), 
											ip_info, 
											std::ref(socketHandle));

	CustomSocket::Socket newConnection;
	EXPECT_EQ(socket_test.Accept(newConnection), CustomSocket::Result::Success);

	EXPECT_EQ(connectCode, 0);
	ConnectThread.join();

	socket_test.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();

	EXPECT_EQ(socket_test.Accept(newConnection), CustomSocket::Result::Fail);
}

TEST(SocketTestCase, ConnectTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket_test;

	socket_test.create();

	CustomSocket::IPEndpoint ip_info("127.0.0.1", 4790);
	socket_test.Listen(ip_info);

	CustomSocket::Socket socket_connect;
	socket_connect.create();
	std::thread ConnectThread = std::thread(newThreadCSConnection,
											std::ref(socket_connect), 
											ip_info);

	CustomSocket::Socket newConnection;
	EXPECT_EQ(socket_test.Accept(newConnection), CustomSocket::Result::Success);

	ConnectThread.join();

	socket_test.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();

	EXPECT_EQ(socket_connect.Connect(ip_info), CustomSocket::Result::Fail);
}

TEST(SocketTestCase, SendTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket_test;

	socket_test.create();

	CustomSocket::IPEndpoint ip_info("127.0.0.1", 4790);
	socket_test.Listen(ip_info);

	CustomSocket::Socket socket_connect;
	socket_connect.create();
	std::thread ConnectThread = std::thread(newThreadCSConnection,
		std::ref(socket_connect),
		ip_info);

	CustomSocket::Socket newConnection;
	EXPECT_EQ(socket_test.Accept(newConnection), CustomSocket::Result::Success);

	ConnectThread.join();

	ConnectThread = std::thread(newThreadCSSend, std::ref(socket_connect));
	ConnectThread.join();

	socket_test.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();

	char buffer[256];
	strcpy_s(buffer, "Hello, world)\0");

	int bytesSend = 0;
	EXPECT_EQ(socket_connect.Send(buffer, 256, bytesSend), CustomSocket::Result::Fail);
}

TEST(SocketTestCase, RecieveTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket_test;

	socket_test.create();

	CustomSocket::IPEndpoint ip_info("127.0.0.1", 4790);
	socket_test.Listen(ip_info);

	CustomSocket::Socket socket_connect;
	socket_connect.create();
	std::thread ConnectThread = std::thread(newThreadCSConnection,
		std::ref(socket_connect),
		ip_info);

	CustomSocket::Socket newConnection;
	EXPECT_EQ(socket_test.Accept(newConnection), CustomSocket::Result::Success);

	ConnectThread.join();

	ConnectThread = std::thread(newThreadCSSend, std::ref(socket_connect));

	char buffer[256];
	int bytesRecieved = 0;
	EXPECT_EQ(newConnection.Recieve(buffer, 256, bytesRecieved), CustomSocket::Result::Success);
	EXPECT_EQ(bytesRecieved, 256);

	ConnectThread.join();

	socket_test.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();
	
	bytesRecieved = 0;
	EXPECT_EQ(newConnection.Recieve(buffer, 256, bytesRecieved), CustomSocket::Result::Fail);
}

TEST(SocketTestCase, SendAllTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket_test;

	socket_test.create();

	CustomSocket::IPEndpoint ip_info("127.0.0.1", 4790);
	socket_test.Listen(ip_info);

	CustomSocket::Socket socket_connect;
	socket_connect.create();
	std::thread ConnectThread = std::thread(newThreadCSConnection,
		std::ref(socket_connect),
		ip_info);

	CustomSocket::Socket newConnection;
	EXPECT_EQ(socket_test.Accept(newConnection), CustomSocket::Result::Success);

	ConnectThread.join();

	ConnectThread = std::thread(newThreadCSSendAll, std::ref(socket_connect));
	ConnectThread.join();

	socket_test.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();

	char buffer[256];
	strcpy_s(buffer, "Hello, world)\0");

	EXPECT_EQ(socket_connect.SendAll(buffer, 256), CustomSocket::Result::Fail);
}

TEST(SocketTestCase, RecieveAllTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);

	CustomSocket::Socket socket_test;

	socket_test.create();

	CustomSocket::IPEndpoint ip_info("127.0.0.1", 4790);
	socket_test.Listen(ip_info);

	CustomSocket::Socket socket_connect;
	socket_connect.create();
	std::thread ConnectThread = std::thread(newThreadCSConnection,
		std::ref(socket_connect),
		ip_info);

	CustomSocket::Socket newConnection;
	EXPECT_EQ(socket_test.Accept(newConnection), CustomSocket::Result::Success);

	ConnectThread.join();

	ConnectThread = std::thread(newThreadCSSend, std::ref(socket_connect));

	char buffer[256];
	EXPECT_EQ(newConnection.RecieveAll(buffer, 256), CustomSocket::Result::Success);

	ConnectThread.join();

	socket_test.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();

	EXPECT_EQ(newConnection.RecieveAll(buffer, 256), CustomSocket::Result::Fail);
}

TEST(SocketTestCase, GetHandleTest)
{
	CustomSocket::Socket socket;

	EXPECT_EQ(socket.getHandle(), INVALID_SOCKET);
}

TEST(SocketTestCase, GetIPVersionTest)
{
	CustomSocket::Socket socket;

	EXPECT_EQ(socket.getIPVersion(), CustomSocket::IPVersion::IPv4);
}

TEST(SocketTestCase, SetHandleTest)
{
	EXPECT_EQ(CustomSocket::NetworkAPIInitializer::Initialize(), true);
	
	CustomSocket::Socket socket;
	EXPECT_EQ(socket.getHandle(), INVALID_SOCKET);

	CustomSocket::Socket socket_2;
	socket_2.create();

	socket.setHandle(socket_2.getHandle());
	EXPECT_EQ(socket.getHandle(), socket_2.getHandle());

	socket_2.close();

	CustomSocket::NetworkAPIInitializer::Shutdown();
}

TEST(SocketTestCase, SetIPVersionTest)
{
	CustomSocket::Socket socket;
	EXPECT_EQ(socket.getIPVersion(), CustomSocket::IPVersion::IPv4);

	socket.setIPVersion(CustomSocket::IPVersion::Unknown);
	EXPECT_EQ(socket.getIPVersion(), CustomSocket::IPVersion::Unknown);
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}