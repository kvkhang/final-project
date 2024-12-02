#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client
{
private:
    int clientSd;              // Socket descriptor for the client
    std::string serverAddress; // Server address
    int serverPort;            // Server port

    bool isConnected(); // Checks if the client is connected

public:
    Client(const std::string &address, int port);
    ~Client();

    bool connectClient();                         // Establishes a connection to the server
    bool sendMessage(const std::string &message); // Sends a message to the server
    std::string receiveMessage();                 // Receives a message from the server
    void disconnect();                            // Closes the connection
};

#endif // CLIENT_H
