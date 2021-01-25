#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_TOKEN_SIZE 64
#define DELIMETERS " \n\t"
#define BACKGROUND 0;
#define FOREGROUND 1;

int mode = FOREGROUND;

char *read_input(void)
{
    // int buffer = 2048;
    char *input = NULL; //malloc(buffer * sizeof(char));
    size_t len = 0;

    getline(&input, &len, stdin);

    input[strlen(input) - 1] = '\0';

    return input;
}

void clean_exit(int status)
{
    fputs("Closing Shell... Goodbye\n", stdout);
    exit(status);
}

char **split_line(char *input)
{
    char **tokens = malloc(MAX_TOKEN_SIZE * sizeof(char *));
    // char *input_copy = (char *) malloc(strlen(input) + 1);

    // strcpy(input_copy, input);
    // input_copy[strlen(input)] = '\0';

    char *token;

    int idx = 0;

    token = strtok(input, DELIMETERS);

    while (token != NULL)
    {
        tokens[idx++] = token;
        token = strtok(NULL, DELIMETERS);
        // add code for more memory allocation...
    }

    // free(input_copy);
    // input_copy = NULL;

    return tokens;
}

// DEBUGGING FUNCTION
void print_args(char **args)
{
    for (int i = 0; args[i] != NULL; i++)
        printf("args[%2d]: %s\n", i, args[i]);
}

void backgroundCheck(char *cmd_line)
{
    printf("Last Char: %c\n", cmd_line[strlen(cmd_line) - 1]);
    if (cmd_line[strlen(cmd_line) - 1] == '&')
    {
        mode = BACKGROUND;
        cmd_line[strlen(cmd_line) - 1] = '\0';
    }
    else
    {
        mode = FOREGROUND;
    }
    printf("Mode: %d\n", mode);
}

int run_command(char **args)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        execvp(args[0], args);
        fputs("\n", stdout);
    }
    else if (pid < 0)
    {
        fputs("ERROR in fork() system call", stderr);
        return 0;
    }
    else
    {
        if (mode)
        {
            wait(NULL);
        }
        else
        {
            printf("[%d]\n", pid);
        }
    }

    return 1;
}

int execute_args(char **args)
{

    if (!strcmp(args[0], "exit"))
        return 0;

    if (args[0] == NULL)
        return 1;

    return run_command(args);
}

int main(void)
{
    // char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
    char *line;  // string to hold input line
    char **args; // array of strings to hold args

    int should_run = 1;

    do
    {
        printf("KShell: $ ");
        fflush(stdout);

        /**
          * After reading user input, the steps are:
          * (1) fork a child process
          * (2) the child process will invoke execvp()
          * (3) if command includes &, parent and child will run concurrently
          */

        line = read_input();
        args = split_line(line);
        backgroundCheck(line);
        print_args(args);
        should_run = execute_args(args);

        free(line);
        free(args);

    } while (should_run);

    clean_exit(EXIT_SUCCESS);

    return 0;
}