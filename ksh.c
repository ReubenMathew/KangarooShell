#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_TOKEN_SIZE 64
#define DELIMETERS " \n\t"

char *read_input(void)
{
    // int buffer = 2048;
    char *input = NULL; //malloc(buffer * sizeof(char));
    size_t len = 0;

    getline(&input, &len, stdin);

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
    char *token;

    int idx = 0;

    token = strtok(input, DELIMETERS);

    while (token != NULL)
    {
        tokens[idx++] = token;
        token = strtok(NULL, DELIMETERS);
        // add code for more memory allocation...
    }

    return tokens;
}

int execute_args(char **args)
{

    // if (!strcmp(args[0], "exit")){

    //     return 0;
    // }

    // if (args[0] == NULL)
    //     return 1;

    return 1;
}

int main(void)
{
    // char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
    char *line;  // string to hold input line
    char **args; // array of strings to hold args

    int should_run = 1;

    do
    {
        printf("KShell:~$ ");
        fflush(stdout);

        /**
          * After reading user input, the steps are:
          * (1) fork a child process
          * (2) the child process will invoke execvp()
          * (3) if command includes &, parent and child will run concurrently
          */

        line = read_input();
        args = split_line(line);
        should_run = execute_args(args);
        printf("%d\n", should_run);
        // Debug line split -> args
        // for (int i = 0; args[i] != NULL; i++)
        // {
        //     printf("args[%2d]: %s\n", i, args[i]);
        // }

        free(line);
        free(args);

    } while (should_run);

    clean_exit(EXIT_SUCCESS);

    return 0;
}