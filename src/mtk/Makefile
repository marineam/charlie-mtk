CC = gcc
CFLAGS += -g -Wall -D_GNU_SOURCE -std=gnu99
CFLAGS += -I../include -I../libmpdclient
CFLAGS += $(shell pkg-config --cflags cairo xcb xcb-aux)
LIBS = $(shell pkg-config --libs cairo xcb xcb-aux)
OBJS = main.o window.o util.o clickarea.o
HEAD = $(wildcard ../include/*.h) private.h

all: mtk.a test

%.o: %.c $(HEAD)
	@echo " CC $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

mtk.a: $(OBJS)
	@echo " AR $@"
	@$(AR) -rc $@ $^

test: test.c mtk.a
	@echo " CC $@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	$(RM) test mtk.a *.o
