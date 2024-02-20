# TechShell
## Russell Kelly
## Description
Implementation of a shell using the C programming language. It includes the major functions of a shell where it prints the working directory, parses user input/command line, and executes the given command line with the given arguments.

## Features
- Execute standard UNIX commands
- I/O redirection (< and >)
- Displays and repeatedly prompts user after command and its execution
- Parse Command Line: Provides a way to parse input that allows redirection and command execution with arguments.

## Installation
Clone the repository and compile source code using GCC
```
git clone [(https://github.com/ViperKelly/TechShell.git)]
cd techshell
gcc -o techshell techshell.c
```
Alternative is to download the repository as a zip and do the last two commands as seen.

## Usage
Run the shell through terminal
```
./techshell
```

## Examples
- ls: Listing. includes arguments `-l, -lh, etc`
- Echo: allows for printing/writing of strings
- Redirect: Output of commands correctly create or append to a file and gives/prints the output. Input to commands is allowed ex. 'wc -l < ps.out'
- Most basic UNIX commands
See typescript
## Issures/Limitations
All known problems
- As of now does not support piping or AND for multi command usage ('||', '&&')
- Simple error handles. More in depth errors are not expected to be shown.

#BreakDown
## 1.) Main
- Simply a driver that unless told to exit repeatedly runs the created functions to print working directory, prompt user input of command line, and execute command.
- This is not in main but is a main function. Create a struct for the ShellCommand. (this includes giving command a command, arguments, redirect, input/output files.)
## 2.) Command Prompt
- Takes the current working directory detected by the unistd.h library with getcwd
- Using stdio and fgets take in a user input from the command line. Returns this input.
## 3.) Command Line Parsing
- This function is a struct creation of the ShellCommand
- Take in the user input from Command Prompt
- Using the string library the tokenize function strtok() went through the user input and tokenized it at each space.
- This tokenized string got put into the arguments array and also the first token was put as the command.
- Each token was compared using strcmp to check for quotes to start a in quote function that would put all things inside quotes as one token.
- Each token was also compared for redirection calls (<, >, >>) and took the next token as the file and set it to the corresponding variable from the struct. An integere value 1 or 2 was given for each input redirection to check for append or overwrite
- the created command struct was returned
## 4.) Command Execution
- Takes in the command object created in parsing
- checks to see if the command gathered is cd and handles change directory. if '..' is the argument move back a directory. For any other changing of directory gather all the arguments that aren't the command and combine them (makes sure to get any directories with spaces) then change directory.
- For other command executions fork the program
- In the child process call the redirection function and then execute the command with its arguments
## 5.) Redirection
- Redirection program takes in the command object and checks to see if input or output files exist. Using freopen the files will be open and then read or written to.
- If these files exist then for input redirection open the file with read and read that to standard input.
- For output redirection check the redirect interger for if it is an append or overwrite call and then open the file with w for overwrite or a for append. This also creates a file if it doesn't exist yet.
## Extras/Handling
- A remove character function was created that loops through a string (token in our case) to get rid of any undesired characters. This is used to guarantee that quotes and the redirection symbols are excluded from outputs
- For Error Handling very basic print error statements were used to print that an error occured. The print statement tells where that error happened.
## Contributing
Any commented feedback is welcome.
Requests for code changes will be thoroughly looked at and tested.

## License
Public Free License

## Contact
rdk018@email.latech.edu
(504) 654 7575
