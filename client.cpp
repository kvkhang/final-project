#include "client.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>

using namespace std;

Client::Client(const std::string &serverAddress, int port)
    : serverAddress(serverAddress), port(port), clientSd(-1) {}

Client::~Client()
{
    stopCommunication();
    if (clientSd >= 0)
    {
        close(clientSd);
    }
}

bool Client::connectToServer()
{
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
    sendSockAddr.sin_port = htons(port);

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
        return false;
    }

    cout << "Connected to server " << serverAddress << " on port " << port << endl;
    return true;
}

void Client::startCommunication()
{
    pthread_t sender, receiver;
    ThreadData *sendData = new ThreadData{clientSd};
    ThreadData *receivedData = new ThreadData{clientSd};

    if (pthread_create(&sender, NULL, sendClient, (void *)sendData) != 0)
    {
        cerr << "Error: Failed to create thread: " << strerror(errno) << endl;
        delete sendData;
        close(clientSd);
        return;
    }
    pthread_detach(sender);

    if (pthread_create(&receiver, NULL, receiveClient, (void *)receivedData) != 0)
    {
        cerr << "Error: Failed to create thread: " << strerror(errno) << endl;
        delete receivedData;
        close(clientSd);
        return;
    }
    pthread_detach(receiver);
}

void Client::stopCommunication()
{

    // Close the socket if it's open
    if (clientSd >= 0)
    {
        close(clientSd);
        clientSd = -1;
    }
}

void *Client::sendClient(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int clientSd = data->clientSd;
    delete data; // Free memory after extracting client socket

    char message[256];
    while (true)
    {
        memset(message, 0, sizeof(message));
        cout << "Enter message (type 'quit' to exit): ";
        cin.getline(message, sizeof(message));

        if (strcmp(message, "quit") == 0)
        {
            return nullptr;
        }

        int write_result = write(clientSd, message, strlen(message));
        if (write_result < 0)
        {
            cerr << "Error: Failed to send message" << endl;
            return nullptr;
        }
    }

    return nullptr;
}

void *Client::receiveClient(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int clientSd = data->clientSd;
    delete data; // Free memory after extracting client socket

    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(clientSd, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0)
        {
            cout << "\nServer: " << buffer << endl;
        }
        else if (bytesRead == 0)
        {
            cout << "\nServer disconnected." << endl;
            return nullptr;
        }
        else
        {
            cerr << "Error: Failed to receive message" << endl;
            return nullptr;
        }
    }
    return nullptr;
}
