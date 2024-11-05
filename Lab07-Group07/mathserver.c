#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>


#define NUM_CONTEXTS 16

// Define structure for context
typedef struct {
    int value;  // The value for this context
} Context;

// Global array of contexts
Context contexts[NUM_CONTEXTS];

// Function to initialize contexts
void initialize_contexts() {
    for (int i = 0; i < NUM_CONTEXTS; i++) {
        contexts[i].value = 0;
    }
}

// Function to handle 'set' operation
void set_context(FILE* output_file, int ctx_id, int val) {
    if (ctx_id < 0 || ctx_id >= NUM_CONTEXTS) {
        fprintf(output_file, "Error: Invalid context ID %d\n", ctx_id);
        return;
    }
    contexts[ctx_id].value = val;
    fprintf(output_file, "ctx %02d: set to value %d\n", ctx_id, val);}

// Function to handle 'add' operation
void add_context(FILE* output_file, int ctx_id, int val) {
    if (ctx_id < 0 || ctx_id >= NUM_CONTEXTS) {
        fprintf(output_file, "Error: Invalid context ID %d\n", ctx_id);
        return;
    }
    contexts[ctx_id].value += val;  // Perform addition
    fprintf(output_file, "ctx %02d: add %d (result: %d)\n", ctx_id, val, contexts[ctx_id].value);
}

// Function to handle 'sub' operation
void sub_context(FILE* output_file, int ctx_id, int val) {
    if (ctx_id < 0 || ctx_id >= NUM_CONTEXTS) {
        fprintf(output_file, "Error: Invalid context ID %d\n", ctx_id);
        return;
    }
    contexts[ctx_id].value -= val;  // Perform subtraction
    fprintf(output_file, "ctx %02d: sub %d (result: %d)\n", ctx_id, val, contexts[ctx_id].value);
}

// Function to handle 'mul' operation
void mul_context(FILE* output_file, int ctx_id, int val) {
    if (ctx_id < 0 || ctx_id >= NUM_CONTEXTS) {
        fprintf(output_file, "Error: Invalid context ID %d\n", ctx_id);
        return;
    }
    contexts[ctx_id].value *= val;  // Perform multiplication
    fprintf(output_file, "ctx %02d: mul %d (result: %d)\n", ctx_id, val, contexts[ctx_id].value);  // Log the operation
}

// Function to handle 'div' operation
void div_context(FILE* output_file, int ctx_id, int val) {
    if (ctx_id < 0 || ctx_id >= NUM_CONTEXTS) {
        fprintf(output_file, "Error: Invalid context ID %d\n", ctx_id);
        return;
    }
    if (val == 0) {
        fprintf(output_file, "Error: Division by zero in context %d\n", ctx_id);
        return;
    }
    contexts[ctx_id].value /= val;  // Perform division
    fprintf(output_file, "ctx %02d: div %d (result: %d)\n", ctx_id, val, contexts[ctx_id].value);  // Log the operation
}

// Function to handle 'pri' operation
void pri_context(FILE* output_file, int ctx_id) {
    if (ctx_id < 0 || ctx_id >= NUM_CONTEXTS) {
        fprintf(output_file, "Error: Invalid context ID %d\n", ctx_id);
        return;
    }

    int limit = contexts[ctx_id].value;
    if (limit < 2) {
        fprintf(output_file, "ctx %02d: pri (result: none)\n", ctx_id);
        return;
    }

    // Sieve of Eratosthenes to find all primes up to 'limit'
    bool is_prime[limit + 1];
    for (int i = 0; i <= limit; i++) is_prime[i] = true;
    is_prime[0] = is_prime[1] = false;

    for (int i = 2; i * i <= limit; i++) {
        if (is_prime[i]) {
            for (int j = i * i; j <= limit; j += i) {
                is_prime[j] = false;
            }
        }
    }

    // Collect and log prime numbers
    fprintf(output_file, "ctx %02d: primes (result:", ctx_id);
    bool first = true;
    for (int i = 2; i <= limit; i++) {
        if (is_prime[i]) {
            if (!first) fprintf(output_file, ",");
            fprintf(output_file, " %d", i);
            first = false;
        }
    }
    fprintf(output_file, ")\n");
}

