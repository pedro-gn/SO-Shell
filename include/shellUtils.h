#ifndef SHELLUTILS_H
#define SHELLUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define PIPE_DELIM "|"
#define SPACE_DELIM " \t\r\n\a"

void typePrompt();

char *readCommand();

void exec_single_command(char *command);

char **split_command(char *command, char *delim);

int shell_execute(char **args);

void exec_piped_commands(char *command1, char *command2);

#endif