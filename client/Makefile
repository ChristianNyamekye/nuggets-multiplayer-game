# Makefile for CS50 Nuggets grid module
#
# Bash Brigade
# Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
VAL = valgrind --leak-check=full --show-leak-kinds=all -s
LIB = ../support/support.a

.PHONY: all clean valgrind

all: client 

client: client.o $(LIB)
	$(CC) $(CFLAGS) $^ -lncurses -o $@

client.o: $(LIB)

clean:
	rm -f *.o
	rm -f vgcore*
	rm -f client