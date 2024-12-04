#include "client.0.1.h"
#include <iostream>
#include <string>
#include <cstdlib> // For std::stoi
#include <vector>

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <server_address> <server_port>" << endl;
        return -1;
    }

    // Parse server address and port from command-line arguments
    string serverAddress = argv[1];
    int serverPort;
    try
    {
        serverPort = std::stoi(argv[2]); // Convert the port to an integer
    }
    catch (const std::invalid_argument &e)
    {
        cerr << "Invalid port number." << endl;
        return -1;
    }

    Client client(serverAddress, serverPort);

    if (!client.connectClient())
    {
        cerr << "Failed to connect to the server. Exiting." << endl;
        return -1;
    }

    // Register Player
    string msgToServer;
    string username;
    string response;

    while (true)
    {
        cout << "Please input username: " << endl;
        getline(cin, username);
        msgToServer = "register " + username;
        client.sendMessage(msgToServer);
        response = client.receiveMessage();
        if (response == "F")
        {
            cout << username << " is already taken. Please try again." << endl;
        }
        else
        {
            cout << "User created!" << endl;
            break;
        }
    }

    string message;

    while (true)
    {
        cout << "<" << username << "> ";
        getline(cin, message);
        // Each command will start related protocol in server
        if (message == "list")
        {
            
        }
        else if (message == "create")
        {
        }
        else if (message == "join")
        {
        }
        else if (message == "exit")
        {
        }
        else if (message == "quit")
        {
        }
        else if (message == "help")
        {
        }
        else
        {
            cout << "\nInvalid Command. Type 'help' for commands." << endl;
        }
    }

    return 0;
}