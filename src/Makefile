CC = gcc
MAKEFLAGS = --no-print-directory
CFLAGS += -g -Wall -D_GNU_SOURCE -std=gnu99
CFLAGS += $(shell pkg-config --cflags cairo gtk+-2.0)
LIBS = $(shell pkg-config --libs cairo gtk+-2.0)
OBJS = main.o mpd.o
HEAD = config.h mpd.h util.h

all: main

%.o: %.c $(HEAD)
	@echo " CC $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

mtk/mtk.a:
	@$(MAKE) -C mtk mtk.a

libmpdclient/libmpdclient.o:
	@$(MAKE) -C libmpdclient libmpdclient.o

main: $(OBJS) mtk/mtk.a libmpdclient/libmpdclient.o
	@echo " CC $@"
	@$(CC) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	@$(MAKE) -C mtk clean
	@$(MAKE) -C libmpdclient clean
	$(RM) main *.o
