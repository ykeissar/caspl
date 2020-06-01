all: hexeditplus

hexeditplus: task0.o
	gcc -m32 -g -Wall -o hexeditplus task0.o
	rm *.o

task0.o: task0.c
	gcc -m32 -g -Wall -c -o task0.o task0.c
	
.PHONY: clean
clean:
	rm -f hexeditplus