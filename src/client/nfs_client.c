/*#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490"

int main(int argc, char *argv[])
{   
    printf("before connected...");
    int sock_fd;
    struct addrinfo hints, *res;\

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(argv[1], PORT, &hints, &res);
    
    sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    printf("before connected...");
    if(connect(sock_fd, res->ai_addr, res->ai_addrlen)<0){
        
        printf("Server down");
        return 1;
    }
    printf("connected...");
    char *cmd="hello";
    char buffer[100];
    while(1){
        send(sock_fd,cmd,strlen(cmd),0);
        recv(sock_fd,buffer,100,0);
        printf("server: %s",buffer);
    }
    return 0;
}

*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "3490"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server>\n", argv[0]);
        return 1;
    }

    int sock_fd;
    struct addrinfo hints, *res;
    int status;

    printf("before connected...\n");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(argv[1], PORT, &hints, &res);

    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    sock_fd = socket(res->ai_family,
                     res->ai_socktype,
                     res->ai_protocol);

    if (sock_fd == -1) {
        perror("socket");
        freeaddrinfo(res);
        return 1;
    }

    printf("before connect...\n");

    if (connect(sock_fd, res->ai_addr, res->ai_addrlen) < 0 ) {
        perror("connect");
        close(sock_fd);
        freeaddrinfo(res);
        return 1;
    }

    printf("connected...\n");

    freeaddrinfo(res);

    char *cmd = "hello";
    char buffer[100];
    
    while (1) {
        if (send(sock_fd, cmd, strlen(cmd), 0) == -1) {
            perror("send");
            break;
        }

        int n = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);

        if (n == -1) {
            perror("recv");
            break;
        }

        if (n == 0) {
            printf("Server disconnected.\n");
            break;
        }

        buffer[n] = '\0';
        printf("server: %s\n", buffer);
    }

    close(sock_fd);
    return 0;
}
//gcc nfs_client.c -o nfs_client