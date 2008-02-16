CC = gcc
CFLAGS += -g -Wall $(shell pkg-config --cflags cairo gtk+-2.0)
LIBS = $(shell pkg-config --libs cairo gtk+-2.0)
OBJS = window.o clickarea.o
HEAD = window.h clickarea.h

all: test

%.o: %.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ $<

test: test.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ test.o $(OBJS) $(LIBS)

.PHONY: clean
clean:
	$(RM) test *.o
