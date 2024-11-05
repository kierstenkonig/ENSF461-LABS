.PHONY: all
all: mathserver.out

mathserver.out: mathserver.c
	gcc -g -Wall -o $@ $^ -pthread

clean:
	rm -f mathserver.out
