CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -D_POSIX_C_SOURCE=200809L
SRCDIR = src
TARGET = ordenacao_combustivel

SRCS = $(SRCDIR)/main.c \
       $(SRCDIR)/csv.c \
       $(SRCDIR)/chave.c \
       $(SRCDIR)/tempo.c \
       $(SRCDIR)/ordenacao.c \
       $(SRCDIR)/benchmark.c \
       $(SRCDIR)/avl.c \
       $(SRCDIR)/rubro_negra.c \
       $(SRCDIR)/benchmark_arvore.c

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
