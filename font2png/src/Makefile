SRCDIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
TARGET  = $(SRCDIR)font2png

OBJS = $(patsubst %.c, %.o, $(wildcard $(SRCDIR)*.c))
HEADERS = $(wildcard *.h)

LIBS    = -L$(SRCDIR)../lib -lpng -lz -lm

CC      = gcc
CFLAGS  = -Wl,-q -Wall -g #-O3
ASFLAGS = $(CFLAGS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET) $(OBJS)