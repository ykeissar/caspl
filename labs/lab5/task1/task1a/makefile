all: task1a

task1a: task1a.o LineParser.o
	gcc -m32 -g -Wall -o task1a task1a.o LineParser.o
	rm *.o

task1a.o: task1a.c
	gcc -m32 -g -Wall -c -o task1a.o task1a.c

LineParser.o: ../../LineParser.c 
	gcc -m32 -g -Wall -c -o LineParser.o ../../LineParser.c
	
.PHONY: clean
clean:
	rm -f task1a