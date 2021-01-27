#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_TOKEN_SIZE 128
#define DELIMETERS " "
#define BACKGROUND 0;
#define FOREGROUND 1;

int mode = FOREGROUND;

char *read_input(void)
{
    char *input = NULL;
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
    char *input_copy = malloc(sizeof(input));

    strcpy(input_copy, input);
    // printf("Original: %s\tCopy: %s\n", input, input_copy);

    char *token;

    int idx = 0;

    token = strtok(input_copy, DELIMETERS);

    while (token != NULL)
    {
        // printf("Token %d:%s\n",idx,token);
        tokens[idx++] = token;
        token = strtok(NULL, DELIMETERS);
    }

    tokens[idx] = NULL;

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
    // printf("Last Char: %c\n", cmd_line[strlen(cmd_line) - 1]);
    if (!strlen(cmd_line))
        return;

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
    int status;
    int devNull, out2devNull, err2devNull;

    if (pid == 0)
    {
        // Send output to /dev/null for background processes
        if (!mode)
        {
            devNull = open("/dev/null", O_WRONLY);
            if (devNull == -1)
            {
                fputs("Error Opening: /dev/null\n", stdout);
                exit(EXIT_FAILURE);
            }
            out2devNull = dup2(devNull, STDOUT_FILENO);
            err2devNull = dup2(devNull, STDERR_FILENO);
            if (out2devNull == -1 || err2devNull == -1){
                fputs("Error Reassigning: STDOUT/STDERR\n",stdout);
                exit(EXIT_FAILURE);
            }
            
        }
        // Execute command with arguments
        if (execvp(args[0], args) < 1)
            return 1;
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
            while (wait(&status) != pid)
                ;
        }
        else
        {
            printf("[%d]: %s\n", pid, args[0]);
        }
    }

    return 1;
}

int execute_args(char **args)
{
    if (args[0] == NULL)
        return 1;
    if (!strcmp(args[0], "exit"))
        return 0;

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
        backgroundCheck(line);
        args = split_line(line);
        should_run = execute_args(args);

        free(line);
        free(args);
        line = NULL;
        args = NULL;

    } while (should_run);

    clean_exit(EXIT_SUCCESS);

    return 0;
}