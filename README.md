# 🧩 Quebra-Cabeças em C

Jogo de quebra-cabeças deslizante (sliding puzzle) desenvolvido em C,
com interface gráfica GTK3.

**Jogue no navegador:** https://brendagnasc.github.io/quebra-cabecas/
(faz parte do [menu de jogos](https://brendagnasc.github.io/jogos/))

## 📸 Funcionalidades

- Tabuleiro configurável de 3×3 até 10×10
- Três níveis de dificuldade (Fácil, Médio, Difícil)
- Interface gráfica com tema escuro
- Sistema de salvar e carregar partidas
- Contador de jogadas em tempo real

## 🛠️ Tecnologias

- Linguagem: C (C11)
- Interface: GTK3
- Gerenciamento de memória dinâmica com malloc/free

## ▶️ Como compilar
```bash
sudo apt-get install libgtk-3-dev gcc make
make
make run
```

## 🌐 Versão web (WebAssembly)

A demo usa **o mesmo código C** da versão desktop: `src/jogo.c` e `src/arquivo.c`
são compilados para WebAssembly com [Emscripten](https://emscripten.org/), e só a
interface foi refeita em HTML/CSS/JS (no lugar do GTK3), com o mesmo tema escuro.

- `web/api.c` — ponte fina que expõe as funções do jogo para o JavaScript
- `web/app.js` — desenha o tabuleiro e chama o C a cada jogada
- Salvar/carregar usa `salvarJogo`/`carregarJogo` originais sobre o sistema de
  arquivos em memória do Emscripten; o arquivo `.qcb` baixado é compatível com a
  versão desktop
- Extra da versão web: mover peças com as setas do teclado

```bash
make web                       # requer emcc no PATH
python3 -m http.server -d web  # abra http://localhost:8000
```

A cada push na `main`, o GitHub Actions compila e publica a demo no GitHub Pages
(`.github/workflows/pages.yml`).

## 📚 Contexto acadêmico

Projeto iniciado no 1º período do curso de Ciência da Computação,
aprimorado ao longo dos períodos seguintes.
Evoluiu de uma interface de terminal para uma aplicação GTK3 completa.