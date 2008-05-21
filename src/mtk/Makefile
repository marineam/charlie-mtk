CC = gcc
CFLAGS += -g -Wall -D_GNU_SOURCE -std=gnu99 -fms-extensions
CFLAGS += -I../include -I../libmpdclient
CFLAGS += $(shell pkg-config --cflags cairo xcb xcb-aux)
LIBS = $(shell pkg-config --libs cairo xcb xcb-aux) -lrt
OBJS = main.o object.o event.o widget.o container.o window.o util.o mpdlist.o timer.o text.o image.o hpack.o vpack.o menu.o
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
