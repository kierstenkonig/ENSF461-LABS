#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    
    if ( argc < 2 ) {
        fprintf(stderr, "Usage: test.out <loop iterations>\n\n");
        return -1;
    }

    int i = atoi(argv[1]);
    while ( i < 10 ) {
        printf("%d\n", i);
        i++;
    }

    return 0;
}
