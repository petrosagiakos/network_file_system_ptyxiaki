#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


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