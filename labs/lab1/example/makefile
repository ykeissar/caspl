all: run
run: main.o numbers.o add.o
	gcc -m32 -g -Wall -o run main.o numbers.o add.o
main.o: main.c
	gcc -m32 -g -Wall -c -o main.o main.c
numbers.o: numbers.c
	gcc -m32 -g -Wall -c -o numbers.o numbers.c
add.o: add.s
	nasm -g -f macho -w+all -o add.o add.s
.PHONY: clean
clean: 
	rm -f *.o run
