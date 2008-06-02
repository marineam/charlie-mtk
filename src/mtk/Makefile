CC = gcc
CFLAGS += -g -Wall -D_GNU_SOURCE -std=gnu99 -fms-extensions
CFLAGS += -I../include -I../libmpdclient
CFLAGS += $(shell pkg-config --cflags cairo x11 openssl)
LIBS = $(shell pkg-config --libs cairo x11 openssl) -lrt
SRCS = $(filter-out test.c, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
HEAD = $(wildcard ../include/*.h) private.h

all: mtk.a test

%.o: %.c $(HEAD)
	@echo " CC mtk/$@"
	@$(CC) $(CFLAGS) -c -o $@ $<

mtk.a: $(OBJS)
	@echo " AR mtk/$@"
	@$(AR) -rc $@ $^

test: test.c mtk.a
	@echo " CC mtk/$@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	$(RM) test mtk.a *.o
