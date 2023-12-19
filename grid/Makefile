# Makefile for CS50 Nuggets grid module
#
# Bash Brigade
# Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -c
OBJS = grid.o file.o 
LIB = grid.a
LIBS = -lm
LLIBS = ../grid/grid.a

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

grid.o: grid.h 
file.o: file.h

.PHONY: all clean

all: grid

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core