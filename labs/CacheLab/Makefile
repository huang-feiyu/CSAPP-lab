#
# Student makefile for Cache Lab
# Note: requires a 64-bit x86-64 system
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64

all: csim test-trans tracegen
	# Generate a handin tar file each time you compile

csim: csim.c cachelab.c cachelab.h
	$(CC) $(CFLAGS) -o csim csim.c cachelab.c -lm

test-csim: csim.c cachelab.c cachelab.h
	rm csim
	$(CC) $(CFLAGS) -o csim csim.c cachelab.c -lm
	clear && ./test-csim

test-trans: test-trans.c trans.o cachelab.c cachelab.h
	$(CC) $(CFLAGS) -O0 -o tracegen tracegen.c trans.o cachelab.c
	$(CC) $(CFLAGS) -o test-trans test-trans.c cachelab.c trans.o
	clear && ./test-trans -M 61 -N 67

tracegen: tracegen.c trans.o cachelab.c
	$(CC) $(CFLAGS) -O0 -o tracegen tracegen.c trans.o cachelab.c

trans.o: trans.c
	$(CC) $(CFLAGS) -O0 -c trans.c

test: clean all
	clear && python2 driver.py

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f *.tar
	rm -f csim
	rm -f test-trans tracegen
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
