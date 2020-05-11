all: task2b

task2b: task2b.o LineParser.o
	gcc -m32 -g -Wall -o task2b task2b.o LineParser.o
	rm *.o

task2b.o: task2b.c
	gcc -m32 -g -Wall -c -o task2b.o task2b.c

LineParser.o: ../../LineParser.c 
	gcc -m32 -g -Wall -c -o LineParser.o ../../LineParser.c
	
.PHONY: clean
clean:
	rm -f task2b