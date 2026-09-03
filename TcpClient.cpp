#include "TcpClient.hpp"
#include <iostream>

// Instructs the linker to include the Windows Sockets library (ws2_32.dll)
//ws2_32.dll provides us all the network communication for windows 
#pragma comment(lib, "ws2_32.lib")

TcpClient::TcpClient(const std::string& ipAddress, int port) {
    //https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    WSADATA wsaData;
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

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &serverAddress.sin_addr);

    if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "[Network] Failed to establish connection with the inference server." << std::endl;
    } else {
        std::cout << "[Network] Successfully connected to Python LADOHD interface." << std::endl;
    }
}

TcpClient::~TcpClient() {
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }
    WSACleanup();
}

void TcpClient::TransmitEvent(int eventId) {
    eventBuffer.push_back(eventId);

    if (eventBuffer.size() == WINDOW_SIZE) {
        // Transmit the raw binary representation of the integer vector
        int bytesSent = send(clientSocket, reinterpret_cast<const char*>(eventBuffer.data()), WINDOW_SIZE * sizeof(int), 0);
        
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "[Network] Buffer transmission failed. Winsock Error: " << WSAGetLastError() << std::endl;
        }

        // Remove the oldest event to advance the sliding window mechanism
        eventBuffer.erase(eventBuffer.begin());
    }
}