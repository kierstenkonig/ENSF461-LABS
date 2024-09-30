#include <stdio.h>
#include <string.h>
#include "record_list.h"
#include "util.h"


int main(int argc, char** argv) {

    char usage[] = "Usage: parsecsv.out <input CSV file> <output CSV file>\n\n";
    char foerr[] = "Error: unable to open/create file\n\n";

    if ( argc != 3 ) {
        fprintf(stderr, "Usage: parsecsv.out <input CSV file> <output CSV file>\n\n");
        return -1;
    }

    FILE* fin = fopen(argv[1], "r");
    if ( fin == NULL ) {
        fprintf(stderr, "Error: unable to open file %s\n\n", argv[1]);
        return -2;
    }

    int* newline = read_next_line(fin);
    record_t* head = NULL;
    record_t* curr = NULL;
    
    while ( newline != NULL ) {
        float avg = compute_average(newline);
        float sdv = compute_stdev(newline);
        curr = append(curr, avg, sdv);
        if ( head == NULL )
            head = curr;
        free(newline);
        newline = read_next_line(fin);
    }
    fclose(fin);

    // TODO: write the list to the output file
    // Each line of the output file should contain the average and the standard deviation
    // as a comma-separated pair (e.g., "1.23,4.56")
    FILE* fout = fopen(argv[2], "w");
    if(fout == NULL){
        fprintf(stderr, "Error is in the file: %s", foerr);
        return -3;
    }

    curr = head;
    while (curr != NULL){
        fprintf(fout, "%.2f,%.2f\n", curr->avg, curr->sdv);
        curr = curr->next;
    }
    fclose(fout);

    // TODO: free all the memory allocated for the list
    curr = head;
    record_t* temp = NULL;
    while(curr != NULL){
        temp = curr;
        curr = curr->next;
        free(temp);
    }
    return 0;
}

// command to compile: gcc -o parsecsv.out parsecsv.c record_list.c util.c
// command to run: ./parsecsv.out <input CSV file> <output CSV file>