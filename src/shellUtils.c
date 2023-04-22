#include "shellUtils.h"



// Texto do Prompt 
void typePrompt(){
    printf("$ ");
}


// Lê da entrada padrão até um \n
char *readCommand(){
    char *command = NULL;
    size_t bsize = 0;

    getline(&command, &bsize, stdin);

    return command;
}

// Separa o comando en tokens para ser executado pelo executados
char **split_command(char *command){
    int buffer_size = BUFFER_SIZE, position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(command, COMMAND_DELIMITERS);

    while (token != NULL) {
        tokens[position] = token;
        position++;

        // Se a memoria não for o suficiente para linha de entrada realoca dobrando a memoria.
        if (position >= buffer_size) {
            buffer_size += BUFFER_SIZE;
            tokens = realloc(tokens, buffer_size * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, COMMAND_DELIMITERS);
    }

    tokens[position] = NULL;
    return tokens;
}


int shell_execute(char **args) {
    pid_t pid;
    int status;

    if (args[0] == NULL) {
        return 1;
    }
    
    if ((pid = fork()) == 0) {
        if (execvp(args[0], args) == -1) {
            perror("shell");
        }
        exit(EXIT_FAILURE);

    } else if (pid < 0) {
        perror("shell");

    } else {
        waitpid(pid, &status, WUNTRACED);
    }

    return 1;
}