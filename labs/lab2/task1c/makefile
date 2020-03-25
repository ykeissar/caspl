all: arrays

arrays: arrays.o
	gcc -m32 -g -Wall -o arrays arrays.o
	rm arrays.o

arrays.o: arrays.c
	gcc -m32 -g -Wall -c -o arrays.o arrays.c

.PHONY: clean
clean:
	rm -f *.o arrays