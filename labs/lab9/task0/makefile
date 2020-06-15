all: myELF virus

myELF: task0a.c
	gcc -m32 -g -Wall -o myELF task0a.c

virus: task0b.s
	nasm -f elf task0b.s -o task0b.o
	ld -m elf_i386 task0b.o -o virus
	rm task0b.o

	
.PHONY: clean
clean:
	rm -f myELF virus