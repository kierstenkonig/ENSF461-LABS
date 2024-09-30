# Exercise 03 Lab 02

Welcome to exercise 03 of lab02. This folder contains various source file:

* `parsecsv.c`: main program; reads a CSV file, computes mean and standard deviation of each line, and write them to file
* `record_list.c`: implement the list data structure used to process the input file
* `record_list.h`: header file for `record_list.c`
* `util.c`: utility functions for reading from file and computing mean and standard deviation
* `util.h`: header file for `util.c`
* `Makefile`: a Makefile for building the `parsecsv.out` executable
* `numbers.csv`: a sample CSV input file
* `reference_output.csv`: the expected output of the program when run on `numbers.csv`

Your task is to implement 6 different missing pieces of code in the files above. Each missing piece is marked with a `TODO` comment. There are two TODOs in parsecsv.c, one in record_list.c, and three in util.c. You can find the missing pieces by searching for the `TODO` string in the files. Each missing piece is worth 0.5 points.

## Further requirements

* You are not allowed to change the function signatures of the functions you are asked to implement.
* You can add additional functions if you need them.
* The program must be run as `parsecsv.out <input file> <output file>`
* You can assume that the input CSV file is well-formed and contains only numbers.
