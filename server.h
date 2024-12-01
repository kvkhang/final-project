#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <netinet/in.h> // sockaddr_in

class Server
{
public:
    Server(const std::string &address, int port, int backlog = 5);
    ~Server();

    bool initialize();
    void start();

private:
    std::string serverAddress;
    int serverPort;
    int backlog;  // Max pending connections
    int serverSd; // Server socket descriptor
    sockaddr_in serverAddr;

    static const unsigned int BUF_SIZE = 65535;

    struct ThreadData
    {
        int clientSd;
    };

    static void *handleClient(void *arg);

    void cleanUp();
};

#endif // SERVER_H
