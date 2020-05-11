all: task2c

task2c: task2c.o LineParser.o
	gcc -m32 -g -Wall -o task2c task2c.o LineParser.o
	rm *.o

task2c.o: task2c.c
	gcc -m32 -g -Wall -c -o task2c.o task2c.c

LineParser.o: ../../LineParser.c 
	gcc -m32 -g -Wall -c -o LineParser.o ../../LineParser.c
	
.PHONY: clean
clean:
	rm -f task2c