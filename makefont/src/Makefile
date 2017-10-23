SRCDIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
TARGET  = $(SRCDIR)makefont

OBJS = $(patsubst %.c, %.o, $(wildcard $(SRCDIR)*.c))
HEADERS = $(wildcard *.h)

LIBS    = -L$(SRCDIR)../../common/lib -lfreetype -lpng -lz -lm

CC      = gcc
CFLAGS  = -Wl,-q -Wall -g -I$(SRCDIR)../../common/include #-O3
ASFLAGS = $(CFLAGS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET) $(OBJS)