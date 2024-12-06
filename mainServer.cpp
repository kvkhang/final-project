#include "server.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib> // For std::stoi
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

struct Game
{
    string name;
    string player1 = "";
    int player1_Sd = 0;
    string player2 = "";
    int player2_Sd = 0;
    int player1_score = 0;
    int player2_score = 0;
    unordered_map<int, int> pool;
    unordered_map<int, int> player1_hand;
    unordered_map<int, int> player2_hand;
    bool isOver()
    {
        int hi = max(player1_score, player2_score);
        int lo = min(player1_score, player2_score);
        if (hi >= 5 && (hi - lo) >= 2)
        {
            return true;
        }
        bool isEmpty1 = true;
        bool isEmpty2 = true;
        for (int i = 1; i <= 13; i++)
        {
            if (player1_hand[i] > 0)
            {
                isEmpty1 = false;
            }
            if (player2_hand[i] > 0)
            {
                isEmpty2 = false;
            }
        }

        return pool.empty() || isEmpty1 || isEmpty2;
    }
    bool isTie() const
    {
        return player1_score == player2_score;
    }
    string findWinner() const
    {
        if (player1_score > player2_score)
            return player1;
        return player2;
    }
};

void handleMessage(int clientSd, const string &message);
void serverWrite(int clientSd, const string &message);
vector<string> tokenizer(string message);
void registerPlayer(int clientSd, vector<string> &message);
void listGames(int clientSd);
void createGame(int clientSd, vector<string> &message);
void joinGame(int clientSd, vector<string> &message);
void exitGame(int clientSd);
void gameStart(int clientSd, vector<string> &message);
// void unregisterPlayer(int clientSd); // Removed unused function declaration
void game(int clientSd, vector<string> &message);
string playerHand(unordered_map<int, int> &hand, const string &playerName, const string &oppName, int playerScore, int oppScore);
string intToString(const int &handNum);
int guessToInt(const string &guess);

unordered_map<int, string> players;
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
    server.start(handleMessage, exitGame);

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
        exitGame(clientSd);
    }
    else if (tokens[0] == "gamestart")
    {
        gameStart(clientSd, tokens);
    }
    else if (tokens[0] == "game")
    {
        game(clientSd, tokens);
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
    bool nameNotFound = true;
    for (const auto &pair : players)
    {
        if (pair.second == message[1])
        {
            nameNotFound = false;
            break;
        }
    }
    auto it = players.find(clientSd);
    if (nameNotFound)
    { // Player not found
        players[clientSd] = message[1];
        serverWrite(clientSd, "T"); // Success
    }
    else
    {
        serverWrite(clientSd, "F"); // Failure
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
    serverWrite(clientSd, message);
}

void createGame(int clientSd, vector<string> &message)
{
    if (message.size() > 3)
    {
        serverWrite(clientSd, "S"); // Failure
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
        serverWrite(clientSd, "T"); // Success
    }
    else
    {
        serverWrite(clientSd, "F"); // Failure
    }
}

void joinGame(int clientSd, vector<string> &message)
{
    if (message.size() > 3)
    {
        // Failure
        serverWrite(clientSd, "S");
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
        // Success
        serverWrite(clientSd, "T");
    }
    else
    {
        char response = 'F'; // Failure
        write(clientSd, &response, sizeof(response));
    }
}

void exitGame(int clientSd)
{
    // ! something wrong
    // removing user from openGames vec
    auto it = find_if(openGames.begin(), openGames.end(), [&](const Game &game)
                      { return game.player1_Sd == clientSd || game.player2_Sd == clientSd; });

    if (it != openGames.end())
    {
        openGames.erase(it);
    }

    // removing user from closedGames vec
    it = find_if(closedGames.begin(), closedGames.end(), [&](const Game &game)
                 { return game.player1_Sd == clientSd || game.player2_Sd == clientSd; });

    if (it != closedGames.end())
    {
        closedGames.erase(it);
    }

    // unregistering player here
    players.erase(clientSd);
}

// no more unregister method
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
        exitGame(clientSd);
        return;
    }

    for (int i = 1; i <= 13; i++)
    {
        currentGame.pool[i] = 4;
        currentGame.player1_hand[i] = 0;
        currentGame.player2_hand[i] = 0;
    }

    for (int i = 0; i < 7; i++)
    {
        int random = 0;
        while (true)
        {
            random = rand() % 13 + 1;
            if (currentGame.pool[random] != 0)
            {
                currentGame.pool[random]--;
                currentGame.player1_hand[random]++;
                break;
            }
        }
        while (true)
        {
            random = rand() % 13 + 1;
            if (currentGame.pool[random] != 0)
            {
                currentGame.pool[random]--;
                currentGame.player2_hand[random]++;
                break;
            }
        }
    }

    if (player == 1)
    {
        serverWrite(currentGame.player2_Sd, currentGame.player1);
    }
    if (player == 2)
    {
        serverWrite(currentGame.player1_Sd, currentGame.player2);
    }

    // sends clients hand from same game
    if (player == 2)
    {
        serverWrite(currentGame.player1_Sd, playerHand(currentGame.player1_hand, currentGame.player1,
                                                       currentGame.player2, currentGame.player1_score, currentGame.player2_score));
        sleep(1);
        serverWrite(currentGame.player2_Sd, playerHand(currentGame.player2_hand, currentGame.player2,
                                                       currentGame.player1, currentGame.player2_score, currentGame.player1_score));
    }
}

