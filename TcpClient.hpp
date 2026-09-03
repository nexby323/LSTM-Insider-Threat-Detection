#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <string>

/*
 * TcpClient
 * Manages reliable TCP network communication utilizing the WinSock2 API.
 */
class TcpClient {
private:
    SOCKET clientSocket;
    sockaddr_in serverAddress;
    std::vector<int> eventBuffer;
    
    // Aligns with the BPTT sequence length configured in the LSTM model
    const size_t WINDOW_SIZE = 10; 

public:
    TcpClient(const std::string& ipAddress, int port);
    ~TcpClient();
    
    // Appends an intercepted kernel event to the buffer and transmits when full
    void TransmitEvent(int eventId);
};