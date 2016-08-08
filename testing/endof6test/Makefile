all: run

run: whiletest.o
	gcc -g -o  run whiletest.o

whiletest.o: whiletest.asm
	nasm -g -f elf64 whiletest.asm 

