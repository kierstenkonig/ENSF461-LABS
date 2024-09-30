#include <stdlib.h>
#include <time.h>
#include "utils.h"

int initialized = FALSE;

void init_randomness()
{
    if ( initialized == FALSE ) {
        srand(time(NULL));
        initialized = TRUE;
    }
    return;
}

int generate_random_int() {
    init_randomness();
    return rand();
}

int generate_int_below_max(int max) {
    init_randomness();
    return rand() % max;
}
