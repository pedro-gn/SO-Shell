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
    int fd_in, fd_out;
    char* input_file = NULL;
    char* output_file = NULL;

    // Procura pelos símbolos de redirecionamento de entrada e saída
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<=") == 0) {
            // Símbolo de redirecionamento de entrada encontrado
            input_file = args[i + 1];
            args[i] = NULL;
        } else if (strcmp(args[i], "=>") == 0) {
            // Símbolo de redirecionamento de saída encontrado
            output_file = args[i + 1];
            args[i] = NULL;
        }
    }

    pid = fork();

    if (pid == 0) {
        // Processo filho
        if (input_file != NULL) {
            // Redireciona a entrada do arquivo
            fd_in = open(input_file, O_RDONLY);
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        if (output_file != NULL) {
            // Redireciona a saída para o arquivo
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        execvp(args[0], args);
        printf("Erro ao executar o comando\n");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Erro ao criar processo filho
        printf("Erro ao criar processo filho\n");
    } else {
        // Processo pai
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;   
}