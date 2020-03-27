#include<pthread.h>
#define MAX_REQ_LENGTH 8096

typedef struct {
  //MinHeap
  int heap;

  // Your data
  int socket;
  char nomeFile[100];
  
} sjf_nodeData;

typedef struct {
  pthread_mutex_t M;
  pthread_cond_t PIENO;
  pthread_cond_t VUOTO;

  // Min Heap
  int heapSize;

  sjf_nodeData *data;

} sjf_prodcons;



void sjf_init(sjf_prodcons *B, int bufferlen);
int sjf_enqueue(sjf_prodcons *B, int size, int socket, char *name);
sjf_nodeData sjf_dequeue(sjf_prodcons *B);

//void sjf_print(sjf_prodcons *B);