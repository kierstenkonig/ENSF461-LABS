#ifndef __RECORD_LIST_H
#define __RECORD_LIST_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct __record_t{
    float avg;
    float sdv;
    struct __record_t *next;
} record_t;


record_t* append(record_t* tail, float avg, float sdv);
record_t* next(record_t* elem);

#endif
