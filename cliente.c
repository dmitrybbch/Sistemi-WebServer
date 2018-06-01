#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_REQUEST_LENGTH 8192
#define PORT 80
#define FILE_TO_REQUEST "index.html"

int main(int argc, char** argv) {

    char arg[MAX_REQUEST_LENGTH/2];
    char firstHalf[MAX_REQUEST_LENGTH/2];
    char secondHalf[MAX_REQUEST_LENGTH/2];
    char request[MAX_REQUEST_LENGTH];

    struct hostent *server;
    struct sockaddr_in serveraddr; // Struct that represents an internet address

    strcpy(arg, argv[1]);

    // Dividing the first argument in host and file
    int i;
    for (i = 0; i < strlen(arg); i++){
        if (arg[i] == '/'){
                strncpy(firstHalf, arg, i);
                firstHalf[i] = '\0';
                break;
        }     
    }
    for (i; i < strlen(arg); i++){
        strcat(secondHalf, &arg[i]);
        break;
    }
     
    // Logging the result
    printf("\nFirst Half: %s", firstHalf);
    printf("\nSecond Half: %s", secondHalf);

    // Opening the socket
    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket < 0)  printf("\nError opening socket!");
    else                printf("\nSuccessfully opened socket: %d", tcpSocket);
    
    // Getting the server and printing relevant info
    server = gethostbyname(firstHalf);
    if (server == NULL)
        printf("gethostbyname() failed\n");
    else {
        printf("\n%s = ", server->h_name);
        unsigned int j = 0;
        while (server -> h_addr_list[j] != NULL){
            printf("%s", inet_ntoa(*(struct in_addr*)(server -> h_addr_list[j])));
            j++;
        }
    }
    printf("\n");

    //Zeroing the server address for some reason
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; // Sets the type of address to IPV4 (?)

    //Copy the desired server's first address to the internet address to use
    bcopy((char *)server->h_addr_list[0], (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    
    // Address conversion: Host to Network Short
    serveraddr.sin_port = htons(PORT);
    
    // Actually connecting to the server
    if (connect(tcpSocket, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
        printf("\nError Connecting!!");
    else
        printf("\nSuccessfully Connected.");

    // Zeroing the request variable 
    bzero(request, MAX_REQUEST_LENGTH);
    
    // Putting the formatted text in the request variable
    sprintf(request, "GET %s HTTP/1.1\r\nHost:%s\r\n\r\n", secondHalf, firstHalf);
    
    // Logging the request text before sending it to the server
    printf("\n%s", request);
    
    
    if (send(tcpSocket, request, strlen(request), 0) < 0)
        printf("Error with send()");
    else
        printf("Successfully sent html fetch request");
     
    bzero(request, 1000);
     
    recv(tcpSocket, request, 999, 0);
    printf("\n%s", request);
    printf("\nhello");
     
    close(tcpSocket);
     
    return (EXIT_SUCCESS);
}