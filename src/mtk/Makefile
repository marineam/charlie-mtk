CC = gcc
CFLAGS += -g -Wall $(shell pkg-config --cflags cairo gtk+-2.0)
LIBS = $(shell pkg-config --libs cairo gtk+-2.0)
OBJS = window.o clickarea.o
HEAD = window.h clickarea.h

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
