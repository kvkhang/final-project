#include "client.0.1.h"
#include <iostream>
#include <string>
#include <cstdlib> // For std::stoi
#include <vector>
#include <unordered_set>

using namespace std;

static unordered_set<int> validGuesses{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

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

    return -1;
}

bool validateGuess(const string &guess)
{
    int num = 0;
    if (guess.length() > 1)
    {
        num = guessToInt(guess);
    }
    else
    {
        try
        {
            num = stoi(guess);
        }
        catch (const invalid_argument &e)
        {
            return false;
        }
    }
    return validGuesses.count(num);
}

void quitHelper(Client &client, const string &username)
{
    cout << "\nYou have chosen to exit the game!" << endl;
    client.disconnect();
}
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

    cout << "\nWelcome to the game of LaGuessy!" << endl;
    cout << "CSS 432 rendition of Go Fish" << endl;
    cout << "\nHow to Start:" << endl;
    cout << "  - enter a username" << endl;
    cout << "  - enter a command: list, create, join, quit" << endl;
    cout << "  - follow the prompts" << endl;
    cout << "  - enjoy the game!" << endl;
    cout << "\nType 'help' for more information.\n" << endl;

    while (true)
    {
        cout << "Please input username: ";
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
            quitHelper(client, username);
            break;
        }
        else if (message == "help") // Explains all functions
        {
            cout << "Available Commands: (ensure no spaces after commands)\n"
                 << "-------------------------\n"
                 << "list   - View all available games.\n"
                 << "create - Create a new game. You will be prompted to name the game (no spaces)\n"
                 << "join   - Join an existing game. You will be prompted to specify the game's name\n"
                 << "quit   - Disconnect and exit the program.\n"
                 << "help   - Display this help message.\n"
                 << "-------------------------" << endl;
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
            string input;
            string outcome;
            string hand;
            while (true)
            {
                hand = client.receiveMessage();
                cout << "Current hand: " + hand << endl;
                if (playerNumber == 1)
                {
                    // player guessing
                    cout << "Please input guess: \n"
                         << "<" << username << "> ";
                    getline(cin, input);
                    // continues to guess until a valid guess is given
                    while (!validateGuess(input))
                    {
                        cout << "Invalid Guess. 2, 3, 4, 5, 6, 7, 8, 9, 10, jack, queen, king, ace" << endl;
                        cout << "Please input guess: \n"
                             << "<" << username << "> ";
                        getline(cin, input);
                    }
                    // sends guess to server
                    msgToServer = "game " + input + " " + gameName + " " + to_string(playerNumber);
                    client.sendMessage(msgToServer);

                    // server sends outcome to guess
                    outcome = client.receiveMessage();
                    if (outcome == "T")
                    {
                        cout << "You have guessed correctly and received a(n) " << input << endl;
                    }
                    else
                    {
                        cout << "You have guessed incorrectly and received a(n) " << outcome << endl;
                    }

                    // waits for other player to guess
                    cout << "Waiting on " << oppUser << endl;
                    response = client.receiveMessage();
                    cout << oppUser << " has guessed: " << response << endl;
                    outcome = client.receiveMessage();

                    // other player guesses correctly and takes your card
                    if (outcome == "T")
                    {
                        cout << "Your " + response + " card has been taken." << endl;
                    }
                }
                else if (playerNumber == 2)
                {
                    // waiting for other player
                    cout << "Waiting on " << oppUser << endl;
                    response = client.receiveMessage();
                    cout << oppUser << " has guessed: " << response << endl;
                    outcome = client.receiveMessage();
                    // other player guesses correctly and takes your card
                    if (outcome == "T")
                    {
                        cout << "Your " + response + " card has been taken." << endl;
                    }
                    else
                    {
                        cout << oppUser + " has drawn a card." << endl;
                    }

                    // player guessing
                    cout << "Please input guess: \n"
                         << "<" << username << "> ";
                    getline(cin, input);
                    while (!validateGuess(input))
                    {
                        cout << "Invalid Guess. 2, 3, 4, 5, 6, 7, 8, 9, 10, jack, queen, king, ace" << endl;
                        cout << "Please input guess: \n"
                             << "<" << username << "> ";
                        getline(cin, input);
                    }

                    // sending guess to server
                    msgToServer = "game " + input + " " + gameName + " " + to_string(playerNumber);
                    client.sendMessage(msgToServer);

                    // server giving result
                    outcome = client.receiveMessage();
                    if (outcome == "T")
                    {
                        cout << "You have guessed correctly and received a(n) " << input << endl;
                    }
                }
            }
        }
    }

    return 0;
}
// TODO: fix guess validation (10 not recognized)
// TODO: fix 2nd player drawing card message
// TODO: fix first player hand becoming F
// TODO: fix scoring/pair finding
// TODO: fix random num generator