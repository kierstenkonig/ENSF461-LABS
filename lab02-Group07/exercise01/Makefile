
CC = gcc
EXECUTABLES = gen_in_range.out gen_numbers.out select_arg.out
SOURCES = gen_in_range.c gen_numbers.c select_arg.c
HEADERS = utils.h

.PHONY: all clean test

uname_s := $(shell uname -s)
ifeq ($(uname_s), Linux)
all: $(EXECUTABLES)
else 
all:
	@echo Sorry, I prefer Linux
endif

gen_in_range.out: gen_in_range.c 
	$(CC) -o $@ $<

gen_numbers.out: gen_numbers.c utils.c $(HEADERS)
	$(CC) -o $@ $^

select_arg.out: select_arg.c utils.c $(HEADERS)
	$(CC) -o $@ $^

clean:
	rm -f $(EXECUTABLES)

test: $(EXECUTABLES)
	./gen_in_range.out 10 20
	./gen_numbers.out 5
	./select_arg.out arg1 arg2 arg3


