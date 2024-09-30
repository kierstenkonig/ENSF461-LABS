.PHONY: all
all: test.out

%.out: %.c
	gcc -g -o $@ $<

.PHONY: clean
clean:
	rm -f *.out
	