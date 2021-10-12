CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyush

nyush: nyush.o init.o cmd.o validator.o

nyush.o: nyush.c init.h cmd.h

init.o: init.c init.h validator.h

cmd.o: cmd.c cmd.h validator.h

validator.o: validator.c validator.h

.PHONY: clean
clean:
	rm -f *.o nyush
