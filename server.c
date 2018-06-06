/**
 * Struttura del programma:
 * Avvio: dichiara le variabili, porta ecc..
 *      Crea un socket per l'ascolto
 *      Chiama listen() per prepararsi a ricevere
 * 
 * 
    while (1) {
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
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "C-Thread-Pool-master/thpool.h"

#define PORT 10000      //porta del server
#define SERVER_PROTOCOL "HTTP/1.1"

#define EOL "\r\n"
#define EOL_SIZE 2

typedef struct socketOldAndNew {
    int socketFD;
    int newSocketFD;
} SockOldNew;

typedef struct {
 char *ext;
 char *mediatype;
} extn;

//Possible media types
extn extensions[] ={
 {"gif", "image/gif" },
 {"txt", "text/plain" },
 {"jpg", "image/jpg" },
 {"jpeg","image/jpeg"},
 {"png", "image/png" },
 {"ico", "image/ico" },
 {"zip", "image/zip" },
 {"gz",  "image/gz"  },
 {"tar", "image/tar" },
 {"htm", "text/html" },
 {"html","text/html" },
 {"php", "text/html" },
 {"pdf","application/pdf"},
 {"zip","application/octet-stream"},
 {"rar","application/octet-stream"},
 {0,0} };

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void init(){
}

char* webroot() {
    return "/var/www/html/";
}
/*
A helper function
*/
int get_file_size(int fd) {
    struct stat stat_struct;
    if (fstat(fd, &stat_struct) == -1)
        return (1);
    return (int) stat_struct.st_size;
}

/*
 A helper function
 */
void send_new(int fd, char *msg) {
    int len = strlen(msg);
    if (send(fd, msg, len, 0) == -1) {
        printf("Error in send\n");
    }
}

/*
 This function recieves the buffer
 until an "End of line(EOL)" byte is recieved
 */
int recv_new(int fd, char *buffer) {
    char *p = buffer; // Use of a pointer to the buffer rather than dealing with the buffer directly
    int eol_matched = 0; // Use to check whether the recieved byte is matched with the buffer byte or not

    // Start receiving 1 byte at a time
    while (recv(fd, p, 1, 0) != 0) {
        // if the byte matches with the first eol byte that is '\r'
        if (*p == EOL[eol_matched]) {
            ++eol_matched;

        // if both the bytes matches with the EOL
        if (eol_matched == EOL_SIZE) {
            *(p + 1 - EOL_SIZE) = '\0'; // End the string
            return (strlen(buffer)); // Return the bytes recieved
        }
        } else {
            eol_matched = 0;
        }

        p++; // Increment the pointer to receive next byte
    }
    return (0);
}

