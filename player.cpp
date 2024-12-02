#include <iostream>
#include <sys/types.h>   // socket, bind
#include <sys/socket.h>  // socket, bind, listen, inet_ntoa
#include <netinet/in.h>  // htonl, htons, inet_ntoa
#include <arpa/inet.h>   // inet_ntoa
#include <netdb.h>       // gethostbyname
#include <unistd.h>      // read, write, close
#include <strings.h>     // bzero
#include <netinet/tcp.h> // SO_REUSEADDR
#include <sys/uio.h>     // writev
#include <string.h>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    const int server_port = 12345;
    char send_string[256];
    char server_name[] = "127.0.0.1";
    // figure out the IP address
    struct hostent *host = gethostbyname(server_name);
    // set up the data structure
    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(server_port);
    // create the socket
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    // connect <-- this makes me a client!
    int connectStatus = connect(clientSd, (sockaddr *)&sendSockAddr,
                                sizeof(sendSockAddr));
    // check for error
    if (connectStatus < 0)
    {
        printf("Failed to connect");
        return -1;
    }
    // write the string
    while (true)
    {
        memset(send_string, 0, sizeof(send_string));
        printf("What should I send?\n");
        cin.getline(send_string, sizeof(send_string));
        int write_result = write(clientSd, send_string, strlen(send_string));
    }
    // quit
    close(clientSd);
    return 0;
}