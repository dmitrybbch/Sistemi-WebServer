#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<unistd.h>
#include"lib/thpool.h"
#include "lib/bufferCircolare.h"

#define MAX_REQ_LENGTH 8096
#define PORT 10005
#define BUFLEN 200

threadpool thpool;
int sock;

prodcons B;

void interrupt_handler(int sig);
int readRequest(int socket, char *buffer);
void URLcheck(char *url, int length);
void sendMsg(int socket, char *messaggio);
void sendFile(int socket, char *file);
void web();

int main(int argc, char *argv[]){
    struct sockaddr_in socketServer, socketClient;
    int sockCliente, pid, nThreadMax, i;

    //Handling Interrupts
    if (signal(SIGINT, interrupt_handler) == SIG_ERR){
        puts("SIGINT non inizializzabile!");
        exit(-1);
    }
    if (signal(SIGQUIT, interrupt_handler) == SIG_ERR){
        puts("SIGQUIT non inizializzabile!");
        exit(-1);
    }

    //Numero dei thread
    if(argc != 3 || (nThreadMax = atoi(argv[1])) < 1){
        printf("ERROR: invalid arguments. Usage: ./server nThreads port\n");
        exit(1);
    }

    //Creazione socket
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf( stderr, "Error: can't create socket\n");
        fprintf( stderr, "%d \n", errno);
        return -2;
    }

    //Tipo di indirizzo
    bzero(&socketServer , sizeof ( struct sockaddr_in ) );
    socketServer.sin_family = AF_INET;
    socketServer.sin_addr.s_addr =  INADDR_ANY;
    socketServer.sin_port = htons( atoi(argv[2]) );

    //Creazione socket
    if( bind(sock, (struct sockaddr *)&socketServer , sizeof(socketServer)) < 0){
        fprintf( stderr, "Error: failed to create socket\n");
        return 1;
    }

    //Socket messo in ascolto
    listen(sock , 128);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    init(&B, BUFLEN);
    
    //Creazione demon
    pid = fork();
    if (pid != 0) {
        printf("Pid = %d.\n", pid);	
        return 0;
    }
    
    //Creazione thread per thpool
    thpool = thpool_init(nThreadMax);
    for(i=0; i < nThreadMax; i++)
        thpool_add_work(thpool, (void*)web, NULL);

    socklen_t lengthClient = sizeof(socketClient);
    while(1){
        sockCliente = accept(sock, (struct sockaddr *)&socketClient, (socklen_t*)&lengthClient ) ;
        //printf("Connesione rilevata\n");
        if(sockCliente <= 0)
            printf("Errore accetazione connessione con un cliente non riuscita\n");
        else
            if(enqueue(&B, sockCliente)){
                printf("Warning: Buffer socket pieno\n");
                sendMsg(sockCliente, "HTTP/1.1 500 Internal Server Error\r\n");
                sendMsg(sockCliente, "<html><head><title>500 Buffer Full</head></title>");
                sendMsg(sockCliente, "<body><h1>500 Buffer is Full</h1></body></html>");
                close(sockCliente);
            }
    }
}

void interrupt_handler(int sig) {
    if ( sig == SIGINT ) {
        printf("\nSIGINT closing.\n");
        close(sock);
        exit(0);
        /*
        pthread_cond_destroy(&B.PIENO);
        pthread_cond_destroy(&B.VUOTO);
        pthread_mutex_destroy(&B.M);
        printf("Killing threadpool\n");
      	thpool_destroy(thpool);
          */
    }
}

// Mette la richiesta sul buffer
int readRequest(int socket, char *buffer){
    int i, length = read(socket, buffer, MAX_REQ_LENGTH);
    if( length < 0 ){
        return length;
    }else{
        buffer[MAX_REQ_LENGTH - 1] = '\0';

        for( i = 0; i < length; i++ ){
            if( buffer[i] == '\r' && buffer[i + 1] == '\n' ){
                buffer[i] = '\0';
                return length;
            }
        }
        return length;
    }
}

// Impedisce tentativi di accesso alla cartella padre(Sicurezza)*/
void URLcheck(char *url, int length){
    int i, j;
    for( i = 0; i < length; i++){
        if( url[i] == ' ' ){
            url[i] = 0;
            break;
        }
    }
    for( j = 0; j < i-1; j++){
        if( url[j] == '.' && url[j+1] == '.' ){
            printf("Thread: %ld tentativo accesso alla cartella padre\n", pthread_self());
            *url = '\0';
            break;
        }
    }
    if( !strncmp( &url[0], "/\0", 2 ) ){
        strcpy(url, "index.html\0" );
    }
}

void sendMsg(int socket, char *messaggio) {
    if (send(socket, messaggio, strlen(messaggio), 0) == -1)
        printf("Thread: %ld Errore in invio\n", pthread_self());
}

//Prova ad aprire il file e ad inviarlo, se non ci riesce manda error 404
void sendFile(int socket, char *file){
    //printf("Sending file %s...\n", file);
    FILE *fp = fopen(file, "r");

    if(!fp){//errore apertura file non riuscita
        printf("Thread: %ld errore 404\nerrno = %d\t%s\n", pthread_self(), errno, file);

        sendMsg(socket, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");
        sendMsg(socket, "<html><head><title>404 Not Found</title></head>");
        sendMsg(socket, "<body><center><h1>404 Not Found:<br> The page was not found!</h1></center></body></html>");
    }else{
        long byteInviati;
        char buffer[MAX_REQ_LENGTH], messaggio[64];

        sprintf(messaggio, "HTTP/1.1 200 OK\r\n\r\n");
        sendMsg(socket, messaggio);

        //Invio del file
        while( ( byteInviati = fread(buffer,sizeof(char), MAX_REQ_LENGTH, fp) ) > 0 )
            write(socket, buffer, byteInviati);
        fclose(fp);
    }
}

// Loop consumatore. Consuma richieste dal buffer
void web(){
    char buf[MAX_REQ_LENGTH];
    int socket, length;

    bzero(buf , sizeof ( char ) * MAX_REQ_LENGTH );

    while( 1 ){
        socket = dequeue(&B);

        length = readRequest(socket, buf);
        if ( length < 0 )
            printf("Thread: %ld Error: invalid message: %d %d %s\nsocket: %d\n%s\n", pthread_self(), length, errno, strerror(errno), socket, buf);
        else{
            char *url, html[100];

            // Controlla che sia una valida GET
            if( strncmp(buf,"GET ", 4) == 0 && ( url = strstr(buf, " HTTP/1.1") ) != NULL ){
                *url = '\0';
                url = buf + 4;
                length = length - 4;

                URLcheck( url, length );
                strcpy(html, "html/");
                strcat(html, url);

                // Controlla la validità dell'URL richiesto
                if( *url != '\0' )  sendFile(socket, html);
                else printf("Thread: %ld Error: invalid URL\n%s\n", pthread_self(), buf);

            }else{
                printf("Thread: %ld Error: invalid server request\n%s\n", pthread_self(), buf);
            }
        }
        close(socket);
    }
}
