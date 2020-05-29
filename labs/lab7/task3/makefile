all: hexeditplus

hexeditplus: task3.o
	gcc -m32 -g -Wall -o hexeditplus task3.o
	rm *.o

task3.o: task3.c
	gcc -m32 -g -Wall -c -o task3.o task3.c
	
.PHONY: clean
clean:
	rm -f hexeditplus
