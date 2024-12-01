#include <iostream>      //
#include <sys/types.h>   // socket, bind
#include <sys/socket.h>  // socket, bind, listen, inet_ntoa
#include <netinet/in.h>  // htonl, htons, inet_ntoa
#include <arpa/inet.h>   // inet_ntoa
#include <netdb.h>       // gethostbyname
#include <unistd.h>      // read, write, close
#include <strings.h>     // bzero
#include <netinet/tcp.h> // SO_REUSEADDR
#include <sys/uio.h>     // writev
#include <sys/time.h>    // gettimeofday
#include <pthread.h>     // threads
#include <cstring>       // memset

using namespace std;

const unsigned int BUF_SIZE = 65535;

// Struct to hold thread data
struct thread_data
{
    int sd;
};

void *thread_server(void *arg)
{
    // get data from struct
    thread_data *data = (thread_data *)arg;
    int newSd = data->sd;
    delete data; // free allocated memory

    char databuf[BUF_SIZE];
    memset(databuf, 0, BUF_SIZE);
    int totalBytesRead = 0;
    int count = 0;

    // Reading data from the client
    while (true)
    {
        memset(databuf, 0, BUF_SIZE);
        int result = read(newSd, databuf, BUF_SIZE);
        if (result < 0)
        {
            cerr << "Error reading: " << strerror(errno) << endl;
            close(newSd); // close on error
            return NULL;
        }
        else if (result == 0)
        {
            cout << "Client disconnected" << endl;
            close(newSd); // clos e if client disconnects
            return NULL;
        }
        cout << databuf << endl;
        if (strcmp(databuf, "quit") == 0)
        {
            cout << "they quit, but implement how it works please!" << endl;
        }
    }

    close(newSd); // Close connection
    return NULL;
}

int main()
{
    // Create the socket
    int server_port = 12345;

    sockaddr_in acceptSockAddr;
    bzero((char *)&acceptSockAddr, sizeof(acceptSockAddr)); // zero out the data structure
    acceptSockAddr.sin_family = AF_INET;                    // using IP
    acceptSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);     // listen on any address this computer has
    acceptSockAddr.sin_port = htons(server_port);           // set the port to listen on

    int serverSd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket for IP using TCP

    const int on = 1;
    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int)); // this lets us reuse the socket without waiting for hte OS to recycle it

    // Bind the socket
    if (bind(serverSd, (sockaddr *)&acceptSockAddr, sizeof(acceptSockAddr)) < 0)
    {
        cerr << "Error binding socket: " << strerror(errno) << endl;
        return -1;
    }

    // Listen on the socket
    int n = 5;
    listen(serverSd, n); // listen on the socket and allow up to n connections to wait.

    // Accept the connection as a new socket
    sockaddr_in newSockAddr; // place to store parameters for the new connection
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    while (true)
    {
        // Accept a new connection
        int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
        if (newSd < 0)
        {
            cerr << "Error accepting connection: " << strerror(errno) << endl;
            continue;
        }

        // Create a thread for the new connection
        pthread_t thread;
        thread_data *data = new thread_data;
        data->sd = newSd;

        if (pthread_create(&thread, NULL, thread_server, (void *)data) != 0)
        {
            cerr << "Error creating thread: " << strerror(errno) << endl;
            delete data; // free memory in case of error
            close(newSd);
        }

        pthread_detach(thread); // Detach thread so it cleans up after itself
    }
    return 0;
}
