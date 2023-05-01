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
                free(tokens);
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, delim);
    }

    tokens[position] = NULL;
    return tokens;
}


int count_pipes(char **commands){
    if(commands == NULL){
        return 0;
    }

    int count = 0;
    for(int i = 0; commands[i] != NULL; i++){
        count++;
    }
    return count-1;
}

int count_args(char **args){
    int count = 0;
    for(int i = 0; args[i] != NULL; i++){
        count++;
    }
    return count;
}

int is_valid_input(char **commands){
    int count = 0;
    while (commands[count] != NULL){
        if(strcmp(commands[count], "\n") == 0){
            return 0;
        }
        count++;
    }

    return 1;
}

int shell_execute(char **commands) {
    int status;
    int nPipes = count_pipes(commands); 

    // Checa a sintaxe
    if(!is_valid_input(commands)){
        return 1;
    }

    int **pipe_fds = malloc(nPipes * sizeof(int*));

    // Cria os pipes
    for (int i = 0; i < nPipes; i++){
        pipe_fds[i] = malloc(2 * sizeof(int));     
        if (pipe(pipe_fds[i]) == -1) {
            printf("Erro ao criar pipe\n");
            return 1;
        }
    }


    // Executa os comandos
    pid_t pid;
    for(int i = 0; commands[i] != NULL; i++){
        char **args = split_command(commands[i], SPACE_DELIM);

        int numArgs = count_args(args);
        int background = 0;

        if(strcmp(args[numArgs-1], "&") == 0){
            args[numArgs-1] = NULL;
            background = 1;
        }

        // Arquivos de redirecionamento
        int fd_in, fd_out;
        char* input_file = NULL;
        char* output_file = NULL;

        
        // Procura pelos símbolos de redirecionamento de entrada e saída
        for (int i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], "<=") == 0) {
                // Símbolo de redirecionamento de entrada encontrado
                input_file = args[i + 1];
                args[i] = NULL;
            }else if (strcmp(args[i], "=>") == 0) {
                // Símbolo de redirecionamento de saída encontrado
                output_file = args[i + 1];
                args[i] = NULL;
            }
        }


        pid = fork();
        if(pid == 0){

            // Se não for o primeiro comando
            if (i > 0) {
                // Redireciona a entrada para o pipe anterior
                close(pipe_fds[i-1][1]);
                dup2(pipe_fds[i-1][0], STDIN_FILENO);
                sleep(0.2);
                close(pipe_fds[i-1][0]);
            }
            // Se não for o ultimo comando
            if(i < nPipes){
                // Redireciona a saída para o pipe atual
                close(pipe_fds[i][0]);
                dup2(pipe_fds[i][1], STDOUT_FILENO);
                close(pipe_fds[i][1]);
            }


            if (input_file != NULL) {
                // Redireciona a entrada do arquivo
                fd_in = open(input_file, O_RDONLY);
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }else if (output_file != NULL) {
                // Redireciona a saída para o arquivo
                fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }


            // Executa o comando 
            execvp(args[0], args);
            printf("Erro ao executar o comando\n");
            exit(EXIT_FAILURE);
        }else if(pid < 0){
            // Erro ao criar processo filho
            printf("Erro ao criar processo filho\n");
            return 1;
        }
        if(background == 1){
            printf("Executando em background: %s\n", args[0]);
        }
        free(args);
    }


    for (int i = 0; i < nPipes; i++) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    for (int i = 0; i < nPipes+1; i++) {
        waitpid(-1, &status, 0);
    }

    for (int i = 0; i < nPipes; i++) {
        free(pipe_fds[i]);
    }
    free(pipe_fds);

    return 0;
}
