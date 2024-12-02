#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client
{
public:
    Client(const std::string &serverAddress, int port);
    ~Client();

    bool connectToServer();
    void startCommunication();
    void stopCommunication();

private:
    std::string serverAddress;
    int port;
    int clientSd;

    static const unsigned int BUF_SIZE = 65535;

    struct ThreadData
    {
        int clientSd;
    };

    static void *sendClient(void *arg);
    static void *receiveClient(void *arg);

    void cleanUp();
};

#endif // CLIENT_H
