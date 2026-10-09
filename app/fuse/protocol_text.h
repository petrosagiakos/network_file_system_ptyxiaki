#ifndef PROTOCOL_TEXT_H
#define PROTOCOL_TEXT_H


#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>


//defining the port
#define PORT "3490"
#define DEFAULT_BUFFER_SIZE 1024

int server_open(int sock_fd, const char *path, int32_t open_mode);

ssize_t server_read(int sock_fd, int fd, char *buf, size_t size, off_t offset);

int init_client(char *address);

void close_sock(int sock_fd);

#endif