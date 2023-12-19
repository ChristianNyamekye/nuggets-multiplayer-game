# Makefile for CS50 Nuggets server module
#
# Bash Brigade
# Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
OBJS = server.o
LIBS = -lm
LLIBS = ../grid/grid.a ../support/support.a

PROG = server

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

server: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) $(LLIBS) -o server

server.o: ../support/support.a server.h ../grid/grid.a server.c 

test:
	bash -v testing.sh >& testing.out
	

.PHONY: test valgrind clean

valgrind: $(PROG)
	valgrind ./$(PROG) 2>server.log ../maps/small.txt 13

valgrindFull: $(PROG)
	valgrind --leak-check=full ./$(PROG) 2>server.log ../maps/small.txt

valgrindAll: $(PROG)
	valgrind --show-leak-kinds=all ./%(PROG) 2>server.log ../maps/small.txt


clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	# rm -f core