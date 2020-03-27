#include<stdlib.h>
#include"bufferCircolare.h"

void init(prodcons* B, int bufferlen) {
    B->buffer = malloc(bufferlen * sizeof(int));
    B->bufferSize = bufferlen;

    pthread_mutex_init(&(B->M),    NULL);
    pthread_cond_init(&(B->PIENO), NULL);
    pthread_cond_init(&(B->VUOTO), NULL);

    B->count = 0;
    B->readPos = 0;
    B->writePos = 0;
}

int enqueue(prodcons *B, int sockClient){
    pthread_mutex_lock(&(B->M));
    int retval = 0;

    if((B->count) != B->bufferSize){
        B->buffer[B->writePos] = sockClient;
        B->count++;
        B->writePos=((B->writePos+1)%B->bufferSize);
    }else{
        retval++;
    }
    pthread_cond_signal(&(B->VUOTO));
    pthread_mutex_unlock(&(B->M));

    return retval;
}

int dequeue(prodcons *B){
    pthread_mutex_lock(&(B->M));
    while (B->count == 0){
        pthread_cond_wait(&(B->VUOTO), &(B->M));
    }

    int socket = B->buffer[B->readPos];

    B->count--;
    B->readPos++;

    if ( B->readPos >= B->bufferSize ){
        B->readPos = 0;
    }

    pthread_cond_signal(&(B->PIENO));
    pthread_mutex_unlock(&(B->M));

    return socket;
}
