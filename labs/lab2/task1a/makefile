all: addresses

addresses: addresses.o
	gcc -m32 -g -Wall -o addresses addresses.o
	rm addresses.o

addresses.o: addresses.c
	gcc -m32 -g -Wall -c -o addresses.o addresses.c

.PHONY: clean
clean:
	rm -f *.o addresses