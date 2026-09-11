#include <stdio.h>
#include <string.h>

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