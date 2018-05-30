#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  //non ricordo cosa conteneva
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "C-Thread-Pool-master/thpool.h"

#define PORT "10000"       //porta del server

void thMasterFunction(pthread_t *thread, pthread_attr_t *attr, void *(*start_routine)(void *)){

}

int main(int argc, char **argv){
    pid_t pchild = fork();      //dubito di dover fare il fork subito subito, ripensaci
    if(pchild == 0){
        //codice del figlio
        int masterSocket = socket(AF_INET, SOCK_STREAM, 0);
    }
    else{
        //codice del padre
    }
    exit(0);
}