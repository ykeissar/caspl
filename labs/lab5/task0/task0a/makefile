all: task0a

task0a: task0a.o LineParser.o
	gcc -m32 -g -Wall -o task0a task0a.o LineParser.o
	rm *.o

task0a.o: task0a.c
	gcc -m32 -g -Wall -c -o task0a.o task0a.c

LineParser.o: ../../LineParser.c 
	gcc -m32 -g -Wall -c -o LineParser.o ../../LineParser.c
	
.PHONY: clean
clean:
	rm -f *.o task0a