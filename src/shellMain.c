#include "shellMain.h"
#include "shellUtils.h"

void shellMain(){
    char *command = NULL;
    char **commands = NULL;
    int status;


    while(true){
        typePrompt();
        command = readCommand(&command);

        // Tokeniza por simbolos de pipes
        commands = split_command(command, PIPE_DELIM);
        
        // Procura pelo fim do programa
        if (strcmp(command,"fim\n") == 0){
            return;
        }


        status = shell_execute(commands);
        

        free(command);
        free(commands);
    }
    
}