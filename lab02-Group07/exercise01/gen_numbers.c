// Generate a fixed number of random numbers and print them on the stdout
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


int main(int argc, char** argv) {
    if ( argc < 2 ) {
        fprintf(stderr, "Usage: gen_numbers.out <# numbers to generate, must be > 0>\n\n");
        return -1;
    }

    int n_vals = strtol(argv[1], NULL, 10);

    if ( n_vals <= 0 ) {
        fprintf(stderr, "Invalid input %s\n\n", argv[1]);
        return -2;
    }

    for (int i = 0; i< n_vals; i++) {
        int randval = generate_random_int();
        printf("%d\n", randval);
    }

    return 0;
}