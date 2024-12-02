#include "server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

// Message handler function
void handleMessage(int clientSd, const std::string &message)
{
    std::cout << "Handling message from client: " << message << std::endl;
    // Send a response back to the client
    const char *response = "Message received!";
    write(clientSd, response, strlen(response));
}

int main()
{
    Server server("127.0.0.1", 12345);
    if (!server.initialize())
    {
        return -1;
    }

    // Start the server, passing the message handler
    server.start(handleMessage);

    return 0;
}
