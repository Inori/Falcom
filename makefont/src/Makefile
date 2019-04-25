SRCDIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
TARGET  = $(SRCDIR)makefont

OBJS = $(patsubst %.c, %.o, $(wildcard $(SRCDIR)*.c))
HEADERS = $(wildcard *.h)

#LIBS    = -L$(SRCDIR)../../common/lib -lfreetype -lpng -lz -lm
LIBS    = -L/opt/local/lib -lfreetype -lpng -lz -liconv

CC      = gcc
#CFLAGS  = -Wl,-q -Wall -std=gnu11 -g -I$(SRCDIR)../../common/include #-O3
CFLAGS  = -Wl, -Wall -std=gnu11 -g -I$(SRCDIR)../../common/include -I/opt/local/include #-O3
ASFLAGS = $(CFLAGS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET) $(OBJS)