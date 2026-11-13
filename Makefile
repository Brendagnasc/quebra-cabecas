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
	rm -f $(TARGET) web/qc.js web/qc.wasm

# Versão web: compila a lógica original (jogo.c + arquivo.c) para WebAssembly.
# Requer o Emscripten (emcc). A interface fica em web/ (HTML, CSS e JS).
EMCC      = emcc
WEB_SRC   = src/jogo.c src/arquivo.c web/api.c
WEB_FLAGS = -O2 -sMODULARIZE -sEXPORT_NAME=createQC -sENVIRONMENT=web \
            -sEXPORTED_RUNTIME_METHODS=cwrap,FS -sFORCE_FILESYSTEM

web: web/qc.js

web/qc.js: $(WEB_SRC)
	$(EMCC) $(WEB_FLAGS) $(WEB_SRC) -o web/qc.js
	@echo "✅ Versão web: abra web/index.html por um servidor local"

.PHONY: all run clean web
