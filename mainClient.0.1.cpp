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
    bool gameStart = false;
    while (true)
    {
        cout << "<" << username << "> ";
        getline(cin, message);
        // Each command will start related protocol in server
        if (message == "list")
        {
            client.sendMessage("list");
            response = client.receiveMessage();
            cout << response << endl;
        }
        else if (message == "create")
        {
            cout << "<Server> What would you like to name the game?\n"
                 << "<" << username << "> ";
            getline(cin, message);
            msgToServer = "create " + message;
            client.sendMessage(msgToServer);
            response = client.receiveMessage();
            if (response == "F")
            {
                cout << "Failed to create game. Try again." << endl;
            }
            else if (response == "S")
            {
                cout << "No spaces allowed. Try again." << endl;
            }
            else
            {
                gameStart = true;
            }
        }
        else if (message == "join")
        {
            cout << "<Server> Which game would you like to join?\n"
                 << "<" << username << "> ";
            getline(cin, message);
            msgToServer = "join " + message;
            client.sendMessage(msgToServer);
            response = client.receiveMessage();
            if (response == "F")
            {
                cout << "Failed to join game. Try again." << endl;
            }
            else if (response == "S")
            {
                cout << "No spaces allowed. Try again." << endl;
            }
            else
            {
                gameStart = true;
            }
        }
        else if (message == "quit")
        {
            cout << "<Server> Goodbye!" << endl;
            client.disconnect();
            break;
        }
        else if (message == "help")
        {
        }
        else
        {
            cout << "\nInvalid Command. Type 'help' for commands." << endl;
        }
        // Game logic goes here
        if (gameStart)
        {
            cout << "Waiting to connect..." << endl;
            string response = client.receiveMessage();
            cout << "Game Start! " << username << " vs. " << endl;
            while (true)
            {
            }
        }
    }

    return 0;
}