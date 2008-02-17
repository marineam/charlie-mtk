CC = gcc
MAKEFLAGS = --no-print-directory
CFLAGS += -g -Wall -D_GNU_SOURCE -std=gnu99
CFLAGS += -I./include -I./libmpdclient
CFLAGS += $(shell pkg-config --cflags cairo gtk+-2.0)
LIBS = $(shell pkg-config --libs cairo gtk+-2.0)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
HEAD = $(wildcard include/*.h)

all: charlie

%.o: %.c $(HEAD)
	@echo " CC $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

mtk/mtk.a: $(HEAD)
	@$(MAKE) -C mtk mtk.a

libmpdclient/libmpdclient.o:
	@$(MAKE) -C libmpdclient libmpdclient.o

charlie: $(OBJS) mtk/mtk.a libmpdclient/libmpdclient.o
	@echo " CC $@"
	@$(CC) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	@$(MAKE) -C mtk clean
	@$(MAKE) -C libmpdclient clean
	$(RM) charlie *.o
