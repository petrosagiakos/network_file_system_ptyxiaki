#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "file_functions.h"
#include "cmd_parser.h"


char *extra_path = "/home/peter/Documents/university/ptyhiaki/network_file_system_ptyxiaki/app/server/tests";

int tokenize_cmd(char* cmd, char *cmd_tok[], int n){
    

    // Returns first token
    char* token = strtok(cmd, " - ");

    // Keep printing tokens while one of the
    // delimiters present in str[].
    int i=0;
    while (token != NULL && i<n) {
        cmd_tok[i] = token;
        i++;

        token = strtok(NULL, " - ");
        
    }
    return i;
}

void parser(int new_fd, char * input){

    printf("received : %s",input);
    char *cmd_tok[MAX_CMD_TOKENS];
    
    int res = tokenize_cmd(input, cmd_tok, MAX_CMD_TOKENS);

    enum op_type type = get_command(cmd_tok[0]);

    switch (type) {

        case READ:

            if (cmd_tok[1] == NULL) {
                return ;
            }
            // maybe the best choice would be to create a struct in each case and parse that struct into suitable function that handles all this
            struct READ_CMD *read=malloc(sizeof(struct READ_CMD));
            read->fd = atoi(cmd_tok[1]);
            read->offset = atoi(cmd_tok[2]);
            read->bytes = atoi(cmd_tok[3]);

            serv_read(read);

            break;


        case CREATE:

            if (cmd_tok[1] == NULL) {
                return ;
            }

            struct CREATE_CMD *create= malloc(sizeof(struct CREATE_CMD));
            create->filepath = strcat(extra_path,cmd_tok[1]);
            serv_create(create);
            break;


        case DELETE:

            if (cmd_tok[1] == NULL) {
                return ;
            }
            struct DELETE_CMD *delete=malloc(sizeof(struct DELETE_CMD));
            delete->filepath = strcat(extra_path,cmd_tok[1]);
            serv_delete(delete);

            break;

        case OPEN:

            if (cmd_tok[1] == NULL) {
                return ;
            }
            struct OPEN_CMD *open=malloc(sizeof(struct OPEN_CMD));

            open->filepath=strcat(extra_path,cmd_tok[1]);
            open->mode = atoi(cmd_tok[2]);

            serv_open(open);

            break;

        case WRITE:

            if (cmd_tok[1] == NULL) {
                return ;
            }
            struct WRITE_CMD *write=malloc(sizeof(struct WRITE_CMD));

            write->fd=atoi(cmd_tok[1]);
            write->offset = atoi(cmd_tok[2]);
            write->bytes = atoi(cmd_tok[3]);
            write->data = cmd_tok[4];
            serv_write(write);

            break;
        case GETATTR:

            if (cmd_tok[1] == NULL) {
                return;
            }

            serv_getattr(new_fd, cmd_tok[1]);

            break;
        default:

            
            break;
    }
}

/*struct command parse_cmd(char *input)
{
    char *cmd_tok[MAX_CMD_TOKENS];

    struct command cmd;

    
     * Set safe default values.
   
    cmd.type = CMD_UNKNOWN;
    cmd.filename = NULL;
    cmd.offset = 0;
    cmd.bytes = 0;


    int num_tokens = tokenize_cmd(
        input,
        cmd_tok,
        MAX_CMD_TOKENS
    );


    if (num_tokens < 1) {
        return cmd;
    }


    cmd.type = get_command(cmd_tok[0]);


   
     * Filename exists for all commands that need it.
    
    if (num_tokens >= 2) {
        cmd.filename = cmd_tok[1];
    }


    
     * READ requires:
     *
     * READ - filename - offset - bytes
     
    if (cmd.type == CMD_READ) {

        if (num_tokens >= 3) {
            cmd.offset = atoi(cmd_tok[2]);
        }

        if (num_tokens >= 4) {
            cmd.bytes = atoi(cmd_tok[3]);
        }
    }


    return cmd;
}
*/
enum op_type get_command(const char *str)
{
    if (strcmp(str, "READ") == 0)
        return READ;

