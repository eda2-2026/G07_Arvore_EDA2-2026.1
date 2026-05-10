CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
SRCDIR = src
TARGET = ordenacao_combustivel

SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/csv.c \
       $(SRCDIR)/ordenacao.c \
       $(SRCDIR)/benchmark.c

ifeq ($(OS),Windows_NT)
    TARGET = ordenacao_combustivel.exe
endif

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f ordenacao_combustivel ordenacao_combustivel.exe

.PHONY: all run clean
