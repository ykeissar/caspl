all: virus

virus: task3.s
	nasm -f elf -lvirus.lst task3.s -o task3.o
	ld -m elf_i386 task3.o -o virus
	rm task3.o
	
.PHONY: clean
clean:
	rm -f virus virus.lst
