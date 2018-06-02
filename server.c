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

void init(){
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
}

//Routine del thread master
void thMasterFunction(){
    
}

int main(int argc, char *argv[]){
    init();

    return (EXIT_SUCCESS);
}