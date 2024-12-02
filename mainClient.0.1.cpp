#include "client.0.1.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    const string serverAddress = "127.0.0.1";
    const int serverPort = 12345;

    Client client(serverAddress, serverPort);

    if (!client.connectClient())
    {
        cerr << "Failed to connect to the server. Exiting." << endl;
        return -1;
    }

    string message;
    while (true)
    {
        cout << "Enter message to send (type 'quit' to exit): ";
        getline(cin, message);

        if (message == "quit")
        {
            client.disconnect();
            break;
        }

        if (!client.sendMessage(message))
        {
            cerr << "Failed to send message. Exiting." << endl;
            break;
        }

        string response = client.receiveMessage();
        if (response.empty())
        {
            cerr << "Failed to receive a response or server disconnected." << endl;
            break;
        }

        cout << "Server response: " << response << endl;
    }

    return 0;
}
