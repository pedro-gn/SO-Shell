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
        } else if (strcmp(args[i], "|") == 0) {
            // Símbolo de encadeamento de comandos encontrado
            args[i] = NULL;

            // Cria um pipe para conectar a saída do primeiro comando com a entrada do segundo
            int pipe_fd[2];

            if (pipe(pipe_fd) == -1) {
                printf("Erro ao criar pipe\n");
                return 0;
            }

            // Cria um processo filho para o primeiro comando
            pid = fork();
            if (pid == 0) {
                // Processo filho
                if (input_file != NULL) {
                    // Redireciona a entrada do arquivo
                    fd_in = open(input_file, O_RDONLY);
                    dup2(fd_in, STDIN_FILENO);
                    close(fd_in);
                } else if (i > 0) {
    
                    // Redireciona a entrada padrão para o pipe
                    dup2(pipe_fd[1], STDOUT_FILENO);
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                }

                if (output_file != NULL && args[i + 1] == NULL) {
                    // Redireciona a saída para o arquivo
                    fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                }

                // Executa o primeiro comando
                execvp(args[0], args);
                printf("Erro ao executar o comando\n");
                exit(EXIT_FAILURE);
            } else if (pid < 0) {
                // Erro ao criar processo filho
                printf("Erro ao criar processo filho\n");
                return 0;
            }

            // Cria um processo filho para o segundo comando
            pid_t pid2 = fork();
            if (pid2 == 0) {
                // Processo filho
                if (output_file != NULL) {
                    // Redireciona a saída para o arquivo
                    fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    dup2(fd_out, STDOUT_FILENO);
                    close(fd_out);
                } else if (args[i + 1] != NULL) {
                    // Redireciona a saída padrão para o pipe
                    close(pipe_fd[1]);
                    dup2(pipe_fd[0], STDIN_FILENO);
                    close(pipe_fd[0]);
                    }
            // Executa o segundo comando
            execvp(args[i + 1], args + i + 1);
            printf("Erro ao executar o comando\n");
            exit(EXIT_FAILURE);
        } else if (pid2 < 0) {
            // Erro ao criar processo filho
            printf("Erro ao criar processo filho\n");
            return 0;
        }
        // Fecha as extremidades do pipe que não serão usadas
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        // Aguarda a finalização dos processos filhos
        waitpid(pid, &status, 0);
        waitpid(pid2, &status, 0);
        // Prepara para o próximo comando, se houver
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        
        input_file = NULL;
        output_file = NULL;
    }
}
// Se não houver símbolo de encadeamento de comandos, executa o comando normalmente
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
pid = fork();
if (pid == 0) {
    // Processo filho
    execvp(args[0], args);
    printf("Erro ao executar o comando\n");
    exit(EXIT_FAILURE);
} else if (pid < 0) {
    // Erro ao criar processo filho
    printf("Erro ao criar processo filho\n");
    return 0;
}
waitpid(pid, &status, 0);
}
