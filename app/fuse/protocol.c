//standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//networking
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>

//system calls
#include <unictd.h>
#include <fcntl.h>

//defining the port
#define PORT 3490

/*struct message_header {
    uint32_t id;
    uint8_t version;
    uint8_t opcode;
    uint8_t flags;
    uint32_t payload_length;
};

struct req_read{
    char * filepath;
    int bytes;
    int offset
};

struct resp_read{
    char * data;
    int bytes_read;
};


struct req_open{
    char * filepath;
    int mode;
};

struct resp_open{
    int fd;
};*/

struct __attribute__((packed)) message_header {
    uint32_t id;             // Unique request/response tracer
    uint8_t  version;        // Protocol version (e.g., 1)
    uint8_t  opcode;         // 1=OPEN, 2=READ, etc.
    uint8_t  flags;          // For future encryption/compression indicators
    uint32_t payload_length; // EXACT size of the struct following this header
};

// --- OPEN OPERATION ---

struct __attribute__((packed)) req_open {
    int32_t  mode;           // e.g., O_RDONLY, O_WRONLY
    uint16_t path_len;       // How long the string is
    char     filepath[0];    // "Flexible Array Member" - The string data 
                             // sits physically right here in memory.
};

struct __attribute__((packed)) resp_open {
    int64_t  remote_fh;      // Use 64-bit file handle. The server opens the file, 
                             // saves the real FD in a table, and gives the client 
                             // this ID to reference later.
};

// --- READ OPERATION ---

struct __attribute__((packed)) req_read {
    int64_t  remote_fh;      // Pass the handle we got from resp_open
    uint64_t offset;         // Files can be > 2GB, use 64-bit for offsets!
    uint32_t bytes_requested;// Max bytes you want to read
};

struct __attribute__((packed)) resp_read {
    uint32_t bytes_read;     // How many bytes the server actually successfully read
    char     data[0];        // Flexible array member. The actual raw binary file 
                             // contents are glued directly to the end of this struct.
};
void server_open(int sock_fd, uint32_t req_id, const char *path, int32_t open_mode) {
    uint16_t path_len = strlen(path);
    
    // 1. Calculate the exact total payload size (struct base size + string characters)
    // sizeof(struct req_open) is 6 bytes (4 bytes for mode + 2 bytes for path_len)
    uint32_t payload_sz = sizeof(struct req_open) + path_len;
    uint32_t total_packet_sz = sizeof(struct message_header) + payload_sz;

    // 2. Allocate a single block of memory for the whole transmission
    uint8_t *buffer = malloc(total_packet_sz);
    
    // 3. Set up the header fields
    struct message_header *hdr = (struct message_header *)buffer;
    hdr->id = req_id;
    hdr->version = 1;
    hdr->opcode = 1; // Assume 1 means OPEN
    hdr->flags = 0;
    hdr->payload_length = payload_sz;

    // 4. Set up the req_open struct metadata inside the buffer (right after header)
    struct req_open *req = (struct req_open *)(buffer + sizeof(struct message_header));
    req->mode = open_mode;
    req->path_len = path_len;

    // 5. Copy the raw string characters directly into the flexible array slot
    // req->filepath points exactly to the memory address immediately following path_len
    memcpy(&(req->filepath), path, path_len);

    // 6. Send the entire combined block across the wire in a single operation
    write(sock_fd, buffer, total_packet_sz);

    // 7. Clean up memory
    free(buffer);
}
    

static int server_read(const char *path, char *buf, size_t size, off_t offset, int sock_fd){
        read(sock_fd,)
}

static int init_client(char *address){

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

void close_sock(static int sock_fd){
    close(sock_fd);
}