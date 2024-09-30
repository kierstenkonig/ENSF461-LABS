// Select a random command line argument and prints it out
#include <stdio.h>
#include "utils.h"


int main(int argc, char** argv)
{
    if ( argc < 2 ) {
        fprintf(stderr, "Usage: select_arg.out <list of arguments>\n\n");
        return -1;
    }

    int index = generate_int_below_max(argc);
    printf("%s\n", argv[index]);

    return 0;
}