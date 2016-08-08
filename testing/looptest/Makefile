all: run

run: looptest.o
	gcc -o run looptest.o

looptest.o: looptest.asm
	nasm -f elf64 looptest.asm 

