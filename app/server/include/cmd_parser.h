#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#define MAX_CMD_TOKENS 4
enum command_type{
    UNKNOWN,
    READ,
    CREATE,
    DELETE,
    OPEN,
    WRITE
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
    char *filepath
 };

struct WRITE_CMD{
    //char *filepath; //or
    int fd;
    int offset;
    int bytes;
    char *data;

}
 struct DELETE_CMD{
    char *filepath
 };
int tokenize_cmd(char* cmd, char *cmd_tok[], int n);
struct command  parse_cmd(char *input);

enum command_type get_command(const char *str);

int execute_file_functions(char *input);

#endif