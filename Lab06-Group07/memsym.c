#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

// Output file
FILE* output_file;

// TLB replacement strategy (FIFO or LRU)
char* strategy;
int current_pid = 0;
int define_called = FALSE;
uint32_t registers[4][2] = {{0}};  // Registers r1 and r2 for each process

// Structure for physical memory
uint32_t *physical_memory;
int offset_bits, pfn_bits, vpn_bits;

// Initialize the memory simulator
void define(int off, int pfn, int vpn) {
    if (define_called) {
        fprintf(output_file, "Current PID: %d. Error: multiple calls to define in the same trace\n", current_pid);
        exit(1);
    }
    
    offset_bits = off;
    pfn_bits = pfn;
    vpn_bits = vpn;

    int num_frames = 1 << (offset_bits + pfn_bits);
    physical_memory = calloc(num_frames, sizeof(uint32_t));

    fprintf(output_file, "Current PID: %d. Memory instantiation complete. OFF bits: %d. PFN bits: %d. VPN bits: %d\n", current_pid, offset_bits, pfn_bits, vpn_bits);
    define_called = TRUE;
}

// Handle context switching
void ctxswitch(int pid) {
    if (pid < 0 || pid > 3) {
        fprintf(output_file, "Current PID: %d. Invalid context switch to process %d\n", current_pid, pid);
        exit(1);
    }
    current_pid = pid;
    fprintf(output_file, "Current PID: %d. Switched execution context to process: %d\n", current_pid, pid);
}


// Load value into register
void load(char* reg, char* src) {
    int reg_index;
    uint32_t value;

    // Determine which register to load into
    if (strcmp(reg, "r1") == 0) {
        reg_index = 0;
    } else if (strcmp(reg, "r2") == 0) {
        reg_index = 1;
    } else {
        fprintf(output_file, "Current PID: %d. Error: invalid register operand %s\n", current_pid, reg);
        exit(1);
    }

    // Load immediate value
    if (src[0] == '#') {
        value = (uint32_t)atoi(src + 1);  // Skip '#' prefix for immediate value
        registers[current_pid][reg_index] = value;
        fprintf(output_file, "Current PID: %d. Loaded immediate %u into register %s\n", current_pid, value, reg);
    } else {
        fprintf(output_file, "Current PID: %d. Error: invalid source operand %s\n", current_pid, src);
        exit(1);
    }
}

// Add values in registers
void add() {
    uint32_t r1_value = registers[current_pid][0];
    uint32_t r2_value = registers[current_pid][1];
    uint32_t result = r1_value + r2_value;

    registers[current_pid][0] = result;
    fprintf(output_file, "Current PID: %d. Added contents of registers r1 (%u) and r2 (%u). Result: %u\n", current_pid, r1_value, r2_value, result);
}

// Tokenize input for parsing
char** tokenize_input(char* input) {
    char** tokens = NULL;
    char* token = strtok(input, " ");
    int num_tokens = 0;

    while (token != NULL) {
        num_tokens++;
        tokens = realloc(tokens, num_tokens * sizeof(char*));
        tokens[num_tokens - 1] = malloc(strlen(token) + 1);
        strcpy(tokens[num_tokens - 1], token);
        token = strtok(NULL, " ");
    }

    num_tokens++;
    tokens = realloc(tokens, num_tokens * sizeof(char*));
    tokens[num_tokens - 1] = NULL;

    return tokens;
}

// Map a virtual page number to a physical frame number
void map(int vpn, int pfn) {
    fprintf(output_file, "Current PID: %d. Mapped virtual page number %d to physical frame number %d\n", current_pid, vpn, pfn);
}

// Unmap a virtual page number
void unmap(int vpn) {
    fprintf(output_file, "Current PID: %d. Unmapped virtual page number %d\n", current_pid, vpn);
}



int main(int argc, char* argv[]) {
    const char usage[] = "Usage: memsym.out <strategy> <input trace> <output trace>\n";
    char* input_trace;
    char* output_trace;
    char buffer[1024];

    // Parse command line arguments
    if (argc != 4) {
        printf("%s", usage);
        return 1;
    }
    strategy = argv[1];
    input_trace = argv[2];
    output_trace = argv[3];

    // Open input and output files
    FILE* input_file = fopen(input_trace, "r");
    output_file = fopen(output_trace, "w");  

    while (!feof(input_file)) {
        // Read input file line by line
        char *rez = fgets(buffer, sizeof(buffer), input_file);
        if (!rez) {
            fprintf(stderr, "Reached end of trace. Exiting...\n");
            break;
        }

        // Ignore comment lines
        if (buffer[0] == '%') {
            continue;
        }

        // Remove newline character at the end of the line
        buffer[strlen(buffer) - 1] = '\0';
        
        char** tokens = tokenize_input(buffer);

        // Check the first token to determine the command
        if (strcmp(tokens[0], "define") == 0) {
            int off = atoi(tokens[1]);
            int pfn = atoi(tokens[2]);
            int vpn = atoi(tokens[3]);
            define(off, pfn, vpn);
        } else {
            if (!define_called) {
                fprintf(output_file, "Current PID: %d. Error: attempt to execute instruction before define\n", current_pid);
                exit(1);
            }
            
            // Handle other commands
            if (strcmp(tokens[0], "ctxswitch") == 0) {
                int pid = atoi(tokens[1]);
                ctxswitch(pid);
            } else if (strcmp(tokens[0], "load") == 0) {
                load(tokens[1], tokens[2]);
            } else if (strcmp(tokens[0], "add") == 0) {
                add();
            } else if (strcmp(tokens[0], "map") == 0) {
                int vpn = atoi(tokens[1]);
                int pfn = atoi(tokens[2]);
                map(vpn, pfn);
            } else if (strcmp(tokens[0], "unmap") == 0) {
                int vpn = atoi(tokens[1]);
                unmap(vpn);
            } else {
                fprintf(output_file, "Current PID: %d. Error: unknown command %s\n", current_pid, tokens[0]);
                exit(1);
            }
        }

        // Deallocate tokens
        for (int i = 0; tokens[i] != NULL; i++)
            free(tokens[i]);
        free(tokens);
    }

    // Close input and output files
    fclose(input_file);
    fclose(output_file);

    // Free physical memory
    free(physical_memory);

    return 0;
}
