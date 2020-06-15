all: virus

virus: task2.s
	nasm -f elf -lvirus.lst task2.s -o task2.o
	ld -m elf_i386  task2.o -o virus
	rm task2.o
	
.PHONY: clean
clean:
	rm -f virus virus.lst
