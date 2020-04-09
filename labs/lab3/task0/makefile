all: bubblesort

bubblesort: bubblesort.o
	gcc -m32 -g -Wall -o bubblesort bubblesort.o
	rm bubblesort.o

bubblesort.o: bubblesort.c
	gcc -m32 -g -Wall -c -o bubblesort.o bubblesort.c

.PHONY: clean
clean:
	rm -f *.o bubblesort