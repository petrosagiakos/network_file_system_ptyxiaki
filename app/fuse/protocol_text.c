#include "protocol_text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netdb.h>



static int send_all(int sock_fd, const char *buffer, size_t length)
{
    size_t sent = 0;

    while (sent < length) {
        ssize_t n = send(sock_fd,
                         buffer + sent,
                         length - sent,
                         0);

        if (n < 0) {
            return -1;
        }

        if (n == 0) {
            return -1;
        }

        sent += (size_t)n;
    }

    return 0;
}

int server_open(int sock_fd, const char *path, int32_t open_mode) {
    char buffer[1024];
    int n=sprintf(buffer,"OPEN /%s %d",path,open_mode);
    if(send_all(sock_fd,buffer,(size_t)n)<0){        
        return -1;
    }
    memset(&buffer[0], 0, sizeof(buffer));
    ssize_t received=recv(sock_fd,buffer,sizeof(buffer)-1,0);
    if(received<0){
        return -1;
    }
    buffer[received] = '\0';
    int fd = atoi(buffer);
    return fd;
}
    
ssize_t server_read(int sock_fd,int fd, char * buf, size_t size, off_t offset ){
        

        
        char buffer[DEFAULT_BUFFER_SIZE];
        int n=sprintf(buffer,"READ /%d %lld %zu",fd,(long long)offset,size);
        
        if(send_all(sock_fd,buffer,(size_t)n)<0){
            return -1;
        }
        
      
        
        ssize_t bytes_received = recv(sock_fd,buf,size,0);
           
        return bytes_received;
}

int init_client(char *address){

    int sock_fd; //file descriptor
    struct addrinfo hints, *res; //res will hold the actual connection parameters
    int status;



    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP

    /*The getaddrinfo() function bellow parses an ip address as char*, a port as char* 
    and a dummy struct addrinfo specifing the protocol and ip version 
    to a struct addrinfo alltogether*/

    status = getaddrinfo(address, PORT, &hints, &res); 

    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }

    // creating the network socket
    sock_fd = socket(res->ai_family,
                     res->ai_socktype,
                     res->ai_protocol);

    if (sock_fd == -1) {
        perror("socket");
        freeaddrinfo(res);
        return -1;
    }

    //actual connection to server
    if (connect(sock_fd, res->ai_addr, res->ai_addrlen) < 0 ) {

        perror("connect");
        close(sock_fd);
        freeaddrinfo(res);
        return -1;
    }
    return sock_fd;
}

void close_sock( int sock_fd){
    close(sock_fd);
}