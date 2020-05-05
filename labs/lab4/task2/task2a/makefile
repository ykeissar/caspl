all: task2a

task2a: start.o main.o util.o
	ld -m elf_i386 start.o main.o util.o -o task2a
	rm *.o

util.o: util.c
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o

main.o: main.c
	gcc -m32 -Wall -g -ansi -c -nostdlib -fno-stack-protector main.c -o main.o

start.o: start.s
	nasm -f elf32 -g start.s -o start.o

.PHONY: clean
clean:
	rm -f *.o task2a debug_log