#Sample Makefile for Malloc
CC=gcc
CFLAGS=-g -O0 -fPIC

all: clean check

clean:
	rm -rf libmalloc.so malloc.o functions.o free.o realloc.o reallocarray.o memalign.o posix_memalign.o calloc.o test1 test1.o test1-1 test1-1.o t-test1 t-test1.o

libmalloc.so: functions.o malloc.o free.o realloc.o calloc.o reallocarray.o memalign.o posix_memalign.o
	$(CC) $(CFLAGS) -shared -Wl,--unresolved-symbols=ignore-all functions.o malloc.o free.o realloc.o reallocarray.o memalign.o posix_memalign.o calloc.o -o $@ -lpthread

test1: test1.o
	$(CC) $(CFLAGS) $< -o $@ -lpthread

test1-1: test1-1.o
	$(CC) $(CFLAGS) $< -o $@ -lpthread
t-test1: t-test1.o
	$(CC) $(CFLAGS) $< -o $@ -lpthread

# For every XYZ.c file, generate XYZ.o.
%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@  -lpthread

check:	libmalloc.so test1 test1-1 t-test1
	LD_PRELOAD=`pwd`/libmalloc.so ./test1
	LD_PRELOAD=`pwd`/libmalloc.so ./t-test1
	LD_PRELOAD=`pwd`/libmalloc.so ./test1-1
	#./test1-1

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar

