CC = gcc
CFLAGS += -g -Wall
OBJS = libmpdclient.o
HEAD = libmpdclient.h

all: libmpdclient.o example

%.o: %.c $(HEAD)
	@echo " CC $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

example: example.c $(OBJS)
	@echo " CC $@"
	@$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	$(RM) example *.o
