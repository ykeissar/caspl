all: hexeditplus

hexeditplus: task4.o
	gcc -m32 -g -Wall -o hexeditplus task4.o
	rm *.o

task4.o: task4.c
	gcc -m32 -g -Wall -c -o task4.o task4.c
	
.PHONY: clean
clean:
	rm -f hexeditplus
