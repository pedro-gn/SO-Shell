#include "shellMain.h"
#include "shellUtils.h"

void shellMain(){
    char *command = NULL;
    char **commands = NULL;
    int status;


    while(true){
        typePrompt();
        command = readCommand(&command);

        commands = split_command(command, PIPE_DELIM);

        status = shell_execute(commands);
        
        

        free(command);
        free(commands);
    }
    
}