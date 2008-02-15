CC = gcc
CFLAGS += -g -Wall $(shell pkg-config --cflags cairo gtk+-2.0)
LIBS = $(shell pkg-config --libs cairo gtk+-2.0)

all: test

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: test.o
	$(CC) $(CFLAGS) -o $@ test.o $(LIBS)

.PHONY: clean
clean:
	$(RM) test *.o
