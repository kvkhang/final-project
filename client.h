#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <thread>
#include <atomic>

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
    std::atomic<bool> isRunning; // To control threads

    std::thread senderThread;
    std::thread receiverThread;

    void sendMessages();
    void receiveMessages();
};

#endif // CLIENT_H
