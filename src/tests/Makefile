CC = gcc
MAKEFLAGS = --no-print-directory
CFLAGS += -g -Wall -D_GNU_SOURCE -std=gnu99
CFLAGS += -I../include -I../libmpdclient
CFLAGS += $(shell pkg-config --cflags cairo xcb xcb-aux)
LIBS = $(shell pkg-config --libs cairo xcb xcb-aux) -pthread -lrt
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%,$(SRCS))
TESTS = $(SRCS:.c=_run)
HEAD = $(wildcard include/*.h)
MTK = ../mtk/mtk.a ../libmpdclient/libmpdclient.o

all: $(TESTS)

$(OBJS): %: %.c $(HEAD) $(MTK) 
	@echo " CC $@"
	@$(CC) $(CFLAGS) -o $@ $< $(MTK) $(LIBS)

.PHONY: $(TESTS)
$(TESTS): %_run: %
	@if ./$< &>/dev/null; then echo " $<: PASS"; else ./$<; fi

.PHONY: clean
clean:
	$(RM) $(OBJS)
