# Makefile — Quebra-Cabeças GTK3
# Brenda Gabrielle Alves Nascimento

CC      = gcc
CFLAGS  = -Wall -Wextra -O2 $(shell pkg-config --cflags gtk+-3.0)
LIBS    = $(shell pkg-config --libs gtk+-3.0)
SRC     = src/main.c src/gui.c src/jogo.c src/arquivo.c
TARGET  = build/quebra-cabecas-gtk

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)
	@echo "✅ Compilado: $(TARGET)"

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
