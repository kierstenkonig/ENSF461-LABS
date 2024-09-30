#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* read_next_line(FILE* fin) {
    // TODO: This function reads the next line from the input file
    // The line is a comma-separated list of integers
    // Return the list of integers as an array where the first element
    // is the number of integers in the rest of the array
    // Return NULL if there are no more lines to read

    char line[150];
    
    if (fgets(line, sizeof(line), fin) == NULL) {
        return NULL; 
    }

    int count = 1;
    for (char* p = line; *p != '\0'; p++) {
        if (*p == ',') count++;
    }

    int* array = (int*)malloc((count + 1) * sizeof(int));
    if (array == NULL) {
        return NULL;
    }
    
    array[0] = count;

    char* token = strtok(line, ",");
    for (int i = 1; token != NULL; i++) {
        array[i] = atoi(token);
        token = strtok(NULL, ",");
    }

    return array;
}

float compute_average(int* line) {
    // TODO: Compute the average of the integers in the vector
    int count = line[0];
    int sum = 0;

    for(int i = 1; i <= count; i++){
        sum += line[i];
    }

    return (float)sum/count;
}


float compute_stdev(int* line) {
    // TODO: Compute the standard deviation of the integers in the vector
    int count = line[0];
    float mean = compute_average(line);
    float variance_sum = 0.0;

    for(int i = 1; i<= count; i++){
        float diff = line[i] - mean; 
        variance_sum += diff * diff;
    }

    float variance = variance_sum/count;
    return sqrt(variance);
}