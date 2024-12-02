#include "client.0.1.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>

using namespace std;

Client::Client(const std::string &address, int port)
    : serverAddress(address), serverPort(port), clientSd(-1) {}

Client::~Client()
{
    disconnect();
}

bool Client::isConnected()
{
    return clientSd >= 0;
}

bool Client::connectClient()
{
    if (isConnected())
    {
        cerr << "Already connected to the server." << endl;
        return false;
    }

    struct hostent *host = gethostbyname(serverAddress.c_str());
    if (!host)
    {
        cerr << "Error: Unable to resolve host " << serverAddress << endl;
        return false;
    }

    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(serverPort);

    clientSd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSd < 0)
    {
        cerr << "Error: Unable to create socket" << endl;
        return false;
    }

    int connectStatus = connect(clientSd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));
    if (connectStatus < 0)
    {
        cerr << "Error: Unable to connect to server" << endl;
        close(clientSd);
        clientSd = -1;
        return false;
    }

    cout << "Connected to server at " << serverAddress << ":" << serverPort << endl;
    return true;
}

bool Client::sendMessage(const std::string &message)
{
    if (!isConnected())
    {
        cerr << "Error: Not connected to any server." << endl;
        return false;
    }

    int writeResult = write(clientSd, message.c_str(), message.size());
    if (writeResult < 0)
    {
        cerr << "Error: Failed to send message" << endl;
        return false;
    }

    return true;
}

std::string Client::receiveMessage()
{
    if (!isConnected())
    {
        cerr << "Error: Not connected to any server." << endl;
        return "";
    }

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesRead = read(clientSd, buffer, sizeof(buffer) - 1);
    if (bytesRead < 0)
    {
        cerr << "Error: Failed to receive message" << endl;
        return "";
    }
    else if (bytesRead == 0)
    {
        cerr << "Server disconnected." << endl;
        disconnect();
        return "";
    }

    return std::string(buffer);
}

void Client::disconnect()
{
    if (isConnected())
    {
        close(clientSd);
        clientSd = -1;
        cout << "Disconnected from server." << endl;
    }
}
