
CC=gcc
CFLAGS=-Wall -pedantic -m64 -std=c99 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_POSIX_PTHREAD_SEMANTICS

all: lastlog

lastlog: Makefile lastlog.c
	$(CC) -o lastlog $(CFLAGS) lastlog.c

clean:
	rm -f core lastlog

