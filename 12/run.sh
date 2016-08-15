#!/bin/bash

name=${1%.*}
asm=$name.asm
obj=$name.o
./tiny < $1 > $asm && nasm -f elf64 $asm -o $obj && gcc $obj -o binario && ./binario
