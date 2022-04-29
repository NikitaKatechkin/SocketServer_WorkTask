#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

namespace CustomSocket
{
	class NetworkHandler
	{
	public:
		NetworkHandler() = default;
		~NetworkHandler() = default;

		static bool Initialize();
		static void Shutdown();
	private:
	};
}