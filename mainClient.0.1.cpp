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
    int playerNumber;

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
    string gameName;
    bool gameStart = false;
    while (true)
    {
        cout << "<" << username << "> ";
        getline(cin, message);
        // Each command will start related protocol in server
        if (message == "list") // Gives lists of current games
        {
            client.sendMessage("list");
            response = client.receiveMessage();
            cout << response << endl;
        }
        else if (message == "create") // Creates a game
        {
            cout << "<Server> What would you like to name the game?\n"
                 << "<" << username << "> ";
            getline(cin, message);
            msgToServer = "create " + username + " " + message;
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
                gameName = message;
                playerNumber = 1;
                gameStart = true;
            }
        }
        else if (message == "join") // Joins a known game
        {
            cout << "<Server> Which game would you like to join?\n"
                 << "<" << username << "> ";
            getline(cin, message);
            msgToServer = "join " + username + " " + message;
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
                gameName = message;
                playerNumber = 2;
                gameStart = true;
            }
        }
        else if (message == "quit") // Quits game entirely
        {
            cout << "<Server> Goodbye!" << endl;
            client.disconnect();
            break;
        }
        else if (message == "help") // Explains all functions
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
            msgToServer = "gamestart " + username + " " + gameName + " " + to_string(playerNumber);
            client.sendMessage(msgToServer);
            string oppUser = client.receiveMessage();
            cout << "Game Start! " << username << " vs. " << oppUser << endl;
            while (true)
            {
            }
        }
    }

    return 0;
}