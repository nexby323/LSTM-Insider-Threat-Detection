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
    //the actual socket of the OS to send to the python server
    SOCKET clientSocket;
    //the server address for the socket above to connect
    sockaddr_in serverAddress;
    //used for the TrasmitEvent method, if full (has WINDOWS_SIZE events then send the all the data and erase the data)
    std::vector<int> eventBuffer;
    
    // Aligns with the BPTT sequence length configured in the LSTM model
    const size_t WINDOW_SIZE = 10; 

public:
    TcpClient(const std::string& ipAddress, int port);
    ~TcpClient();
    
    // Appends an intercepted kernel event to the buffer and transmits when full
    void TransmitEvent(int eventId);
};