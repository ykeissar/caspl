all: b

b: b.o
	gcc -m32 -g -Wall -o b b.o
	rm b.o

b.o: b.c
	gcc -m32 -g -Wall -c -o b.o b.c

.PHONY: clean
clean:
	rm -f *.o b