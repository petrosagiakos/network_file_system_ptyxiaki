#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#define MAX_CMD_TOKENS 4
enum command_type{
    CMD_UNKNOWN,
    CMD_READ,
    CMD_CREATE,
    CMD_DELETE
};

struct command {
    enum command_type type;
    char *filename;
    int offset;
    int bytes;
};

int tokenize_cmd(char* cmd, char *cmd_tok[], int n);
struct command  parse_cmd(char *input);

enum command_type get_command(const char *str);

int execute_file_functions(char *input);

#endif