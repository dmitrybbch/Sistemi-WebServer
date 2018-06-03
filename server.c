/**
 * Struttura del programma:
 * Avvio: dichiara le variabili, porta ecc..
 *      Crea un socket per l'ascolto
 *      Chiama listen() per prepararsi a ricevere
 * 
 * while (1) {
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
#include "C-Thread-Pool-master/thpool.h"
#include <pthread.h>

#define PORT 10000      //porta del server
#define SERVER_PROTOCOL "HTTP/1.1"

#define EOL "\r\n"
#define EOL_SIZE 2

void init(){
    /*
    pthread_t thMaster; //istanzio il thread master
    pthread_create(&thMaster, NULL, &thMasterFunction, NULL); //gli argomenti li rivedremo
    pid_t pchild = fork();      //facciamo il fork dopo che il server è pronto a ricevere richieste
    if(pchild == 0){
        //codice del figlio
        int sock = socket(AF_INET, SOCK_STREAM, SERVER_PROTOCOL);
    }
    else{
        printf("Error in the server setup.\n");
        return(EXIT_FAILURE);
    }
    */
}

int main(int argc, char *argv[]){

    // Initialization
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen; // Size of address
    struct sockaddr_in serv_addr, cli_addr; // Addresses

    

    return (EXIT_SUCCESS);
}