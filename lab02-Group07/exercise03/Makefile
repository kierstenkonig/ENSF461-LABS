all: parsecsv.out

parsecsv.out: parsecsv.c record_list.c record_list.h util.c util.h
	gcc -g -o $@ $^ -lm

clean:
	rm -f parsecsv.out
