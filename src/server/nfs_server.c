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
//#include "command_validator.h"

#define PORT "3490" 
void tokenize_cmd(char* cmd, char *cmd_tok[]){
    

    // Returns first token
    char* token = strtok(cmd, " - ");

    // Keep printing tokens while one of the
    // delimiters present in str[].
    int i=0;
    while (token != NULL) {
        cmd_tok[i] = token;
        i++;

        token = strtok(NULL, " - ");
        
    }
}

int readf(char* filename,int offset,int bytes){
    char file_content[20];
    int fd1 = open(filename, O_RDONLY);
    read(fd1,file_content,20);
    printf("file: %s\n content: %s \n",filename,file_content);
    close(fd1);
    return 1;
}
int createf(char* filename){
    int fd2=open(filename,O_CREAT);
    printf("file: %s created succesfully \n",filename);
    close(fd2);
    return 2;
}
int deletef(char * filename){
    int fd3=open(filename,O_TRUNC);
    printf("file: %s deleted succesfully \n",filename);
    close(fd3);
    return 3;
}


enum command {
    CMD_UNKNOWN,
    CMD_READ,
    CMD_CREATE,
    CMD_DELETE
};

enum command get_command(const char *str)
{
    if (strcmp(str, "READ") == 0)
        return CMD_READ;

    if (strcmp(str, "CREATE") == 0)
        return CMD_CREATE;

    if (strcmp(str, "DELETE") == 0)
        return CMD_DELETE;

    return CMD_UNKNOWN;
}

int execute_file_functions(char *command, char *file){
    int op;
    switch(get_command(command)){
        case CMD_READ:
            op=readf(file,0,0);
            break;
        case CMD_CREATE:
            op=createf(file);
            break;
        case CMD_DELETE:
            op=deletef(file);
            break;
        default:
            op=-1;
    }
    return op;
}

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
    char buffer[100];//to change
    socklen_t addr_size;
    char *text="test";
    while(1){
        addr_size = sizeof their_addr;
        new_fd = accept(sock_fd, (struct sockaddr *)&their_addr, &addr_size);
        char *cmd[2];
        printf("connection accepted: %i",new_fd);
        while(1){
            int n = recv(new_fd, buffer, sizeof(buffer) - 1, 0);

            if (n > 0) {
                buffer[n] = '\0';
                printf("client: %s\n", buffer);
            }
            tokenize_cmd(buffer,cmd);
            printf("command:%s\n",cmd[0]);
            printf("file:%s\n",cmd[1]);
            int result = execute_file_functions(cmd[0],cmd[1]);
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