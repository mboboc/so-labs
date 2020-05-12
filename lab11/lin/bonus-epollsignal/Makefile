CC = gcc
CPPFLAGS = -I../utils -Wno-unused-variable
CFLAGS = -Wall -Wextra -g

.PHONY: all clean

all: epollsignal

epollsignal: epollsignal.o

epollsignal.o: epollsignal.c ../utils/utils.h

clean:
	-rm -f *.o *~ epollsignal *.txt
