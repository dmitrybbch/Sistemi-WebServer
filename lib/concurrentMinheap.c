#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<limits.h>

#include"concurrentMinheap.h"
#define MAX_REQ_LENGTH 8096

void sjf_init(sjf_prodcons* B, int heapMaxLength) {

    pthread_mutex_init(&(B->M),    NULL);
    pthread_cond_init(&(B->PIENO), NULL);
    pthread_cond_init(&(B->VUOTO), NULL);
    
    // Min Heap
    B->heapSize = 0;
    //B->data->heap = malloc(heapMaxLength * sizeof(int));
    //B->data->socket = malloc(heapMaxLength * sizeof(int));

    B->data = malloc(heapMaxLength * sizeof(sjf_nodeData));
    
    B->data[0].heap = -INT_MAX;
    B->data[0].socket = -INT_MAX;
    strcpy(B->data[0].nomeFile, "- INT MAX");
}

int sjf_enqueue(sjf_prodcons *B, int size, int socket, char *name){

    pthread_mutex_lock(&(B->M));
    int retval = 0;

    B->heapSize++;


    ///TODO togliere le assegnazioni di socket e name ovunque tranne che alla fine

    B->data[B->heapSize].heap = size; /*Insert in the last place*/

        // Assegni con la struct
    /*Adjust its position*/
    int now = B->heapSize;
    while (B->data[now / 2].heap > size) {
        B->data[now].heap = B->data[now / 2].heap;                     
        now /= 2;
    }

    B->data[now].heap = size;
    B->data[now].socket = socket;
    strcpy(B->data[now].nomeFile, name);

    //printf("SJF enqueued %s: %d with size %d.\n", name, socket, size);

    pthread_cond_signal(&(B->VUOTO));
    pthread_mutex_unlock(&(B->M));

    return retval;
}

sjf_nodeData sjf_dequeue(sjf_prodcons *B){
    
    pthread_mutex_lock(&(B->M));
    // Wait until there is something in the heap
    while (B->heapSize == 0){
        pthread_cond_wait(&(B->VUOTO), &(B->M));
    }

    /* heap[1] is the minimum element. So we remove heap[1]. Size of the heap is decreased.
     Now heap[1] has to be filled. We put the last element in its place and see if it fits.
     If it does not fit, take minimum element among both its children and replaces parent with it.
     Again See if the last element fits in that place.*/
    int lastElement, child, now;

    /*int minElement = B->data[1].heap;
    int minElementSocket = B->data[1].socket;*/

    lastElement = B->data[B->heapSize].heap;
    int lastElementSocket = B->data[B->heapSize].socket;
    B->heapSize--;

    /* now refers to the index at which we are now */
    for (now = 1; now * 2 <= B->heapSize; now = child) {
        /* child is the index of the element which is minimum among both the children */
        /* Indexes of children are i*2 and i*2 + 1*/
        child = now * 2;
        /*child!=heapSize beacuse heap[heapSize+1] does not exist, which means it has only one
         child */
        if (child != B->heapSize && B->data[child + 1].heap < B->data[child].heap) {
            child++;
        }
        /* To check if the last element fits ot not it suffices to check if the last element
         is less than the minimum element among both the children*/
        if (lastElement > B->data[child].heap) {
            B->data[now].heap = B->data[child].heap;
            B->data[now].socket = B->data[child].socket;
        } else /* It fits there */
        {
            break;
        }
    }
    
    B->data[now].heap = lastElement;
    B->data[now].socket = lastElementSocket;

    //printf("SJF extracted element: %d", minElementSocket);

    pthread_cond_signal(&(B->PIENO));
    pthread_mutex_unlock(&(B->M));
    
    return B->data[now];

}
/* 
void sjf_print(sjf_prodcons *B){
    printf("\nHeap: ");
    int i;
    for(i=0; i<=B->heapSize; i++)
        printf("Node %d: (%d, %d)", i, B->heap[i], B->socket[i]);
    printf("\n");
}
*/