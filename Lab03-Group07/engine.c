#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include "parser.h"

#define TRUE 1
#define FALSE 0

int read_line(int infile, char *buffer, int maxlen)
{
    int readlen = 0;

    // TODO: Read a single line from file; retains final '\n'
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
    // Convert command to absolute path if needed (e.g., "ls" -> "/bin/ls")
    // Returns TRUE if command was found, FALSE otherwise

    if (command[0][0] == '/') { //it's an absolute path
        return TRUE;  
    }

    if (strchr(command[0], '/') != NULL) {   //it's a relative path
        return TRUE;    
    }
    char *path = getenv("PATH");  //get the PATH variable   
    char *path_copy = strdup(path);  //copy the PATH variable for manipulating
    char *directory = strtok(path_copy, ":"); //get the first directory

    while (directory != NULL) {     //search for the command in each directory
        char *full_path = malloc(strlen(directory) + strlen(command[0]) + 2); //allocate memory for the full path
        sprintf(full_path, "%s/%s", directory, command[0]);   //create the full path
        if (access(full_path, X_OK) == 0) {  //check if the file is executable
            command[0] = full_path; //update the command    
            free(path_copy);    //free the memory
            return TRUE;    
        }
        directory = strtok(NULL, ":");    
    }
    
    free(path_copy);    //free the memory
    return FALSE;      
}


void update_variable(char* name, char* value) {
    // Update or create a variable
}

char* lookup_variable(char* name) {
    // Lookup a variable
    return NULL;
}

void execute_command(char** command, int numtokens) {
    pid_t pid = fork(); // Fork a new process

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) { // Child process
        // Debugging log
        printf("Executing command: ");
        for (int i = 0; i < numtokens; i++) {
            printf("%s ", command[i]);
        }
        printf("\n");

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


int main(int argc, char *argv[])
{
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

        // Tokenize the line
        int numtokens = 0;
        token_t** tokens = tokenize(buffer, readlen, &numtokens);
        assert(numtokens > 0);

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

        char *command[numtokens + 1];
        for (int ii = 0; ii < numtokens; ii++) {
            command[ii] = tokens[ii]->value;
        }
        command[numtokens] = NULL;

        // Execute the command
        execute_command(command, numtokens);
        normalize_executable(command);

        // Free tokens vector
        for (int ii = 0; ii < numtokens; ii++) {
            free(tokens[ii]->value);
            free(tokens[ii]);
        }
        free(tokens);
    }

    close(infile);
    
    // Remember to deallocate anything left which was allocated dynamically
    // (i.e., using malloc, realloc, strdup, etc.)

    return 0;
}