CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: shell

shell: shell.o init.o cmd.o io.o validator.o

shell.o: shell.c init.h cmd.h

init.o: init.c init.h validator.h

cmd.o: cmd.c cmd.h validator.h io.h

io.o: io.c io.h validator.h

validator.o: validator.c validator.h

.PHONY: clean
clean:
	rm -f *.o shell
