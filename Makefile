CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyush

nyush: nyush.o init.o

nyush.o: nyush.c init.h

init.o: init.c init.h

.PHONY: clean
clean:
	rm -f *.o nyush
