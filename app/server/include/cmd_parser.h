#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#define MAX_CMD_TOKENS 5
enum op_type{
    UNKNOWN,
    READ,
    CREATE,
    DELETE,
    OPEN,
    WRITE,
    GETATTR,
    READDIR,
};

/*struct command {
    enum op_type type;
    char *filename;
    int offset;
    int bytes;
};*/

/* 
 * Instead of having just one struct command 
 * with many fields that are either activated or deactivated
 * it will be used many struct as differnet type of commands.
 * For example there will be the struct READ_CMD with its coresponding fields
 */

 struct READ_CMD{
    //char * filepath; //or
    int fd;
    int offset;
    int bytes;
 };

 struct OPEN_CMD{
    char *filepath;
    int mode;
 };

 struct CREATE_CMD{
    char *filepath;
 };

struct WRITE_CMD{
    //char *filepath; //or
    int fd;
    int offset;
    int bytes;
    char *data;

};
 struct DELETE_CMD{
    char *filepath;
 };
int tokenize_cmd(char* cmd, char *cmd_tok[], int n);
//struct command  parse_cmd(char *input);
void parser(int new_fd, char * input);

enum op_type get_command(const char *str);


void serv_create(struct CREATE_CMD *create);
void serv_open(struct OPEN_CMD *open);
void serv_delete(struct DELETE_CMD *delete);
void serv_read(struct READ_CMD *read);
void serv_write(struct WRITE_CMD *write);

//int execute_file_functions(char *input);

static int make_server_path(const char *client_path,
                            char *server_path,
                            size_t size);

#endif