all: task1c

task1c: task1c.o LineParser.o
	gcc -m32 -g -Wall -o task1c task1c.o LineParser.o
	rm *.o

task1c.o: task1c.c
	gcc -m32 -g -Wall -c -o task1c.o task1c.c

LineParser.o: ../../LineParser.c 
	gcc -m32 -g -Wall -c -o LineParser.o ../../LineParser.c
	
.PHONY: clean
clean:
	rm -f task1c