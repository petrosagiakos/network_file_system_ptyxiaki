#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include "cmd_parser.h"

#define PORT "3490" 

#define DEFAULT_BUFFER 256



int main(){
    int sock_fd, new_fd;
    struct addrinfo hints, *res;
    struct sockaddr_storage their_addr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;  
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, "3490", &hints, &res);

    sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    bind(sock_fd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    if (listen(sock_fd, 10) == -1) {
        perror("listen");
        exit(1);
    }
    char buffer[DEFAULT_BUFFER];//to change
    socklen_t addr_size;
    char *text="test";
    while(1){
        addr_size = sizeof their_addr;
        new_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);
        
        printf("connection accepted: %i",new_fd);
        while(1){
            int n = recv(new_fd, buffer, sizeof(buffer) - 1, 0);

            if (n > 0) {
                buffer[n] = '\0';
                printf("client: %s\n", buffer);
            }
          
         
            int result=execute_file_functions(buffer);
            printf("result: %d\n",result);
            if(result<0){
                printf("unknown command");
            }
            send(new_fd,text,strlen(text),0);
        }
        close(new_fd);
        
    }
    return 0;
}
//gcc nfs_server.c -o nfs_server