#pragma once

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <string.h>

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <thread>

namespace Networking
{

	class ProtocolV2
	{
	public:
		ProtocolV2();
		~ProtocolV2();

		int login(const char*, const char*);

		void sendMessage(std::string&);

		void startReceivingMessages(std::string&);

	private:
		SOCKET mainSocket;	
	    addrinfo* addressInfo = nullptr;

		SOCKET connectToServer(const char*, const char*);
	};

	class MessageExchange
	{
	public:
		MessageExchange();
		~MessageExchange();

		void sendMessage(std::string&);
		void sendMessage(const char*);

		void startReceivingMessages(std::string&);

	private:
		SOCKET mainSocket;
	    addrinfo* addressInfo = nullptr;

		SOCKET connectToServer(const char*, const char*);
		void sendHandshake();

		bool receivingMessages = true;
		std::thread messageReceivingThread;
    	char messageReceivingBuffer[512]; //Buffer is needed for continuous stream received over a network
		
	};

}