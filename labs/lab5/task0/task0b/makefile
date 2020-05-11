all: task0b

task0b: task0b.o LineParser.o
	gcc -m32 -g -Wall -o task0b task0b.o LineParser.o
	rm *.o

task0b.o: task0b.c
	gcc -m32 -g -Wall -c -o task0b.o task0b.c

LineParser.o: ../../LineParser.c 
	gcc -m32 -g -Wall -c -o LineParser.o ../../LineParser.c
	
.PHONY: clean
clean:
	rm -f task0b