int handleConnection(SockOldNew* sockPointer){
    SockOldNew s = *sockPointer;

    char request[500], resource[500], *ptr;
    int fd1, length;
    // Receive request on socket   
    
    if (recv_new(s.socketFD, request) == 0)
        printf("Receive Failed\n");
    printf("%s\n", request);

    // Check for a valid browser request
    ptr = strstr(request, " HTTP/");

    if (ptr == NULL) {
        printf("NOT HTTP !\n");
    } else {
        *ptr = 0;
        ptr = NULL;

        if (strncmp(request, "GET ", 4) == 0) {
            ptr = request + 4;
        }
        if (ptr == NULL) {
            printf("Unknown Request ! \n");
        } else {
            if (ptr[strlen(ptr) - 1] == '/') {
                strcat(ptr, "index.html");
            }
            strcpy(resource, webroot());
            strcat(resource, ptr);

            char* ex = strchr(ptr, '.'); // Now ptr holds a pointer to the extension
            int i;
            for (i = 0; extensions[i].ext != NULL; i++) { // Check which extension the request is
                if (strcmp(ex + 1, extensions[i].ext) == 0) {
                    fd1 = open(resource, O_RDONLY, 0);
                    printf("Opening \"%s\"\n", resource);
                    if (fd1 == -1) {
                        printf("404 File not found Error\n");
                        send_new(s.socketFD, "HTTP/1.1 404 Not Found\r\n");
                        send_new(s.socketFD, "Server : Web Server in C\r\n\r\n");
                        send_new(s.socketFD, "<html><head><title>404 Not Found</head></title>");
                        send_new(s.socketFD, "<body><p>404 Not Found: The requested resource could not be found!</p></body></html>");
                        //Handling php requests
                    } else if (strcmp(extensions[i].ext, "php") == 0) {
                        //php_cgi(resource, s.socketFD);
                        sleep(1);
                        close(s.socketFD);
                        exit(1);
                    } else {
                        printf("200 OK, Content-Type: %s\n\n",
                        extensions[i].mediatype);
                        send_new(s.socketFD, "HTTP/1.1 200 OK\r\n");
                        send_new(s.socketFD, "Server : Web Server in C\r\n\r\n");
                        // if it is a GET request
                        if (ptr == request + 4){
                            if ((length = get_file_size(fd1)) == -1)
                                printf("Error in getting size !\n");
                            size_t total_bytes_sent = 0;
                            ssize_t bytes_sent;
                            while (total_bytes_sent < length) {
                            //Zero copy optimization
                                if ((bytes_sent = sendfile(s.socketFD, fd1, 0, length - total_bytes_sent)) <= 0) {
                                    if (errno == EINTR || errno == EAGAIN) {
                                        continue;
                                    }
                                    perror("sendfile");
                                    return -1;
                                }
                                total_bytes_sent += bytes_sent;
                            }

                        }
                    }
                    break;
                }
                int size = sizeof(extensions) / sizeof(extensions[0]);
                if (i == size - 2) {
                    printf("415 Unsupported Media Type\n");
                    send_new(s.socketFD, "HTTP/1.1 415 Unsupported Media Type\r\n");
                    send_new(s.socketFD, "Server : Web Server in C\r\n\r\n");
                    send_new(s.socketFD, "<html><head><title>415 Unsupported Media Type</head></title>");
                    send_new(s.socketFD, "<body><p>415 Unsupported Media Type!</p></body></html>");
                }
            }

            close(s.socketFD);
        }
    }
    shutdown(s.socketFD, SHUT_RDWR);
}

int main(int argc, char **argv){
    int numThreads = 4;
    switch(argc){
        case 1:
        break;
        case 2:
            numThreads = atoi(argv[1]);
        break;
        default:
            printf("Fkn rtrd.\n");
            return EXIT_FAILURE;
        break;
    }
    // Initialization
    threadpool tpool = thpool_init(numThreads);  // Initializing the threadpool
    
    SockOldNew sockOldNew; // Includes sockfd and newsockfd
    int sockfd, newsockfd, pid;
    socklen_t clilen; // Size of address
    struct sockaddr_in serv_addr, cli_addr; // Addresses

    sockOldNew.socketFD = socket(AF_INET, SOCK_STREAM, 0); // Opening socket
    if (sockOldNew.socketFD < 0)
        error("Error opening socket!");
    bzero((char *) &serv_addr, sizeof(serv_addr)); // Zeroes serv_addr

    serv_addr.sin_family = AF_INET;  // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);
    if (bind(sockOldNew.socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  // Give the socket's FD his local address
        error("Error on binding!");
    listen(sockOldNew.socketFD, 4);  
    clilen = sizeof(cli_addr);


    while (1) {
        sockOldNew.newSocketFD = accept(sockOldNew.socketFD, (struct sockaddr *) &cli_addr, &clilen);
        if (sockOldNew.newSocketFD < 0)
            error("ERROR on accept");
        thpool_add_work(tpool, (void*)handleConnection, &sockOldNew);

    } /* end of while */
    close(sockOldNew.socketFD);
    return 0; /* we never get here */

    return (EXIT_SUCCESS);
}