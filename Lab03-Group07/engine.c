#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define TRUE 1
#define FALSE 0
#define MAX_COMMANDS 128
#define MAX_TOKENS 128

int read_line(int infile, char *buffer, int maxlen)
{
    int readlen = 0;
    char ch;
    while (readlen < maxlen - 1) {
        int result = read(infile, &ch, 1);
        if (result == 0) { // End of file
            break;
        } else if (result < 0) { // Read error
            return -1;
        }
        buffer[readlen++] = ch;
        if (ch == '\n') {
            break;
        }
    }
    buffer[readlen] = '\0'; // Null-terminate the string

    return readlen;
}

int normalize_executable(char **command) {
    if (command[0][0] == '/') { // Absolute path
        return TRUE;  
    }

    if (strchr(command[0], '/') != NULL) {   // Relative path
        return TRUE;    
    }
    
    char *path = getenv("PATH");  // Get the PATH variable   
    char *path_copy = strdup(path);  // Copy the PATH variable for manipulating
    char *directory = strtok(path_copy, ":"); // Get the first directory

    while (directory != NULL) {     // Search for the command in each directory
        char *full_path = malloc(strlen(directory) + strlen(command[0]) + 2); // Allocate memory for the full path
        sprintf(full_path, "%s/%s", directory, command[0]);   // Create the full path
        if (access(full_path, X_OK) == 0) {  // Check if the file is executable
            command[0] = full_path; // Update the command    
            free(path_copy);    // Free the memory
            return TRUE;    
        }
        directory = strtok(NULL, ":");    
    }
    
    free(path_copy);    // Free the memory
    return FALSE;      
}

void update_variable(char* name, char* value) {
    // Update or create a variable
}

char* lookup_variable(char* name) {
    // Lookup a variable
    return NULL;
}

void execute_command(char **command, int numtokens) {
    pid_t pid = fork(); // Fork a new process
    int fd_in = -1, fd_out = -1;

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) { // Child process
        // Debugging log
        for (int i = 0; i < numtokens; i++) {
            if (strcmp(command[i], ">") == 0) {
                fd_out = open(command[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    perror("open");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
                command[i] = NULL;
                break;
            } else if (strcmp(command[i], ">>") == 0) {
                fd_out = open(command[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd_out < 0) {
                    perror("open");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
                command[i] = NULL;
                break;
            } else if (strcmp(command[i], "<") == 0) {
                fd_in = open(command[i+1], O_RDONLY);
                if (fd_in < 0) {
                    perror("open");
                    exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
                command[i] = NULL;
                break;
            }
        }

        // Normalize command path
        normalize_executable(command);

        // Execute the command
        execvp(command[0], command);
        perror("execvp");
        exit(1);
    } else { // Parent process
        // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
    }

    return;
}

void execute_piped_commands(char ***commands, int num_commands) {
    int pipefds[2 * (num_commands - 1)];

    // Create pipes for all but the last command
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe");
            exit(1);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid == 0) {  // Child process
            // Set up input redirection from the previous pipe
            if (i > 0) {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO);
            }
            // Set up output redirection to the next pipe
            if (i < num_commands - 1) {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO);
            }

            // Close all pipes in the child
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefds[j]);
            }

            // Execute the command
            normalize_executable(commands[i]);
            execvp(commands[i][0], commands[i]);
            perror("execvp");
            exit(1);
        }
    }

    // Close all pipes in the parent
    for (int i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipefds[i]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_commands; i++) {
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <input file>\n", argv[0]);
        return -1;
    }

    int infile = open(argv[1], O_RDONLY);
    if(infile < 0) {
        perror("Error opening input file");
        return -2;
    }

    char buffer[1024];
    int readlen;

    // Read file line by line
    while( 1 ) {

        // Load the next line
        readlen = read_line(infile, buffer, 1024);
        if(readlen < 0) {
            perror("Error reading input file");
            return -3;
        }
        if(readlen == 0) {
            break;
        }

        char *pipe_commands[MAX_COMMANDS];
        // Tokenize the line
        int num_commands = 0;
        char *cmd = strtok(buffer, "|");

        // Parse token list
        // * Organize tokens into command parameters
        // * Check if command is a variable assignment
        // * Check if command has a redirection
        // * Expand variables if any
        // * Normalize executables
        // * Check if pipes are present

        // * Check if pipes are present
        // TODO

        // Run commands
        // * Fork and execute commands
        // * Handle pipes
        // * Handle redirections
        // * Handle pipes
        // * Handle variable assignments
        // TODO
        while (cmd != NULL) {
            pipe_commands[num_commands++] = strdup(cmd);
            cmd = strtok(NULL, "|");
        }

        // If there are pipes, handle them
        if (num_commands > 1) {
            char **commands[MAX_COMMANDS];
            for (int i = 0; i < num_commands; i++) {
                int numtokens;
                token_t **tokens = tokenize(pipe_commands[i], strlen(pipe_commands[i]), &numtokens);
                commands[i] = malloc((numtokens + 1) * sizeof(char *));
                for (int j = 0; j < numtokens; j++) {
                    commands[i][j] = tokens[j]->value;
                }
                commands[i][numtokens] = NULL;
            }
            execute_piped_commands(commands, num_commands);
        } else {
            // Handle a single command without pipes
            int numtokens;
            token_t **tokens = tokenize(buffer, readlen, &numtokens);
            char *command[numtokens + 1];
            for (int i = 0; i < numtokens; i++) {
                command[i] = tokens[i]->value;
            }
            command[numtokens] = NULL;

            execute_command(command, numtokens);
        }

        // Free allocated memory for pipe commands
        for (int i = 0; i < num_commands; i++) {
            free(pipe_commands[i]);
        }
    }

    close(infile);

    // Remember to deallocate anything left which was allocated dynamically
    // (i.e., using malloc, realloc, strdup, etc.)

    return 0;
}