void game(int clientSd, vector<string> &message)
{
    bool endGame = false;
    string guess = message[1];
    string gameName = message[2];
    int player = stoi(message[3]);
    vector<Game>::iterator it;
    if (player == 1)
    {
        it = find_if(closedGames.begin(), closedGames.end(), [&](const Game &game)
                     { return gameName == game.name; });
    }
    if (player == 2)
    {
        it = find_if(closedGames.begin(), closedGames.end(), [&](const Game &game)
                     { return gameName == game.name; });
    }
    Game &currentGame = *it;
    int guessNum = guessToInt(guess);

    // handling guessing
    if (player == 1)
    {
        cout << currentGame.player1 << " guessed: " << guess << endl;
        cout << currentGame.player2 << " has: " << currentGame.player2_hand[guessNum] << " " << guess << " cards" << endl;
        // player 1 guesses correctly
        if (currentGame.player2_hand[guessNum] > 0)
        {
            // updates player's hands
            currentGame.player2_hand[guessNum]--;
            currentGame.player1_hand[guessNum]++;
            serverWrite(currentGame.player1_Sd, "T");
            sleep(1);
            serverWrite(currentGame.player2_Sd, guess);
            sleep(1);
            serverWrite(currentGame.player2_Sd, "T");
        }
        // player 1 guesses incorrectly and draws a card
        else
        {
            int random = 0;
            while (true)
            {
                random = rand() % 13 + 1;
                if (currentGame.pool[random] != 0)
                {
                    currentGame.pool[random]--;
                    currentGame.player1_hand[random]++;
                    break;
                }
            }
            serverWrite(currentGame.player1_Sd, intToString(random));
            sleep(1);
            serverWrite(currentGame.player2_Sd, guess);
            sleep(1);
            serverWrite(currentGame.player2_Sd, "F");
        }
    }

    // player 2 is guessing
    if (player == 2)
    {
        cout << currentGame.player2 << " guessed: " << guess << endl;
        cout << currentGame.player1 << " has: " << currentGame.player1_hand[guessNum] << " " << guess << " cards" << endl;
        // player 2 guesses correctly
        if (currentGame.player1_hand[guessNum] > 0)
        {
            // updates player's hands
            currentGame.player1_hand[guessNum]--;
            currentGame.player2_hand[guessNum]++;
            serverWrite(currentGame.player2_Sd, "T");
            sleep(1);
            serverWrite(currentGame.player1_Sd, guess);
            sleep(1);
            serverWrite(currentGame.player1_Sd, "T");
        }
        // player 2 guesses incorrectly and draws a card
        else
        {
            int random = 0;
            while (true)
            {
                random = rand() % 13 + 1;
                if (currentGame.pool[random] != 0)
                {
                    currentGame.pool[random]--;
                    currentGame.player2_hand[random]++;
                    break;
                }
            }
            serverWrite(currentGame.player2_Sd, intToString(random));
            sleep(1);
            serverWrite(currentGame.player1_Sd, guess);
            sleep(1);
            serverWrite(currentGame.player1_Sd, "F");
        }
    }

    sleep(1);

    // checks for new pairs and updates scores
    for (int i = 1; i <= 13; i++)
    {
        if (currentGame.player1_hand[i] > 1)
        {
            currentGame.player1_score++;
            currentGame.player1_hand[i] -= 2;
        }
        if (currentGame.player2_hand[i] > 1)
        {
            currentGame.player2_score++;
            currentGame.player2_hand[i] -= 2;
        }
    }

    // send clients their hand
    serverWrite(currentGame.player1_Sd, playerHand(currentGame.player1_hand, currentGame.player1, currentGame.player2,
                                                   currentGame.player1_score, currentGame.player2_score));

    serverWrite(currentGame.player2_Sd, playerHand(currentGame.player2_hand, currentGame.player2, currentGame.player1,
                                                   currentGame.player2_score, currentGame.player1_score));

    sleep(1);

    if (currentGame.isOver())
    {
        if (currentGame.isTie())
        {
            serverWrite(currentGame.player1_Sd, "\nGame Over!\nTie!");
            serverWrite(currentGame.player2_Sd, "\nGame Over!\nTie!");
        }
        else
        {
            string winner = currentGame.findWinner();
            serverWrite(currentGame.player1_Sd, "\nGame Over!\n" + winner + " has won!");
            serverWrite(currentGame.player2_Sd, "\nGame Over!\n" + winner + " has won!");
        }
    }
    else
    {
        serverWrite(currentGame.player1_Sd, "F");
        serverWrite(currentGame.player2_Sd, "F");
    }
}

string playerHand(unordered_map<int, int> &hand, const string &playerName, const string &oppName, int playerScore, int oppScore)
{
    string result;
    for (int i = 1; i <= 13; i++)
    {
        if (hand[i] > 0)
        {
            for (int j = 0; j < hand[i]; j++)
            {
                result += intToString(i) + " ";
            }
        }
    }
    result += "\n" + playerName + ": " + to_string(playerScore) + "\n" + oppName + ": " + to_string(oppScore);
    return result;
}

string intToString(const int &handNum)
{
    if (handNum == 13)
    {
        return "king";
    }
    if (handNum == 12)
    {
        return "queen";
    }
    if (handNum == 11)
    {
        return "jack";
    }
    if (handNum == 1)
    {
        return "ace";
    }
    return to_string(handNum);
}

int guessToInt(const string &guess)
{
    if (guess == "king")
    {
        return 13;
    }

    if (guess == "queen")
    {
        return 12;
    }

    if (guess == "jack")
    {
        return 11;
    }

    if (guess == "ace")
    {
        return 1;
    }

    try
    {
        return stoi(guess);
    }
    catch (const invalid_argument &e)
    {
        return -1;
    }
}

void serverWrite(int clientSd, const string &message)
{
    write(clientSd, message.c_str(), message.size());
}
