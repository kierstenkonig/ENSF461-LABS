#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

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
    if (access(*command, F_OK) == 0) {
        return TRUE;
    }
    // Try common paths (e.g., /bin, /usr/bin)
    char *paths[] = {"/bin/", "/usr/bin/", NULL};
    char fullpath[1024];
    for (int i = 0; paths[i] != NULL; i++) {
        snprintf(fullpath, sizeof(fullpath), "%s%s", paths[i], *command);
        if (access(fullpath, F_OK) == 0) {
            *command = strdup(fullpath);
            return TRUE;
        }
    }

    return FALSE;
}

void update_variable(char* name, char* value) {
    // Update or create a variable
    setenv(name, value, 1); // Set environment variable
}

char* lookup_variable(char* name) {
    // Lookup a variable
    if(getenv(name)){
        return getenv(name);
    }
    else{
        return NULL;
    }
}

void execute_command(token_t** tokens, int numtokens) {
    int pipefd[2];
    int pipe_present = FALSE;
    int pid;
    
    char *command[64];
    int cmd_idx = 0;
    int output_redirect = -1;

    // Parse tokens and build the command array
    for (int i = 0; i < numtokens; i++) {
        if (tokens[i]->type == TOKEN_STRING) {
            command[cmd_idx++] = tokens[i]->value;  // Add command string to command array
        } else if (tokens[i]->type == TOKEN_PIPE) {
            pipe_present = TRUE;
            command[cmd_idx] = NULL; // Null-terminate left command

            if (pipe(pipefd) < 0) {
                perror("Pipe error");
                return;
            }
            if ((pid = fork()) == 0) {
                // Child process for left side of the pipe
                dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
                close(pipefd[0]); // Close unused read end of pipe
                execvp(command[0], command); // Execute the left side command
                perror("exec failed");
                _exit(1);
            } else {
                // Parent process
                close(pipefd[1]); // Close write end of pipe
                cmd_idx = 0; // Reset command index for right-side command
                i++; // Move to the next token for the right command
                while (i < numtokens) {
                    if (tokens[i]->type == TOKEN_STRING) {
                        command[cmd_idx++] = tokens[i]->value; // Add right command to command array
                    } else if (tokens[i]->type == TOKEN_REDIR) {
                        // Handle output redirection
                        output_redirect = open(tokens[++i]->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (output_redirect < 0) {
                            perror("Output redirection failed");
                            return;
                        }
                    }
                    i++;
                }
                command[cmd_idx] = NULL; // Null-terminate right command
                // Execute the right side of the pipe
                if ((pid = fork()) == 0) {
                    dup2(pipefd[0], STDIN_FILENO); // Redirect stdin from pipe
                    close(pipefd[0]); // Close read end of pipe
                    if (output_redirect != -1) {
                        dup2(output_redirect, STDOUT_FILENO); // Redirect stdout to file if needed
                        close(output_redirect);
                    }
                    execvp(command[0], command); // Execute the right side command
                    perror("exec failed");
                    _exit(1);
                }
                // Wait for both child processes to finish
                close(pipefd[0]); // Close read end of pipe in parent
                wait(NULL); // Wait for left child
                wait(NULL); // Wait for right child
                return; // Exit after processing pipe command
            }
        } else if (tokens[i]->type == TOKEN_REDIR) {
            // Output redirection detected
            output_redirect = open(tokens[++i]->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_redirect < 0) {
                perror("Output redirection failed");
                return;
            }
        }
    }

    // Handle normal command execution if no pipe is present
    if (!pipe_present) {
        command[cmd_idx] = NULL; // Null-terminate the command array

        if ((pid = fork()) == 0) {
            // Child process
            if (output_redirect != -1) {
                dup2(output_redirect, STDOUT_FILENO);
                close(output_redirect);
            }
            execvp(command[0], command); // Execute the command
            perror("exec failed");
            _exit(1);
        }
        // Wait for child to finish
        wait(NULL);
    }
}

typedef struct Variable {
    char *name;
    char *value;
    struct Variable *next;
} Variable;

Variable *var_list = NULL; // Global variable list


void expand_variables(token_t **tokens, int numtokens) {
    for (int i = 0; i < numtokens; i++) {
        if (tokens[i]->type == TOKEN_VAR) {
            char *var_name = tokens[i]->value + 1; // Skip the '$'
            char *var_value = lookup_variable(var_name);
            if (var_value) {
                free(tokens[i]->value);
                tokens[i]->value = strdup(var_value);
                tokens[i]->type = TOKEN_STRING; // Change type to string
            } else {
                // If variable not found, consider handling it (e.g., set to empty)
                free(tokens[i]->value);
                tokens[i]->value = strdup(""); // Set to empty if not found
                tokens[i]->type = TOKEN_STRING;
            }
        }
    }
}


void free_variables() {
    Variable *current = var_list;
    while (current) {
        Variable *temp = current;
        current = current->next;
        free(temp->name);
        free(temp->value);
        free(temp);
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

        int numtokens = 0;
        token_t** tokens = tokenize(buffer, readlen, &numtokens);
        assert(numtokens > 0);

       if (numtokens > 2 && tokens[1]->type == TOKEN_ASSIGN) {
        update_variable(tokens[0]->value, tokens[2]->value);
        } else {
            // Expand variables before executing the command
            expand_variables(tokens, numtokens);
            
            // Normalize the command
            if (!normalize_executable(&tokens[0]->value)) {
                fprintf(stderr, "Command not found: %s\n", tokens[0]->value);
                continue;
            }

            // Execute the command
            execute_command(tokens, numtokens);
        }
        // Run commands
        // * Fork and execute commands
        // * Handle pipes
        // * Handle redirections
        // * Handle pipes
        // * Handle variable assignments
        // TODO

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
    free_variables();

    return 0;
}