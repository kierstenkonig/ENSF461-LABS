#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
    if ( argc != 3 ) {
        fprintf(stderr, "Usage: gen_in_range.out <min number> <max number>\n\n");
        return -1;
    }

    int min = atoi(argv[1]);
    int max = atoi(argv[2]);
    if ( min >= max ) {
        fprintf(stderr, "Error: <min number> must be strictly less than <max number>\n\n");
        return -2;
    }

    srand(time(NULL));
    printf("%d\n", rand() % (max - min) + min);

    return 0;
}

// command to compile: gcc -o gen_in_range.out gen_in_range.c
// command to run: ./gen_in_range.out <min number> <max number>