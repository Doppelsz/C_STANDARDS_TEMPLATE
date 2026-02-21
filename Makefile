# Multi-threaded server — C99, POSIX. Unix-like systems only.
# Usage: make [all] | clean

CC     ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -I src/includes
LDFLAGS = -lpthread
BINDIR  = bin
SRC     = src/server.c
TARGET  = $(BINDIR)/server

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
