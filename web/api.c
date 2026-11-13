// Brenda Gabrielle Alves Nascimento
// api.c — ponte entre a lógica original em C (jogo.c / arquivo.c) e a
// interface web. Compilado com Emscripten para WebAssembly: o JavaScript só
// desenha a tela e chama estas funções; as regras do jogo continuam em C.

#include <emscripten/emscripten.h>
#include "../src/jogo.h"
#include "../src/arquivo.h"

// arquivo usado no sistema de arquivos em memória do Emscripten (MEMFS)
// para reaproveitar salvarJogo/carregarJogo sem alterar arquivo.c
#define ARQUIVO_TEMP "/partida.qcb"

static Jogo jogo;
static int iniciado = 0;

static void garanteIniciado(void) {
    if (!iniciado) {
        inicializaJogo(&jogo);
        iniciado = 1;
    }
}

// Cria um tabuleiro n x n embaralhado. dificuldade: 'F', 'M' ou 'D'.
EMSCRIPTEN_KEEPALIVE
int qc_novo(int n, int dificuldade) {
    garanteIniciado();
    if (n < 3 || n > TAM) return 0;

    if (jogo.jogoEmAndamento) liberaTabuleiro(&jogo);

    jogo.tabuleiro = criaTabuleiro(n);
    if (!jogo.tabuleiro) return 0;
    jogo.tamanho = n;
    inicializaTabuleiro(&jogo);
    jogo.jogoEmAndamento = 1;

    return embaralhaTabuleiro(&jogo, (char)dificuldade);
}

EMSCRIPTEN_KEEPALIVE
int qc_tamanho(void) { return jogo.jogoEmAndamento ? jogo.tamanho : 0; }

EMSCRIPTEN_KEEPALIVE
int qc_valor(int linha, int coluna) { return jogo.tabuleiro[linha][coluna]; }

EMSCRIPTEN_KEEPALIVE
int qc_jogadas(void) { return jogo.jogadas; }

EMSCRIPTEN_KEEPALIVE
int qc_mover(int valor) { return moverPeca(&jogo, valor); }

EMSCRIPTEN_KEEPALIVE
int qc_completo(void) { return jogoCompleto(&jogo); }

// Grava a partida em ARQUIVO_TEMP (mesmo formato .qcb da versão desktop).
// O JavaScript lê o arquivo do MEMFS e oferece para download.
EMSCRIPTEN_KEEPALIVE
int qc_salvar(void) {
    if (!jogo.jogoEmAndamento) return 0;
    return salvarJogo(&jogo, ARQUIVO_TEMP);
}

// Carrega a partida de ARQUIVO_TEMP, escrito antes pelo JavaScript
// a partir do arquivo que o usuário escolheu.
EMSCRIPTEN_KEEPALIVE
int qc_carregar(void) {
    garanteIniciado();
    if (jogo.jogoEmAndamento) liberaTabuleiro(&jogo);

    if (!carregarJogo(&jogo, ARQUIVO_TEMP)) {
        inicializaJogo(&jogo);
        return 0;
    }

    // arquivo.c não valida o conteúdo: recusa tamanhos fora do intervalo
    // e tabuleiros com peças repetidas ou fora da faixa
    int n = jogo.tamanho;
    int ok = (n >= 3 && n <= TAM && jogo.tabuleiro != NULL);
    if (ok) {
        int vistos[TAM * TAM] = {0};
        for (int i = 0; i < n && ok; i++) {
            for (int j = 0; j < n && ok; j++) {
                int v = jogo.tabuleiro[i][j];
                if (v < 0 || v >= n * n || vistos[v]) ok = 0;
                else vistos[v] = 1;
            }
        }
    }
    if (!ok) {
        if (jogo.tabuleiro != NULL && n >= 1 && n <= TAM) liberaTabuleiro(&jogo);
        inicializaJogo(&jogo);
        return 0;
    }
    return 1;
}
