# Exercise 01 Lab 02

Welcome to exercise 01 of lab02. This folder contains various source file:

* `gen_numbers.c`: generates random numbers. Must be compiled to `gen_numbers.out` executable
* `select_arg.c`: selects and prints a random command line argument. Must be compiled to `select_arg.out` executable
* `gen_in_range.c`: generates a random number between given [min, max) values. Must be compiled to `gen_in_range.out` executable
* `utils.c`: various utility functions for generating random numbers
* `utils.h`: header file for `utils.c`

## Further requirements

* Define `all` as default Makefile target that builds all other targes (e.g., running `make` should run the `all target`, and in turn build everything else. Also, define `clean` as target to remove all generated executable files. Finally, use a variable to store the compiler command (by default `gcc`), in such a way that changing the value of that variable will change the compiler used throughout the Makefile **(0.5 pts)**
* Use, where possible, wildcards and/or target placeholders (`$@`, `$<`, `$^`) rather than manually writing down prerequisites and targets **(0.5 pts)**
* Test if make is being run on Linux before executing the `all`` target. If the OS is not Linux, print "Sorry, I prefer Linux" and do nothing. If the OS is Linux, execute the targets as normal **(1 pt)**
* Define a “test” target which compiles each executable, runs it with valid command line parameters, and checks that the return value of each executable is 0 (anything is fine as long as some kind of error is returned if this condition is not met) **(1 pt)**

## A note on correctness

In order to receive full points, the Makefile rule for a given target should include *only* the necessary prerequisites: nothing less, nothing more
