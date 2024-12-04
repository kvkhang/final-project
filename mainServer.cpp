#include "server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib> // For std::stoi
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

struct Game
{
    string name;
    string player1 = "";
    int player1_Sd = 0;
    string player2 = "";
    int player2_Sd = 0;
};

void handleMessage(int clientSd, const string &message);
vector<string> tokenizer(string message);
void registerPlayer(int clientSd, vector<string> &message);
void listGames(int clientSd);
void createGame(int clientSd, vector<string> &message);
void joinGame(int clientSd, vector<string> &message);
void exitGame(int clientSd, vector<string> &message);
void gameStart(int clientSd, vector<string> &message);
void unregisterPlayer(int clientSd, vector<string> &message);
void game();

vector<string> players;
vector<Game> openGames;
vector<Game> closedGames;

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
    vector<string> tokens = tokenizer(message);
    if (tokens[0] == "register")
    {
        registerPlayer(clientSd, tokens);
    }
    else if (tokens[0] == "list")
    {
        listGames(clientSd);
    }
    else if (tokens[0] == "create")
    {
        createGame(clientSd, tokens);
    }
    else if (tokens[0] == "join")
    {
        joinGame(clientSd, tokens);
    }
    else if (tokens[0] == "exit")
    {
    }
    else if (tokens[0] == "gamestart")
    {
        gameStart(clientSd, tokens);
    }
    else if (tokens[0] == "game")
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
    cout << "Tokens: ";
    for (const auto &t : tokens)
    {
        cout << t << " ";
    }
    cout << endl;

    return tokens;
}

void registerPlayer(int clientSd, vector<string> &message)
{
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

void listGames(int clientSd)
{
    string message = "Open:";
    for (Game x : openGames)
    {
        message += " (" + x.name + ")";
    }
    message += "\nClosed:";
    for (Game x : closedGames)
    {
        message += " (" + x.name + ")";
    }
    write(clientSd, message.c_str(), message.size());
}
void createGame(int clientSd, vector<string> &message)
{
    if (message.size() > 3)
    {
        char response = 'S'; // Failure
        write(clientSd, &response, sizeof(response));
        return;
    }
    string user = message[1];
    string gameName = message[2];
    vector<Game>::iterator it;
    it = find_if(openGames.begin(), openGames.end(), [&](const Game &game)
                 { return gameName == game.name; });
    if (it == openGames.end()) // Game not created yet
    {
        Game newGame;
        newGame.name = gameName;
        newGame.player1 = user;
        newGame.player1_Sd = clientSd;
        openGames.push_back(newGame);
        char response = 'T'; // Success
        write(clientSd, &response, sizeof(response));
    }
    else
    {
        char response = 'F'; // Failure
        write(clientSd, &response, sizeof(response));
    }
}
void joinGame(int clientSd, vector<string> &message)
{
    if (message.size() > 3)
    {
        char response = 'S'; // Failure
        write(clientSd, &response, sizeof(response));
        return;
    }
    string user = message[1];
    string gameName = message[2];
    auto it = find_if(openGames.begin(), openGames.end(), [&](const Game &game)
                      { return gameName == game.name; });
    if (it != openGames.end()) // Game found
    {
        Game &currentGame = *it;
        currentGame.player2 = user;
        currentGame.player2_Sd = clientSd;
        closedGames.push_back(currentGame);
        openGames.erase(it);
        char response = 'T'; // Success
        write(clientSd, &response, sizeof(response));
    }
    else
    {
        char response = 'F'; // Failure
        write(clientSd, &response, sizeof(response));
    }
}
void exitGame(int clientSd, vector<string> &message)
{
}
void unregisterPlayer(int clientSd, vector<string> &message)
{
}
void gameStart(int clientSd, vector<string> &message)
{
    string user = message[1];
    string gameName = message[2];
    int player = stoi(message[3]);
    vector<Game>::iterator it;
    if (player == 1)
    {
        it = find_if(openGames.begin(), openGames.end(), [&](const Game &game)
                     { return gameName == game.name; });
    }
    if (player == 2)
    {
        it = find_if(closedGames.begin(), closedGames.end(), [&](const Game &game)
                     { return gameName == game.name; });
    }
    Game &currentGame = *it;
    // cout << currentGame.name << endl;
    // cout << currentGame.player1 << endl;
    // cout << currentGame.player1_Sd << endl;
    // cout << currentGame.player2 << endl;
    // cout << currentGame.player2_Sd << endl;
    int repeat = 120;
    while (currentGame.player1.empty() || currentGame.player1_Sd == 0 || currentGame.player2.empty() || currentGame.player2_Sd == 0)
    {
        cout << gameName << " looking for player2..." << endl;
        sleep(1);
        repeat--;
        if (repeat < 0)
            break;
    }
    if (repeat < 1)
    {
        return;
    }
    if (player == 1)
    {
        write(currentGame.player2_Sd, currentGame.player1.c_str(), currentGame.player1.size());
    }
    if (player == 2)
    {
        write(currentGame.player1_Sd, currentGame.player2.c_str(), currentGame.player2.size());
    }
}