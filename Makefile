
CC=gcc
CFLAGS=-Wall -pedantic

all: lastlog

lastlog: Makefile lastlog.c
	$(CC) -o lastlog $(CFLAGS) lastlog.c

clean:
	rm -f core lastlog

