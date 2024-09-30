.PHONY: all
all: engine.out

engine.out: engine.c parser.c
	gcc -Wall -g -o $@ $^

.PHONY: clean
clean: 
	rm -f *.out