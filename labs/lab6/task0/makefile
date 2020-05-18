all: mypipe

mypipe: mypipe.o
	gcc -m32 -g -Wall -o mypipe mypipe.o
	rm *.o

mypipe.o: mypipe.c
	gcc -m32 -g -Wall -c -o mypipe.o mypipe.c
	
.PHONY: clean
clean:
	rm -f mypipe