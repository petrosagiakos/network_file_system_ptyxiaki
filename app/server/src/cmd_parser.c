#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "file_functions.h"
#include "cmd_parser.h"

#define MAX_CMD_TOKENS 4



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
struct command parse_cmd(char *input)
{
    char *cmd_tok[MAX_CMD_TOKENS];

    struct command cmd;

    /*
     * Set safe default values.
     */
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


    /*
     * Filename exists for all commands that need it.
     */
    if (num_tokens >= 2) {
        cmd.filename = cmd_tok[1];
    }


    /*
     * READ requires:
     *
     * READ - filename - offset - bytes
     */
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

enum command_type get_command(const char *str)
{
    if (strcmp(str, "READ") == 0)
        return CMD_READ;

    if (strcmp(str, "CREATE") == 0)
        return CMD_CREATE;

    if (strcmp(str, "DELETE") == 0)
        return CMD_DELETE;

    return CMD_UNKNOWN;
}

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