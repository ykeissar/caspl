all: task2a

task2a: task2a.o LineParser.o
	gcc -m32 -g -Wall -o task2a task2a.o LineParser.o
	rm *.o

task2a.o: task2a.c
	gcc -m32 -g -Wall -c -o task2a.o task2a.c

LineParser.o: ../../LineParser.c 
	gcc -m32 -g -Wall -c -o LineParser.o ../../LineParser.c
	
.PHONY: clean
clean:
	rm -f task2a