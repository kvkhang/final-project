#include "client.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <server_address> <server_port>" << endl;
        return -1;
    }

    string serverAddress = argv[1];
    int port = atoi(argv[2]);

    Client client(serverAddress, port);

    if (!client.connectToServer())
    {
        cerr << "Error: Failed to connect to server" << endl;
        return -1;
    }

    client.startCommunication();
    // client.stopCommunication();

    // cout << "Client terminated." << endl;
    return 0;
}
