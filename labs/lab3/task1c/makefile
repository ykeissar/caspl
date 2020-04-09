all: main

main: main.o
	gcc -m32 -g -Wall -o main main.o
	rm main.o

main.o: main.c
	gcc -m32 -g -Wall -c -o main.o main.c

.PHONY: clean
clean:
	rm -f *.o main