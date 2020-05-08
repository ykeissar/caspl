all: calc

calc: calc.o
	gcc -m32 -Wall -g calc.o -o calc
	rm *.o

calc.o: calc.s
	nasm -f elf -lcalclist.lst calc.s -o calc.o
	
.PHONY: clean
clean:
	rm -f calc