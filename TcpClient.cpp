#define WIN32_LEAN_AND_MEAN
#include <winsock2.h> //windows socket api 
#include <ws2tcpip.h> //for tcp and ip communication 
#include "TcpClient.hpp"
#include <iostream> // for standart I/O 

#pragma comment(lib, "ws2_32.lib")
//ws2_32.dll provides us all the network communication for windows 

TcpClient::TcpClient(const std::string& ipAddress, int port) {
    //https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    WSADATA wsaData; //get the version (first parameter), second parameter is for getting the details about the windows socket implementation 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[Network] WSAStartup initialization failed." << std::endl;
        return;
    }
    //https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket
    //create a socket for ipv4 tcp (sock_stream and IPPROTO_TCP)
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "[Network] Socket instantiation failed." << std::endl;
        WSACleanup();
        return;
    }
    //set the address for ipv4 family 
    serverAddress.sin_family = AF_INET;
    //htons - host to network short convert to network byte order the port 
    serverAddress.sin_port = htons(port);
    //convert the address from string format to network format (including byte order)
    inet_pton(AF_INET, ipAddress.c_str(), &serverAddress.sin_addr);
    //connect via this format https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-connect
    if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "[Network] Failed to establish connection with the inference server." << std::endl;
    } else {
        std::cout << "[Network] Successfully connected to Python LADOHD interface." << std::endl;
    }
}

TcpClient::~TcpClient() {
    //close the socket 
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }
    //terminates the use of ws2_32.dll
    WSACleanup();
}

void TcpClient::TransmitEvent(int eventId) {
    eventBuffer.push_back(eventId);

    if (eventBuffer.size() == WINDOW_SIZE) {
        //transmit the raw binary representation of the integer vector to the server 
        //https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send according to those parameters 
        int bytesSent = send(clientSocket, reinterpret_cast<const char*>(eventBuffer.data()), static_cast<int>(WINDOW_SIZE * sizeof(int)), 0);
        
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "[Network] Buffer transmission failed. Winsock Error: " << WSAGetLastError() << std::endl;
        }

        //remove the oldest event to advance the sliding window mechanism
        eventBuffer.erase(eventBuffer.begin());
    }
}