    if (strcmp(str, "CREATE") == 0)
        return CREATE;

    if (strcmp(str, "DELETE") == 0)
        return DELETE;
    if (strcmp(str, "OPEN") == 0)
        return OPEN;
    if (strcmp(str, "WRITE") == 0)
        return WRITE;
    if (strcmp(str, "GETATTR") == 0)
        return GETATTR;
    return UNKNOWN;
}

void serv_create(struct CREATE_CMD *create){
    return ;
}
void serv_open(struct OPEN_CMD *open){
    return ;
}
void serv_delete(struct DELETE_CMD *delete){
    return ;
}
void serv_read(int client_fd, struct READ_CMD *read){
    
    char buffer[read->bytes];
    ssize_t r;

    r = read(cmd->fd, buffer, cmd->bytes);

    if (r < 0) {
        char response[64];

        int n = snprintf(response,
                         sizeof(response),
                         "READ_RESP -1\n");

        send(client_fd, response, n, 0);
        return;
    }

   
    char response[64];

    int n = snprintf(response,
                     sizeof(response),
                     "READ_RESP %zd\n",
                     r);

    if (send(client_fd, response, n, 0) < 0) {
        return;
    }

    
    if (send_all(client_fd, buffer, r) < 0) {
        return;
    }
}
void serv_write(int_client_fd, struct WRITE_CMD *write){

    ssize_t bytes=write(write->fd,write->data,write->bytes)
   
    
    if(bytes<0){
        char response[64];
        int n= snprintf(response,sizeof(response),"WRITE_RESP -1");
        send(client_fd, response, n, 0);
        return;
    }
    char response[64];
    int n= snprintf(response,sizeof(response),"WRITE_RESP %zd",bytes);
    send(client_fd,response,n,0);

    return ;
}

void serv_getattr(int client_fd, const char *client_path)
{
    struct stat st;
    char server_path[PATH_MAX];

    if (make_server_path(client_path,
                         server_path,
                         sizeof(server_path)) < 0) {
        send(client_fd, "ERROR", 5, 0);
        return;
    }

    printf("GETATTR client path: %s\n", client_path);
    printf("GETATTR server path: %s\n", server_path);

    if (lstat(server_path, &st) == -1) {
        int error = errno;

        send(client_fd, &error, sizeof(error), 0);
        return;
    }

    int success = 0;

    send(client_fd, &success, sizeof(success), 0);
    send(client_fd, &st, sizeof(st), 0);
}
/*
int execute_file_functions(char *input)
{
    struct command cmd = parse_cmd(input);

    int op;

    switch (cmd.type) {

        case CMD_READ:

            if (cmd.filename == NULL) {
                return -1;
            }

            op = readf(
                cmd.filename,
                cmd.offset,
                cmd.bytes
            );

            break;


        case CMD_CREATE:

            if (cmd.filename == NULL) {
                return -1;
            }

            op = createf(cmd.filename);

            break;


        case CMD_DELETE:

            if (cmd.filename == NULL) {
                return -1;
            }

            op = deletef(cmd.filename);

            break;


        default:

            op = -1;
            break;
    }

    return op;
}
    */
static int send_all(int fd, const void *buffer, size_t length)
{
    size_t sent = 0;

    while (sent < length) {
        ssize_t n = send(fd,
                         (const char *)buffer + sent,
                         length - sent,
                         0);

        if (n < 0) {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (n == 0)
            return -1;

        sent += n;
    }

    return 0;
}
static int make_server_path(const char *client_path,
                            char *server_path,
                            size_t size)
{
    if (client_path == NULL || client_path[0] != '/') {
        return -1;
    }

    int n = snprintf(server_path,
                     size,
                     "%s%s",
                     EXPORT_DIR,
                     client_path);

    if (n < 0 || (size_t)n >= size) {
        return -1;
    }

    return 0;
}
