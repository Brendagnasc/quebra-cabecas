# Como compilar a versão GTK3

## Ubuntu / Debian / WSL
```bash
# 1. Instalar dependências (apenas uma vez)
sudo apt-get install libgtk-3-dev gcc make

# 2. Compilar
make

# 3. Executar
make run
# ou
./build/quebra-cabecas-gtk
```

## Compilação manual (sem make)
```bash
gcc -Wall -Wextra -O2 \
    $(pkg-config --cflags gtk+-3.0) \
    src/main.c src/gui.c src/jogo.c src/arquivo.c \
    -o build/quebra-cabecas-gtk \
    $(pkg-config --libs gtk+-3.0)
```

## Estrutura dos arquivos
```
quebracabeca_gtk/
├── Makefile
├── COMO_COMPILAR.md
├── saves/              ← pasta para salvar jogos
└── src/
    ├── main.c          ← ponto de entrada GTK
    ├── gui.c           ← toda a interface gráfica
    ├── gui.h
    ├── jogo.c          ← lógica do jogo (original)
    ├── jogo.h
    ├── arquivo.c       ← salvar/carregar (original)
    └── arquivo.h
```

## Funcionalidades da interface
- Menu principal com botões Novo Jogo e Carregar
- Diálogo para escolher tamanho (3–10) e dificuldade (Fácil/Médio/Difícil)
- Tabuleiro interativo: clique na peça para movê-la
- Contador de jogadas em tempo real
- Salvar/Carregar via seletor de arquivos nativo do sistema
- Confirmação ao voltar ao menu com jogo em andamento
- Diálogo de vitória ao completar o puzzle
- Tema escuro (Catppuccin Mocha)
