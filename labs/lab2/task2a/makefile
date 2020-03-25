all: a

a: a.o
	gcc -m32 -g -Wall -o a a.o
	rm a.o

a.o: a.c
	gcc -m32 -g -Wall -c -o a.o a.c

.PHONY: clean
clean:
	rm -f *.o a