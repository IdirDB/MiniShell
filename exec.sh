#!/bin/bash

# Compilation
gcc -Wall -c parsing.c -o parsing.o
gcc -Wall -c execution.c -o execution.o
gcc -Wall -c main.c -o main.o
gcc -Wall -c hashTable.c -o hashTable.o
gcc -Wall -c builtIn.c -o builtIn.o
gcc -Wall parsing.o execution.o hashTable.o builtIn.o main.o -o my_shell
