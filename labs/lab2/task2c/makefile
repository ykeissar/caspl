all: c

c: c.o
	gcc -m32 -g -Wall -o c c.o
	rm c.o

c.o: c.c
	gcc -m32 -g -Wall -c -o c.o c.c

.PHONY: clean
clean:
	rm -f *.o c