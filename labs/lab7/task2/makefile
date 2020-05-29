all: hexeditplus

hexeditplus: task2.o
	gcc -m32 -g -Wall -o hexeditplus task2.o
	rm *.o

task2.o: task2.c
	gcc -m32 -g -Wall -c -o task2.o task2.c
	
.PHONY: clean
clean:
	rm -f hexeditplus