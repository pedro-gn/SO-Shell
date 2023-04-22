#include "shellMain.h"
#include "shellUtils.h"

void shellMain(){
    char *command = NULL;
    char **commandArgs = NULL;
    int status;


    while(true){
        typePrompt();
        command = readCommand(&command);
        commandArgs = split_command(command);
        status = shell_execute(commandArgs);

        free(command);
        free(commandArgs);
    }
    
}