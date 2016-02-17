#Thanks to http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

CC=gcc
CFLAGS=-I.
DEPS = $(shell ls *.h)
CDEPS = $(shell ls *.c)

OBJ = $(subst .c,.o,$(CDEPS))

BINDIR=/bin/
NAME=main

%.o: %.c $(DEPS)
		$(CC) -c -std=gnu99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual \
			-Wstrict-prototypes -Wmissing-prototypes -Werror -o $@ $< $(CFLAGS)

$(NAME): $(OBJ)
		$(CC) -std=gnu99 -pedantic -Wall -Wshadow -Wpointer-arith -Wcast-qual \
			-Wstrict-prototypes -Wmissing-prototypes -Werror -o $@ $^ $(CFLAGS)

clean:
		rm -f *.o
		rm $(NAME)
		clear
