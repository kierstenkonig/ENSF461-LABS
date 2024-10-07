#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

typedef struct Variable {
    char *name;
    char *value;
    struct Variable *next;
} Variable;

Variable *var_list = NULL; // Global variable list

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
    if (strchr(*command, '/') != NULL) {
        // If the command contains a '/', treat it as a path
        return access(*command, X_OK) == 0;
    }
    
    char *path = getenv("PATH");
    char *path_copy = strdup(path);
    char *dir = strtok(path_copy, ":");
    char fullpath[1024];
    
    while (dir != NULL) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, *command);
        if (access(fullpath, X_OK) == 0) {
            *command = strdup(fullpath);
            free(path_copy);
            return TRUE;
        }
        dir = strtok(NULL, ":");
    }
    
    free(path_copy);
    return FALSE;
}

void update_variable(char* name, char* value) {
    //takes the 2 parameters and will search the var_list for the existing variable
    //note that memory leak is prevented by feeing up the old value

    Variable *var = var_list;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            free(var->value);
            var->value = strdup(value); //creates a new copy of the value and assigned to var
            return;
        }
        var = var->next;
    }
    // If not found, create new variable
    Variable *new_var = malloc(sizeof(Variable));
    new_var->name = strdup(name); //name and value copied and stored in new struct and added to var_list
    new_var->value = strdup(value);
    new_var->next = var_list;
    var_list = new_var;
}

char* lookup_variable(char* name) {
    Variable *var = var_list;
    //iterates through the var_list and compares each variable in the list
    //to the given name using the strcmp
    //NULL returned if no match found
    while (var) {
        if (strcmp(var->name, name) == 0) {
            return var->value;
        }
        var = var->next;
    }
    return NULL;
}

void execute_command(token_t** tokens, int numtokens) {
    int pipefd[2];
    int pipe_present = FALSE;
    int output_redirect = -1;
    int pid, pid2;
    
    char *command[64];
    int cmd_idx = 0;

    // Check for variable assignment
    if (numtokens > 2 && tokens[1]->type == TOKEN_ASSIGN) {
        char output[1024] = {0};
        int assignpipe[2];
        if (pipe(assignpipe) == -1) {
            perror("pipe");
            return;
        }
        
        pid = fork();
        if (pid == 0) {
            // Child process
            close(assignpipe[0]);
            dup2(assignpipe[1], STDOUT_FILENO);
            close(assignpipe[1]);
            
            // Execute the command after '='
            char *cmd[62];
            int i;
            for (i = 2; i < numtokens; i++) {
                cmd[i-2] = tokens[i]->value;
            }
            cmd[i-2] = NULL;
            execvp(cmd[0], cmd);
            perror("execvp");
            exit(1);
        } else {
            // Parent process
            close(assignpipe[1]);
            int nbytes = read(assignpipe[0], output, sizeof(output) - 1);
            if (nbytes > 0) {
                if (output[nbytes-1] == '\n') {
                    output[nbytes-1] = '\0';
                } else {
                    output[nbytes] = '\0';
                }
            }
            close(assignpipe[0]);
            wait(NULL);
        }
        
        update_variable(tokens[0]->value, output);
        return;
    }

    // Parse tokens and build the command array
    for (int i = 0; i < numtokens; i++) {
        if (tokens[i]->type == TOKEN_STRING) {
            command[cmd_idx++] = tokens[i]->value;
        } else if (tokens[i]->type == TOKEN_PIPE) {
            pipe_present = TRUE;
            command[cmd_idx] = NULL; // Null-terminate left command

            if (pipe(pipefd) < 0) {
                perror("Pipe error");
                return;
            }
            
            pid = fork();
            if (pid == 0) {
                // Child process for left side of the pipe
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
                execvp(command[0], command);
                perror("exec failed");
                exit(1);
            }
            
            // Reset for right side of pipe
            cmd_idx = 0;
        } else if (tokens[i]->type == TOKEN_REDIR) {
            output_redirect = open(tokens[++i]->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_redirect < 0) {
                perror("Output redirection failed");
                return;
            }
        }
    }
    command[cmd_idx] = NULL; // Null-terminate the command array

    // Execute the command (or right side of pipe)
    pid2 = fork();
    if (pid2 == 0) {
        // Child process
        if (pipe_present) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
        }
        if (output_redirect != -1) {
            dup2(output_redirect, STDOUT_FILENO);
            close(output_redirect);
        }
        execvp(command[0], command);
        perror("exec failed");
        exit(1);
    }

    // Parent process
    if (pipe_present) {
        close(pipefd[0]);
        close(pipefd[1]);
    }
    if (output_redirect != -1) {
        close(output_redirect);
    }
    
    // Wait for all child processes
    while (wait(NULL) > 0);
}

void expand_variables(token_t **tokens, int numtokens) {
    //will first iterate through all tokens and only expand the 
    //variables in string tokens or variables
    for (int i = 0; i < numtokens; i++) {
        if (tokens[i]->type == TOKEN_STRING || tokens[i]->type == TOKEN_VAR) {
            //space allocated before the while loop
            char *expanded = malloc(1024);  // Allocate space for expanded string
            expanded[0] = '\0';
            char *start = tokens[i]->value;
            char *dollar;
            while ((dollar = strchr(start, '$')) != NULL) {
                //looks continously for '$' in token
                strncat(expanded, start, dollar - start);
                // move start to just after the $
                start = dollar + 1;

                char var_name[256]; //extracts variable name
                int j = 0;
                while (isalnum(start[j]) || start[j] == '_') {
                    var_name[j] = start[j];
                    j++;
                }
                var_name[j] = '\0';
                char *var_value = lookup_variable(var_name); //loopup_variable will look up the value of the variable
                if (var_value) {
                    //if variable exists, it's added to the expanded string
                    strcat(expanded, var_value);
                }
                start += j; //moves the start to after the variable name
            }
            //Adds remaining part of the orig string to after the last variable
            strcat(expanded, start);
            //Then replaces the orig token value with the version that's expanded
            free(tokens[i]->value);
            tokens[i]->value = expanded;
            tokens[i]->type = TOKEN_STRING;
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
    //frees list
    var_list = NULL;
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

    while (1) {
        readlen = read_line(infile, buffer, 1024);
        if (readlen < 0) {
            perror("Error reading input file");
            return -3;
        }
        if (readlen == 0) {
            break;
        }

        int numtokens = 0;
        token_t** tokens = tokenize(buffer, readlen, &numtokens);
        assert(numtokens > 0);

        // Expand variables
        expand_variables(tokens, numtokens);

        // Normalize the command (only for non-assignment commands)
        if (!(numtokens > 2 && tokens[1]->type == TOKEN_ASSIGN)) {
            if (!normalize_executable(&tokens[0]->value)) {
                fprintf(stderr, "Command not found: %s\n", tokens[0]->value);
                continue;
            }
        }

        // Execute the command
        execute_command(tokens, numtokens);

        // Free tokens vector
        for (int ii = 0; ii < numtokens; ii++) {
            free(tokens[ii]->value);
            free(tokens[ii]);
        }
        free(tokens);
    }

    close(infile);
    free_variables();

    return 0;
}