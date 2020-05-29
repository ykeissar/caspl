all: hexeditplus

hexeditplus: task0b.o
	gcc -m32 -g -Wall -o hexeditplus task0b.o
	rm *.o

task0b.o: task0b.c
	gcc -m32 -g -Wall -c -o task0b.o task0b.c
	
.PHONY: clean
clean:
	rm -f hexeditplus