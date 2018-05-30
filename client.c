#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>  //non ricordo cosa conteneva
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>


int main(int argc, char **argv){
    
    char request[8192];

    switch(argc){
        case 1:
            char requestFile[256];
            printf("File to request: ");
            scanf("%s", requestFile);
        break;
        case 2:
            
        break;
    }

    


    return 0;
}