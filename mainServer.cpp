#include "server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib> // For std::stoi
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

void handleMessage(int clientSd, const string &message);
vector<string> tokenizer(string message);
void registerPlayer(int clientSd, vector<string> &message);
void listGames(int clientSd, vector<string> &message);
void createGame(int clientSd, vector<string> &message);
void joinGame(int clientSd, vector<string> &message);
void exitGame(int cliendSd, vector<string> &message);
void unregisterPlayer(int clientSd, vector<string> &message);

vector<string> players;
vector<string> openGames;

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <server_address> <server_port>" << std::endl;
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

    // Create the server instance
    Server server(serverAddress, serverPort);
    if (!server.initialize())
    {
        return -1;
    }

    // Start the server, passing the message handler
    server.start(handleMessage);

    return 0;
}

void handleMessage(int clientSd, const string &message)
{
    cout << "Handling message from client: " << message << endl;
    vector<string> tokens = tokenizer(message);
    if (tokens[0] == "register")
    {
        registerPlayer(clientSd, tokens);
    }
    else if (tokens[0] == "create")
    {
    }
    else if (tokens[0] == "join")
        {
        }
        else if (tokens[0] == "exit")
        {
        }
        else if (tokens[0] == "quit")
        {
        }
}


vector<string> tokenizer(string message)
{
    // Tokenize the message using a stringstream
    vector<string> tokens;
    stringstream ss(message);
    string token;

    // Split by space (or you can specify other delimiters if needed)
    while (getline(ss, token, ' '))
    {
        tokens.push_back(token);
    }

    // Print tokens (for debugging purposes)
    cout << "Tokens:" << endl;
    for (const auto &t : tokens)
    {
        cout << t << endl;
    }

    return tokens;
}

void registerPlayer(int clientSd, vector<string> &message)
{
    cout << "Registering " << clientSd << endl;
    auto it = find(players.begin(), players.end(), message[1]);
    if (it == players.end()) // Player not found
    {
        players.push_back(message[1]);
        char response = 'T'; // Success
        write(clientSd, &response, sizeof(response));
    }
    else
    {
        char response = 'F'; // Failure
        write(clientSd, &response, sizeof(response));
    }
}

void listGames(int clientSd, vector<string> &message)
{

}
void createGame(int clientSd, vector<string> &message)
{
}
void joinGame(int clientSd, vector<string> &message)
{
}
void exitGame(int cliendSd, vector<string> &message)
{
}
void unregisterPlayer(int clientSd, vector<string> &message)
{
}