#ifndef __UTIL_H
#define __UTIL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int* read_next_line(FILE* fin);
float compute_average(int* line);
float compute_stdev(int* line);

#endif
