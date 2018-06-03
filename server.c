/**
 * Struttura del programma:
 * Avvio: dichiara le variabili, porta ecc..
 *      Crea un socket per l'ascolto
 *      Chiama listen() per prepararsi a ricevere
 * 
 * 
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        pid = fork();
        if (pid < 0)
            error("ERROR on fork");
        if (pid == 0) {
            close(sockfd);
            connection(newsockfd);
            exit(0);
        } else  close(newsockfd);
    }
    close(sockfd);
    return 0;

    IMPLEMENTAZIONE DI QUESTO WHILE MA CON I THREAD INVECE CHE PROCESSI

 * Infine implementare la fork iniziale
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //non ricordo cosa conteneva
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "C-Thread-Pool-master/thpool.h"

#define PORT 10000      //porta del server
#define SERVER_PROTOCOL "HTTP/1.1"

#define EOL "\r\n"
#define EOL_SIZE 2

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void init(){
}

int main(int argc, char *argv[]){

    // Initialization
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen; // Size of address
    struct sockaddr_in serv_addr, cli_addr; // Addresses

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Opening socket
    if (sockfd < 0)
        error("Error opening socket!");
    bzero((char *) &serv_addr, sizeof(serv_addr)); // Zeroes serv_addr

    serv_addr.sin_family = AF_INET;  // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  // Give the socket's FD his local address
        error("Error on binding!");
    listen(sockfd, 5);  
    clilen = sizeof(cli_addr);

    


    return (EXIT_SUCCESS);
}