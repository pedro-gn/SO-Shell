#ifndef SHELLUTILS_H
#define SHELLUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <wait.h>

#define BUFFER_SIZE 1024
#define COMMAND_DELIMITERS " \t\r\n\a"

void typePrompt();

char *readCommand();

char **split_command(char *command);

int shell_execute(char **args);


#endif