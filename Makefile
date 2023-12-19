# Makefile for CS50 Nuggets module
#
# Bash Brigade
# Authors: Caleb Ash, Emi Rohn, Natasha Kapadia, Christian Nyamekye

L = support
G = grid
.PHONY: all clean

############## default: make all libs and programs ##########
# If libcs50 contains set.c, we build a fresh libcs50.a;
# otherwise we use the pre-built library provided by instructor.
all: 
	(cd $L && make $L.a;)
	(cd $G && make $G.a;)
	make -C client
	make -C server

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C support clean
	make -C client clean
	make -C server clean
	make -C grid clean