#include "record_list.h"

record_t* append(record_t* tail, float avg, float sdv) {
    const char err_msg[] = "Error: unable to allocate memory!\n\n";

    record_t* new_elem = (record_t*)malloc(sizeof(record_t));
    
    if ( new_elem == NULL ) {
        write(2, err_msg, strlen(err_msg));
        exit(-3);
    }

    new_elem->avg = avg;
    new_elem->sdv = sdv;
    new_elem->next = NULL;

    if ( tail != NULL ) {
        tail->next = new_elem;
    }

    return new_elem;
}


record_t* next(record_t* elem) {
    // TODO: given an element of the list, return the next element
    return elem->next;
}
