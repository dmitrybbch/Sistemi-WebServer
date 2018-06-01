#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  //non ricordo cosa conteneva
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "C-Thread-Pool-master/thpool.h"
#include <pthread.h>

#define PORT 10000      //porta del server

void thMasterFunction(){
    //routine del thread master
}

int main(int argc, char *argv[]){
    //creo il thread master come prima cosa
    pthread_t thMaster;
    pthread_create(&thMaster, NULL, &thMasterFunction, NULL); //poi vediamo gli argomenti in caso
    pid_t pchild = fork();      //facciamo il fork dopo che il server è pronto a ricevere richieste
    if(pchild == 0){
        //codice del figlio
        int sock = socket(AF_INET, SOCK_STREAM, 0);
    }
    else{
        printf("Errore nell'avvio del server.\n");
        return(EXIT_FAILURE);
    }
    return 0;
}