// Function to handle 'pia' operation
void pia_context(FILE* output_file, int ctx_id) {
    if (ctx_id < 0 || ctx_id >= NUM_CONTEXTS) {
        fprintf(output_file, "Error: Invalid context ID %d\n", ctx_id);
        return;
    }

    int iterations = contexts[ctx_id].value;
    if (iterations < 1) {
        fprintf(output_file, "Error: Invalid number of iterations %d for pi approximation\n", iterations);
        return;
    }

    double pi_approx = 0.0;
    for (int k = 0; k < iterations; k++) {
        pi_approx += (k % 2 == 0 ? 1.0 : -1.0) / (2 * k + 1);
    }
    pi_approx *= 4.0;

    // Log the result with 15 significant digits
    fprintf(output_file, "ctx %02d: pia (result %.15f)\n", ctx_id, pi_approx);
}

// Recursive function to compute Fibonacci number
int fib_r(int n) {
    if (n <= 0) return 0;
    else if (n == 1) return 1;
    else return fib_r(n - 1) + fib_r(n - 2);
}

// Function to handle 'fib' operation
void fib_context(FILE* output_file, int ctx_id) {
    if (ctx_id < 0 || ctx_id >= NUM_CONTEXTS) {
        fprintf(output_file, "Error: Invalid context ID %d\n", ctx_id);
        return;
    }

    int n = contexts[ctx_id].value;
    int fib_value = fib_r(n);  // Compute the Fibonacci number using the recursive function

    // Log the result
    fprintf(output_file, "ctx %02d: fib (result: %d)\n", ctx_id, fib_value);
}

// Function to parse and execute commands
void parse_and_execute(FILE* output_file, char* command) {
    char operation[4];  // To store the operation name (e.g., "set", "add", "sub")
    int ctx_id, val;

    // Updated parsing: remove the "ctx" prefix requirement
    if (sscanf(command, "%3s %d %d", operation, &ctx_id, &val) == 3) {
        if (strcmp(operation, "set") == 0) {
            set_context(output_file, ctx_id, val);
        } else if (strcmp(operation, "add") == 0) {
            add_context(output_file, ctx_id, val);
        } else if (strcmp(operation, "sub") == 0) {
            sub_context(output_file, ctx_id, val);
        } else if (strcmp(operation, "mul") == 0) {
            mul_context(output_file, ctx_id, val);
        } else if (strcmp(operation, "div") == 0) {
            div_context(output_file, ctx_id, val);
        } else {
            fprintf(output_file, "Error: Unsupported operation %s\n", operation);
        }
    } else if (sscanf(command, "%3s %d", operation, &ctx_id) == 2) {
        if (strcmp(operation, "pri") == 0) {
            pri_context(output_file, ctx_id);
        } else if (strcmp(operation, "pia") == 0) {
            pia_context(output_file, ctx_id);
        } else if (strcmp(operation, "fib") == 0) {
            fib_context(output_file, ctx_id);
        } else {
            fprintf(output_file, "Error: Unsupported operation %s\n", operation);
        }
    } else {
        fprintf(output_file, "Error: Invalid command format\n");
    }
}

// Main function
int main(int argc, char* argv[]) {
    const char usage[] = "Usage: mathserver.out <input trace> <output log>\n";
    char* input_trace;
    char* output_log;
    char buffer[1024];

    // Parse command line arguments
    if (argc != 3) {
        printf("%s", usage);
        return 1;
    }
    
    input_trace = argv[1];
    output_log = argv[2];

    // Open input file
    FILE* input_file = fopen(input_trace, "r");
    if (!input_file) {
        perror("Error opening input file");
        return 1;
    }

    // Open output log file
    FILE* output_file = fopen(output_log, "w");
    if (!output_file) {
        perror("Error opening output log file");
        fclose(input_file);
        return 1;
    }

    // Initialize contexts
    initialize_contexts();

    // Read input file line by line
    while (!feof(input_file)) {
        char* rez = fgets(buffer, sizeof(buffer), input_file);
        if (!rez) {
            break;
        }
        
        // Remove newline character, if present
        buffer[strcspn(buffer, "\n")] = '\0';

        // Parse and execute the command
        parse_and_execute(output_file, buffer);
    }

    // Close files
    fclose(input_file);
    fclose(output_file);

    return 0;
}
