#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

// Define a DEBUG macro
#define DEBUG 0
#if DEBUG
#define debug_printf(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)
#else
#define debug_printf(fmt, ...) \
            do {} while (0)
#endif

// The base structure of the command given by the user (using a struct)
struct ShellCommand
{
    char* command; // the command given by user to be executed
    int redirect; // a value to hold what type of redirection was requested (1 = output with overwrite, 2 = output with append)
    char* outputFile; // if requested get an output file for redirection
    char* inputFile; // if requested get an input file for redirection
    char* arguments[1024]; // all given arguments including the command
};

void removeChar(char *str, char* c);
//// Functions to implement:
// Display current working directory and return user input
char* CommandPrompt();
// Process the user input (As a shell command)
// Note the return type is a ShellCommand struct
struct ShellCommand ParseCommandLine(char* input);
// handle redirection
void redirection(struct ShellCommand command);
// Execute a shell command
// Note the parameter is a ShellCommand struct
void ExecuteCommand(struct ShellCommand command);

int main() // MAIN
{
    char* input;
    struct ShellCommand command;
    // repeatedly prompt the user for input
    for (;;)
    {
        // get the user's input
        input = CommandPrompt();
        // parse the command line
        command = ParseCommandLine(input);
        // execute the command
        ExecuteCommand(command);
        // debug
        debug_printf("%s\n", command.command);
        for (int i=0; i < 5; i++)
        {
            debug_printf("%s\n", command.arguments[i]);
        }
    }
    exit(0);
}

// Display current working directory and return user input

char* CommandPrompt()
{
    char* userInput = malloc(1024);
    char cwd[1024];
    
    // prints current working directory by using the getcwd() function and then prints it with the $ if it exists if not then return error
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s$ ", cwd);
        // using fgets we take the standard input of the user and set userInput to that value/string
        fgets(userInput, 1024, stdin);
        // Remove trailing newline character from userInput
        userInput[strcspn(userInput, "\n")] = '\0';
    }
    else // error case if there is no working directory
    {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    return userInput;
}

struct ShellCommand ParseCommandLine(char* input)
{
    struct ShellCommand command;
    command.inputFile=NULL;
    command.outputFile=NULL;

    char* token;
    int i = 1;
    int in_quote = 0;

    token = strtok(input, " ");
    command.command = token;
    command.arguments[0] = token;

    if (strcmp(command.command, "exit") == 0)
    {
        exit(0);
    }

    while (token != NULL)
    {
        token = strtok(NULL, in_quote ? "\"" : " \"");
        
        if (token != NULL && strcmp(token, "\"") == 0)
        {
            in_quote = !in_quote;
            continue;
        }

        if (!in_quote)
        {
            command.arguments[i] = token;
            
            if (token != NULL && strcmp(token, "<") == 0)
            {
                token = strtok(NULL, " ");
                command.inputFile = token;
                debug_printf("Input File: %s\n", command.inputFile);
            }
            else if (token != NULL && strcmp(token, ">") == 0)
            {
                token = strtok(NULL, " ");
                command.outputFile = token;
                command.redirect = 1;
                debug_printf("Output File: %s\n", command.outputFile);
            }
            else if (token != NULL && strcmp(token, ">>") == 0)
            {
                token = strtok(NULL, " ");
                command.outputFile = token;
                command.redirect = 2;
                debug_printf("Output File: %s\n", command.outputFile);
            }
            else
            {
                debug_printf("Token: %s\n", token);
                i++;
            }
        }
    }
    command.arguments[i] = NULL;

    return command;
}

void ExecuteCommand(struct ShellCommand command)
{
    // Handle command cd for Change Directory
    if (strcmp(command.command, "cd") == 0) // if the command is cd then call the changeDirectory function
        {
            if (command.arguments[1] == NULL) // if no argument provided with cd
            {
                fprintf(stderr, "Usage: cd <directory>\n");
            }
            
            char cdResult[1024] = "";
            for (int i = 1; command.arguments[i] != NULL; i++) // for each argument (not the command) concatenate them into a string
            {
                strcat(cdResult, command.arguments[i]);
                if (command.arguments[i + 1] != NULL)
                {
                    strcat(cdResult, " ");
                }
            }

            if (strcmp(cdResult, "..") == 0) // if the argument is ".." then go back a directory
            {
                chdir("..");
            }
            else
            {
                if (chdir(cdResult) != 0) {
                    perror("chdir failed");
                }
            }
            return; // no need to proceed further if it's a cd command
        }

    pid_t pid = fork(); 

    if (pid < 0) // Handle a failed fork
    {
        perror("Failed to create child");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) // using exec calls execute the userInput command (child process)
    {
        redirection(command); // calls the redirection function to handle any requests for such
        if(execvp(command.command, command.arguments) == -1) // execvp to exectue the command
        {
            perror("execvp failed");
        } // execvp to exectue the command
        exit(EXIT_SUCCESS);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
}


// Extra Functions
// A function that removes desired characters (used to remove <, >, ,, and "")
void removeChar(char *str, char* c)
{
    int i, j;
    int len = strlen(str);
    for(i = j = 0; i < len; i++)
    {
        if (str[i] != *c)
        {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}


void redirection(struct ShellCommand command)
{
    if (command.inputFile)
    {
        if(freopen(command.inputFile, "r", stdin) == NULL)
        {
            perror("Input file failed to open");
            exit(EXIT_FAILURE);
        }
    }
    if (command.outputFile)
    {
        if (command.redirect == 1)
        {
            if(freopen(command.outputFile, "w", stdout) == NULL)
            {
                perror("Output file failed to open");
                exit(EXIT_FAILURE);
            }
        }
        else if (command.redirect == 2)
        {
            if(freopen(command.outputFile, "a", stdout) == NULL)
            {
                perror("Output file failed to open");
                exit(EXIT_FAILURE);
            }
        }
    }
}
