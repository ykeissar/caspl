all: task1b

task1b: task1b.o LineParser.o
	gcc -m32 -g -Wall -o task1b task1b.o LineParser.o
	rm *.o

task1b.o: task1b.c
	gcc -m32 -g -Wall -c -o task1b.o task1b.c

LineParser.o: ../../LineParser.c 
	gcc -m32 -g -Wall -c -o LineParser.o ../../LineParser.c
	
.PHONY: clean
clean:
	rm -f task1b