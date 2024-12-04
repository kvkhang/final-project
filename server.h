#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <netinet/in.h> // sockaddr_in
#include <functional>

class Server
{
public:
    // msg handler --> msgs beyond disconnect
    using MessageHandler = std::function<void(int clientSd, const std::string &)>;
    // delete handler --> basicially user creating deconstructor for server/client
    using DeleteHandler = std::function<void(int clientSd)>;

    // Constructor, initializing with address, port, and backlog size (default is 5)
    Server(const std::string &address, int port, int backlog = 5);
    ~Server();

    // Initializes the server (creates socket, binds, listens)
    bool initialize();

    // Starts the server, accepting clients and using a handler function for each client
    // void start(MessageHandler handler);
    void start(MessageHandler handler, DeleteHandler deleteHandler);

private:
    std::string serverAddress;
    int serverPort;
    int backlog;  // Max pending connections
    int serverSd; // Server socket descriptor
    sockaddr_in serverAddr;

    static const unsigned int BUF_SIZE = 65535;

    // Thread data structure for passing client socket to the handler
    struct ThreadData
    {
        int clientSd;
        MessageHandler handler;
        DeleteHandler deleteHandler;
    };

    // Handles a single client connection
    static void *handleClient(void *arg);

    // Cleans up resources (closes the server socket)
    void cleanUp();
};

#endif // SERVER_H
