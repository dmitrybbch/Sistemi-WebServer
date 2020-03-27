#include<pthread.h>

typedef struct {
  int* buffer;
  int bufferSize;
  pthread_mutex_t M;
  int readPos, writePos;
  int count;
  pthread_cond_t PIENO;
  pthread_cond_t VUOTO;
} prodcons;

void init(prodcons *B, int bufferlen);
int enqueue(prodcons *B, int sockClient);
int dequeue(prodcons *B);