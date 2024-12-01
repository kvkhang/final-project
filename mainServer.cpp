#include "server.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <server_address> <port>" << endl;
        return -1;
    }

    string serverAddress = argv[1];
    int port = atoi(argv[2]);

    Server server(serverAddress, port);

    if (!server.initialize())
    {
        cerr << "Error: Failed to initialize server." << endl;
        return -1;
    }

    server.start();

    return 0;
}
