#include "server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

using namespace std;

Server::Server(const std::string &address, int port, int backlog)
    : serverAddress(address), serverPort(port), backlog(backlog), serverSd(-1)
{
    memset(&serverAddr, 0, sizeof(serverAddr));
}

Server::~Server()
{
    cleanUp();
}

bool Server::initialize()
{
    // Create the server socket
    serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSd < 0)
    {
        cerr << "Error: Failed to create socket: " << strerror(errno) << endl;
        return false;
    }

    // Allow socket reuse
    const int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // Configure the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(serverPort);

    // Bind the socket
    if (bind(serverSd, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cerr << "Error: Failed to bind socket: " << strerror(errno) << endl;
        cleanUp();
        return false;
    }

    // Start listening
    if (listen(serverSd, backlog) < 0)
    {
        cerr << "Error: Failed to listen on socket: " << strerror(errno) << endl;
        cleanUp();
        return false;
    }

    cout << "Server initialized and listening on " << serverAddress << ":" << serverPort << endl;
    return true;
}

void Server::start()
{
    while (true)
    {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        // Accept a new client connection
        int clientSd = accept(serverSd, (sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSd < 0)
        {
            cerr << "Error: Failed to accept client connection: " << strerror(errno) << endl;
            continue;
        }

        cout << "Accepted new connection from client." << endl;

        // Create a new thread to handle the client
        pthread_t thread;
        ThreadData *data = new ThreadData{clientSd};

        if (pthread_create(&thread, NULL, handleClient, (void *)data) != 0)
        {
            cerr << "Error: Failed to create thread: " << strerror(errno) << endl;
            delete data;
            close(clientSd);
            continue;
        }

        pthread_detach(thread); // Detach the thread for automatic cleanup
    }
}

void *Server::handleClient(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int clientSd = data->clientSd;
    delete data; // Free memory after extracting client socket

    char buffer[BUF_SIZE];
    memset(buffer, 0, BUF_SIZE);

    fd_set readfds;
    struct timeval timeout;

    // Handle client communication
    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(clientSd, &readfds);

        timeout.tv_sec = 5; // Set a timeout of 5 seconds
        timeout.tv_usec = 0;

        int activity = select(clientSd + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0)
        {
            cerr << "Error: select failed: " << strerror(errno) << endl;
            close(clientSd);
            return nullptr;
        }

        if (activity == 0)
        {
            // Timeout occurred, no data received
            cout << "Timeout occurred, no data received." << endl;
            continue;
        }

        // Check if the client socket is ready for reading
        if (FD_ISSET(clientSd, &readfds))
        {
            memset(buffer, 0, BUF_SIZE);
            int bytesRead = read(clientSd, buffer, BUF_SIZE);
            if (bytesRead < 0)
            {
                cerr << "Error: Failed to read from client: " << strerror(errno) << endl;
                close(clientSd);
                return nullptr;
            }
            else if (bytesRead == 0)
            {
                // Client disconnected
                cout << "Client disconnected." << endl;
                close(clientSd);
                return nullptr;
            }

            cout << "Received: " << buffer << endl;

            if (strcmp(buffer, "quit") == 0)
            {
                cout << "Client sent 'quit'. Closing connection." << endl;
                close(clientSd);
                return nullptr;
            }
        }
    }

    close(clientSd);
    return nullptr;
}

void Server::cleanUp()
{
    if (serverSd >= 0)
    {
        close(serverSd);
        serverSd = -1;
    }
}
