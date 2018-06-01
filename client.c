#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>  //non ricordo cosa conteneva
#include <sys/socket.h> // Robe per gestione di socket
#include <netinet/in.h> // NOn ricordo immagino serva per la connessione
#include <unistd.h>     //Non ricordo neanche quella
#include <pthread.h>
#include <netdb.h>

#define MAX_REQUEST_LENGTH 8192
#define SERVER DEQRF
#define PORT 10000

#define INDEX "index.html"
#define FILE_PATH "www/"


int main(int argc, char **argv){
    
    char fileName[255];
    char request[MAX_REQUEST_LENGTH];
    char mockRequest[] = "GET / HTTP/1.1\nhost: hostname";

    switch(argc){
        case 2:
            printf("Requesting %s, blyat...\n", argv[1]);
            strcpy(fileName, argv[1]);
        break;
        default:
            printf("Requesting index.html ...\n");
            strcpy(fileName, INDEX);
        break;
    }

    return 0;
}

void requestFile(char* fileName, int n){
    struct hostent *server;
    struct sockaddr_in serveraddr;

    // Stabiliamo un socket
    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket < 0)
        printf("\nError opening socket");
    else
        printf("\nSuccessfully opened socket");


    server = gethostbyname(SERVER);
     
    if (server == NULL)
    {
        printf("gethostbyname() failed\n");
    }
    else
    {
        printf("\n%s = ", server->h_name);
        unsigned int j = 0;
        while (server -> h_addr_list[j] != NULL)
        {
            printf("%s", inet_ntoa(*(struct in_addr*)(server -> h_addr_list[j])));
            j++;
        }
    }
     
    printf("\n");
}