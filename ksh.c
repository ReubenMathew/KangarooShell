#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_TOKENS 40
#define MAX_TOKEN_SIZE 128
#define DELIMETERS " "
#define BACKGROUND 0;
#define FOREGROUND 1;

int mode = FOREGROUND;
char **history_args;

char *read_input(void)
{
    char *input = NULL;
    size_t len = 0;

    getline(&input, &len, stdin);

    input[strlen(input) - 1] = '\0';

    return input;
}

void ksh_exit(int status)
{
    free(history_args);
    fputs("Closing Shell... Goodbye\n", stdout);
    exit(status);
}

int ksh_cd(char *path)
{
    if (chdir(path) == -1)
    {
        return -1;
    }
    return 0;
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
void print_args_debug(char **args)
{
    for (int i = 0; args[i] != NULL; i++)
        printf("args[%2d]: %s\n", i, args[i]);
}

void print_history_args(char **args)
{
    for (int i = 0; args[i] != NULL; i++)
        printf("Running last command: %s \n", args[i]);
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
    // printf("Mode: %d\n", mode);
}

void update_history(char **args)
{
    // Add command arguments to history
    /*                              */
    if (history_args)
    {
        print_args_debug(history_args);
        print_args_debug(args);
    }
    else
    {
        // Initialize history_args
        history_args = malloc(MAX_TOKENS * sizeof(char *));
    }

    // history_args = args
    int i;
    for (i = 0; args[i] != NULL; i++)
    {
        char *new_arg = malloc(strlen(args[i]));
        memcpy(new_arg, args[i], strlen(args[i]));
        if (history_args[i] == NULL)
        {
            free(history_args[i]);
        }
        history_args[i] = malloc(strlen(args[i]));
        memcpy(history_args[i], new_arg, strlen(new_arg));
        free(new_arg);
    }
    history_args[i] = '\0';
}

int run_command(char **args)
{
    pid_t pid = fork();
    int status;
    int devNull, out2devNull, err2devNull;

    update_history(args);

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
            if (out2devNull == -1 || err2devNull == -1)
            {
                fputs("Error Reassigning: STDOUT/STDERR\n", stdout);
                exit(EXIT_FAILURE);
            }
        }

        // Change Directory
        if (!strcmp(args[0], "cd"))
        {
            // error handling for chdir()
            if (ksh_cd(args[1]) < 0)
                fputs("No such file or directory\n", stdout);

            return 1;
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
            printf("Background PID for [%s]: %d\n", args[0], pid);
        }
    }

    return 1;
}

int execute_args(char **args)
{
    // Empty newline
    if (args[0] == NULL)
        return 1;
    // Exit Shell
    if (!strcmp(args[0], "exit"))
        return 0;
    // Run last command with !!
    if (!strcmp(args[0], "!!"))
    {
        if (!history_args)
        {
            fputs("No commands in history.\n", stdout);
        }
        else
        {
            print_history_args(history_args);
            return run_command(history_args);
        }
    }

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
        fflush(stdin);
        fflush(stdout);

        line = read_input();
        backgroundCheck(line);
        args = split_line(line);
        should_run = execute_args(args);

        free(line);
        free(args);
        line = NULL;
        args = NULL;

    } while (should_run);

    ksh_exit(EXIT_SUCCESS);

    return 0;
}