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
char **split_command(char *command, char *delim){
    int buffer_size = BUFFER_SIZE, position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(command, delim);

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

        token = strtok(NULL, delim);
    }

    tokens[position] = NULL;
    return tokens;
}

void exec_single_command(char *command){
    char **args = split_command(command, SPACE_DELIM);

    int fd_in, fd_out;
    char* input_file = NULL;
    char* output_file = NULL;

    pid_t pid;
    int status;

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
        return;
    }
    waitpid(pid, &status, 0);
    free(args);
}


// Executa os comandos em pipe, saida do command1 sera redirecionada para entrada do command2
void exec_piped_commands(char *command1, char *command2){
    char **args1 = split_command(command1, SPACE_DELIM);
    char **args2 = split_command(command2, SPACE_DELIM);

    int pipe_fd[2];

    int fd_in, fd_out;
    char* input_file = NULL;
    char* output_file = NULL;

    pid_t pid1,pid2;
    int status;

    // Procura pelos símbolos de redirecionamento de entrada e saída
    for (int i = 0; args1[i] != NULL; i++) {
        if (strcmp(args1[i], "<=") == 0) {
            // Símbolo de redirecionamento de entrada encontrado
            input_file = args1[i + 1];
            args1[i] = NULL;
        }
    }

    for (int i = 0; args2[i] != NULL; i++) {
        if (strcmp(args2[i], "=>") == 0) {
                // Símbolo de redirecionamento de saída encontrado
                output_file = args2[i + 1];
                args2[i] = NULL;
        }
    }

    if (pipe(pipe_fd) == -1) {
        printf("Erro ao criar pipe\n");
        return ;
    }

    // Primeiro Comando
    pid1 = fork();
    if (pid1 == 0) {
        // Processo filho
        if (input_file != NULL) {
            // Redireciona a entrada do arquivo
            fd_in = open(input_file, O_RDONLY);
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        } 

        // Redireciona a saida padrão para o pipe
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);

        // Executa o primeiro comando
        execvp(args1[0], args1);
        printf("Erro ao executar o comando\n");
        exit(EXIT_FAILURE);
    } else if (pid1 < 0) {
        // Erro ao criar processo filho
        printf("Erro ao criar processo filho\n");
        return;
    }

    waitpid(pid1, &status, 0);


    // Segundo Comando
    pid2 = fork();
    if (pid2 == 0) {
        // Processo filho
        if (output_file != NULL) {
            // Redireciona a saída para o arquivo
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        } 

        // Redireciona a entrada padrão para o pipe
        close(pipe_fd[1]);
        dup2(pipe_fd[0], STDIN_FILENO);
        close(pipe_fd[0]);
        // Executa o segundo comando
        execvp(args2[0], args2);
        printf("Erro ao executar o comando\n");
        exit(EXIT_FAILURE);
    } else if (pid2 < 0) {
        // Erro ao criar processo filho
        printf("Erro ao criar processo filho\n");
        return;
    }




    waitpid(pid2, &status, 0);

    free(args1);
    free(args2);
}

int shell_execute(char **args) {

    int status;

    for(int i = 0; args[i] != NULL; i++){
        if( args[i+1] != NULL){
            //PIPE
        } 
        else{
            exec_single_command(args[i]);
        }
    }


    return 1;
    
}
