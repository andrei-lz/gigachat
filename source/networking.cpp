#include "networking.h"

#include <iostream>
#include <chrono>
#include <future>

#include <imgui.h>
#include <string>


inline void debug(const char* x)
{
	std::cout << x << "\n";
}

inline void debug(int x)
{
	std::cout << x << "\n";
}

namespace Networking
{


	ProtocolV2::ProtocolV2()
	{
		WSAData wsadata;
	    auto code = WSAStartup(MAKEWORD(2, 2), &wsadata);
	    if (code != 0)
	        debug("WSA Startup Fail!");

	    mainSocket = connectToServer("eudvlg-kierans-etx", "3001");

	}

	SOCKET ProtocolV2::connectToServer(const char* server_ip, const char* port)
	{
	    auto code = getaddrinfo(server_ip, port, nullptr, &addressInfo);
	    if (code != 0) //Success returns zero
		{	    	
			debug("Error: Failed to getaddrinfo");
			debug(code);
		}
	    auto sock = socket(AF_INET, SOCK_STREAM, 0);
	    if (sock == INVALID_SOCKET)
	        debug("Could not find socket!");

	    code = connect(sock, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);
	    if(code != 0)
	    {
	    	debug("Could not connect to socket using addressInfo");
	    	debug(code);
	    }

	    return sock;
	}

	int ProtocolV2::login(const char* username, const char* password)
	{
		//Sending Version

	    char header = 0x02;
	    send(mainSocket, &header, sizeof(header), 0);

		//Sending Login

		char usernameLength = strlen(username);
		char passwordLength = strlen(password);

		std::string packet;
		packet.append(1, usernameLength);
		packet.append(username);		
		packet.append(1, passwordLength);
		packet.append(password);		

		printf("%s", packet.c_str());

	    send(mainSocket, packet.c_str(), packet.size(), 0);

	    char serverResponse;
	    int returnLength = recv(mainSocket, &serverResponse, 1, 0);
	    if (returnLength == 0)
        {
            debug("Server Closed\n");
            return -1;
        }
        else if (returnLength < 0)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
            	debug("Just WSAEWOULDBLOCK, nothing to worry about");
            }

            debug("This is really bad");
            return -1;
        }

	    return serverResponse;
	}

	void ProtocolV2::sendMessage(std::string& message)
	{
		int messageLength = message.size();
		message.insert(0, messageLength, 2);
		send(mainSocket, message.c_str(), messageLength, 0);
	}

	ProtocolV2::~ProtocolV2()
	{
		freeaddrinfo(addressInfo);
	}

	MessageExchange::MessageExchange()
	{
		// Set up socket

	    WSAData wsadata;
	    auto code = WSAStartup(MAKEWORD(2, 2), &wsadata);
	    if (code != 0)
	        debug("WSA Startup Fail!");

	    mainSocket = connectToServer("eudvlg-kierans-etx", "3001");

	    sendHandshake();
	}

	SOCKET MessageExchange::connectToServer(const char* server_ip, const char* port)
	{
	    auto code = getaddrinfo(server_ip, port, nullptr, &addressInfo);
	    if (code != 0) //Success returns zero
		{	    	
			debug("Error: Failed to getaddrinfo");
			debug(code);
		}
	    auto sock = socket(AF_INET, SOCK_STREAM, 0);
	    if (sock == INVALID_SOCKET)
	        debug("Could not find socket!");

	    code = connect(sock, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);
	    if(code != 0)
	    {
	    	debug("Could not connect to socket using addressInfo");
	    	debug(code);
	    }

	    return sock;
	}

	void MessageExchange::sendHandshake()
	{

	    char header = 1;
	    send(mainSocket, &header, sizeof(header), 0);

	    const char* username = "Andrei Tate\n";
	    send(mainSocket, username, strlen(username), 0);

	}

	void MessageExchange::sendMessage(std::string& message)
	{
		message.append("\n");
	    send(mainSocket, message.c_str(), message.size(), 0);

	    printf("Sent: ");
	    printf("%s", message.c_str());
	    printf("\n");
	}

	void MessageExchange::sendMessage(const char* message)
	{
	    send(mainSocket, message, strlen(message), 0);

	    printf("Sent: ");
	    for(int i = 0; i < strlen(message); i++)
	    	printf("%c (%d) ", message[i], message[i]);
	    printf("\n");
	}

	using namespace std::chrono;

	void MessageExchange::startReceivingMessages(std::string& messages)
	{
		messageReceivingThread = std::thread([this, &messages](){
	        while (receivingMessages)
	        {
	            // What to run when the socket has some data for you

	            auto retValue = recv(mainSocket, messageReceivingBuffer, sizeof(messageReceivingBuffer), 0); // This is a continuous stream of at most 512 bytes
	            if (retValue == 0)
	            {
	                debug("Server Closed\n");
	            }
	            else if (retValue < 0)
	            {
	                if (WSAGetLastError() == WSAEWOULDBLOCK)
	                    continue;

	                debug("This is really bad");
	            }
	            else // retValue > 0 it has some content!
	            {
	            	std::cout<< "Server returned some bytestream!" << "\n";
	                std::string tempBuf = "";
	                tempBuf.append(messageReceivingBuffer);

	                messages.append(tempBuf.substr(0, retValue));
	                memset(messageReceivingBuffer, 0, sizeof(messageReceivingBuffer));
	            }
	        };
	    });
	}

	MessageExchange::~MessageExchange()
	{
		//Receiving Thread end
		receivingMessages = false;
		messageReceivingThread.join();

		// Winsock
    	freeaddrinfo(addressInfo);
	}
}