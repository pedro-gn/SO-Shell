#include "shellUtils.h"


#define MAX_COMMAND_SIZE 80

void typePrompt(){
    printf("$ ");
}


char *readCommand(){
    char command[MAX_COMMAND_SIZE];

    fgets(command, MAX_COMMAND_SIZE, stdin);
    printf("%s", command);
}