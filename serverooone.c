#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //non ricordo cosa conteneva
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "C-Thread-Pool-master/thpool.h"

#define PORTA 10000
#define PROTOCOLLO "HTTP/1.1"
#define EOL "\r\n"
#define DIM_EOL 2
#define WEB_ROOT "/var/www/html"
#define N_THREAD 4


typedef struct {
    char* mediatype;
    char* est;
} estToMediatype;

// Tutti i mediatype che supportiamo
estToMediatype estensioni[] ={
    {"text/html", "html"},
    {"text/html", "htm"},
    {"text/plain", "txt"},
    {"application/pdf", "pdf"},
    {"image/gif", "gif"},
    {"image/jpg", "jpg"},
    {"image/jpeg", "jpeg"},
    {"image/png", "png"},
    {"image/ico", "ico"},
    {0, 0}
};

void errore(const char *msg){  //Funzione helper per gli errori
    perror(msg);
    exit(1);
}

int dimensioneFile(int fd){
    return 0;
}

int dim_fileDescriptor(int fd) {  // TODO : CAMBIAREEEE
    struct stat stat_struct;
    if (fstat(fd, &stat_struct) == -1)
        return (1);
    return (int) stat_struct.st_size;
}

// Sintesi dell'invio
void invia(int fd, char* msg) {
    int dim = strlen(msg);
    if (send(fd, msg, dim, 0) == -1)
        printf("ATTENZIONE: errore in invia().\n");
}
// Sintesi della ricezione
int ricevi(int fd, char* buffer){
    char* p = buffer;
    int eol_trovato = 0;
    char eol[] = EOL;

    // Ricevi un byte alla volta
    while (recv(fd, p, 1, 0) != 0) {
        if(*p == eol[eol_trovato]){
            eol_trovato++;
            if(eol_trovato == DIM_EOL){
                *(p + 1 - DIM_EOL) = '\0'; // End the string
                return (strlen(buffer)); // Return the bytes recieved
            }
        } else {
            eol_trovato = 0;
        }

        p++; // Increment the pointer to receive next byte
    }
    return (0);
}


// La funzione che praticamente fa il mestiere del webserver
int gestisci(int* fdII){
    int fd;
    fd = *fdII;
    char richiesta[500], risorsa[500], *ptr; /// TODO: CURSORE RICHIESTA
    int fd1, lung;

    if(ricevi(fd, richiesta) == 0)
        printf("Ricezione fallita.\n");
    printf("%s\n", richiesta);

    // Controlla che ci sia una richista valida
    ptr = strstr(richiesta, " HTTP/");

    if (ptr == NULL) {
        printf("NOT HTTP !\n");
    } else {
        *ptr = 0;
        ptr = NULL;

        if (strncmp(richiesta, "GET ", 4) == 0){
            ptr = richiesta + 4;
        }
        if(ptr == NULL) {
            printf("Richiesta Sconosciuta! \n");
        } else {
            if (ptr[strlen(ptr) - 1] == '/') {
                strcat(ptr, "index.html");
            }
            strcpy(risorsa, WEB_ROOT);
            strcat(risorsa, ptr);

            char* s = strchr(ptr, '.');
            int i;
            for(i = 0; estensioni[i].est != NULL; i++){
                if (strcmp(s + 1, estensioni[i].est) == 0) {
                    fd1 = open(risorsa, O_RDONLY, 0);
                    printf("Opening \"%s\"\n", risorsa);
                    if (fd1 == -1) {
                        printf("404 File not found Error\n");
                        invia(fd, "HTTP/1.1 404 Not Found\r\n");
                        invia(fd, "Serrrrver : Web Server in C\r\n\r\n");
                        invia(fd, "<html><head><title>404 Not Found</head></title>");
                        invia(fd, "<body><p>404 Not Found: The requested resource could not be found!</p></body></html>");
                    }
                    else {
                        printf("200 OK, Content-Type: %s\n\n",
                        estensioni[i].mediatype);
                        char prova[500];
                        sprintf(prova, "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-type: %s\r\nContent-length: %d\r\n\r\n", estensioni[i].mediatype, dim_fileDescriptor(fd1));
                        invia(fd, prova);
                        //send_new(fd, "HTTP/1.1 200 OK\r\n");
                        //send_new(fd, "Server : Web Server in C\r\n\r\n");
                        
                        // Se è una GET
                        if (ptr == richiesta + 4){
                            
                            if ((lung = dim_fileDescriptor(fd1)) == -1)
                                printf("Error in getting size !\n");
                            size_t byteInviatiTotali = 0;
                            ssize_t byteInviati;
                            printf("Blyat\n");
                            while (byteInviatiTotali < lung) {
                            //Zero copy optimization
                                if ((byteInviati = sendfile(fd, fd1, 0, lung - byteInviatiTotali)) <= 0) {
                                    if (errno == EINTR || errno == EAGAIN) {
                                        continue;
                                    }
                                    perror("ERRORE: errore nel sendfile.\n");
                                    return -1;
                                }
                                byteInviatiTotali += byteInviati;
                            }
                        }
                    }
                    break;
                }
                int size = sizeof(estensioni) / sizeof(estensioni[0]);
                if (i == size - 2) {
                    printf("415 Unsupported Media Type\n");
                    invia(fd, "HTTP/1.1 415 Unsupported Media Type\r\n");
                    invia(fd, "Server : Web Server in C\r\n\r\n");
                    invia(fd, "<html><head><title>415 Unsupported Media Type</head></title>");
                    invia(fd, "<body><p>415 Unsupported Media Type!</p></body></html>");
                }
            }
            close(fd);
        }
    }
    shutdown(fd, SHUT_RDWR);
}

int main(int argc, char* argv[]) {
    int numThread = N_THREAD;

    int sockFD, newSockFD, pid;
    pid = fork();

    if(pid < 0){
        errore("ERRORE: Errore nella fork.\n");
    }
    if(pid == 0){  // ?? TEST: FORSE MEGLIO CON ELSE ??
        threadpool tpool = thpool_init(numThread);
        socklen_t dim_cli;                       // Dimensione di un indirizzo
        struct sockaddr_in ind_serv, ind_cli; // Indirizzi

        sockFD = socket(AF_INET, SOCK_STREAM, 0);
        if(sockFD < 0)
            errore("ERRORE: errore nell'apertura del socket.");
        bzero((char*) &ind_serv, sizeof(ind_serv));

        ind_serv.sin_family = AF_INET;
        ind_serv.sin_addr.s_addr = INADDR_ANY;
        ind_serv.sin_port = htons(PORTA);
        if(bind(sockFD, (struct sockaddr *) &ind_serv, sizeof(ind_serv)) < 0)
            errore("ERRORE: errore sul binding.");
        
        listen(sockFD, 5);  /// COS'È IL 5??
        dim_cli = sizeof(ind_cli);

        // Il server va avanti per sempre, aggiungendo lavoro al t.pool per ogni connessioone
        while(1) {
            newSockFD = accept(sockFD, (struct sockaddr *) &ind_cli, &dim_cli);
            if( newSockFD < 0 )
                errore("ERRORE: errore nell'accept.");
            thpool_add_work(tpool, (void*)gestisci, &newSockFD);
            
        } //Fine del while
        close(sockFD);
        return 0;
    }
}