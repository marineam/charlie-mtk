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

.PHONY: mtk
mtk:
	@$(MAKE) -s -C mtk mtk.a

.PHONY: libmpdclient
libmpdclient:
	@$(MAKE) -s -C libmpdclient libmpdclient.o

charlie: $(OBJS) mtk libmpdclient
	@echo " CC $@"
	@$(CC) -o $@ $(OBJS) mtk/mtk.a libmpdclient/libmpdclient.o  $(LIBS)

.PHONY: clean
clean:
	@$(MAKE) -C mtk clean
	@$(MAKE) -C libmpdclient clean
	$(RM) charlie *.o
