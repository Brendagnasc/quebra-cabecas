#include "jogo.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Prepara a struct para um novo uso
void inicializaJogo(Jogo *jogo) {
    jogo->tabuleiro = NULL;
    jogo->tamanho = 0;
    jogo->jogadas = 0;
    jogo->jogoEmAndamento = 0;
}

// Alocacao dinamica da matriz bidimensional do tabuleiro
int** criaTabuleiro(int n) {
    int** tabuleiro = (int**) malloc((size_t)n * sizeof(int*));
    if (!tabuleiro) return NULL;

    for (int i = 0; i < n; i++) {
        tabuleiro[i] = (int*) malloc((size_t)n * sizeof(int));
        if (!tabuleiro[i]) {
            for (int k = 0; k < i; k++) free(tabuleiro[k]);
            free(tabuleiro);
            return NULL;
        }
    }
    return tabuleiro;
}

// Liberacao da memoria alocada para o tabuleiro
void liberaTabuleiro(Jogo *jogo) {
    if (jogo->tabuleiro != NULL) {
        for (int i = 0; i < jogo->tamanho; i++) {
            free(jogo->tabuleiro[i]);
        }
        free(jogo->tabuleiro);
        jogo->tabuleiro = NULL;
    }
    jogo->jogoEmAndamento = 0;
}

// Preenche o tabuleiro com os numeros em ordem crescente e o vazio no final
void inicializaTabuleiro(Jogo *jogo) {
    int num = 1;
    int n = jogo->tamanho;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == n-1 && j == n-1) {
                jogo->tabuleiro[i][j] = VAZIO;
            } else {
                jogo->tabuleiro[i][j] = num++;
            }
        }
    }
    jogo->jogadas = 0;
}

// Verifica se o jogo foi completado
int jogoCompleto(Jogo *jogo) {
    int num = 1;
    int n = jogo->tamanho;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == n-1 && j == n-1) {
                return (jogo->tabuleiro[i][j] == VAZIO);
            }
            if (jogo->tabuleiro[i][j] != num++) {
                return 0;
            }
        }
    }
    return 1;
}

// Executa movimentos aleatorios validos. Retorna 1 em sucesso, 0 se dificuldade invalida.
int embaralhaTabuleiro(Jogo *jogo, char dificuldade) {
    srand((unsigned int)time(NULL));
    int movimentos = 0;

    if      (dificuldade == 'F' || dificuldade == 'f') movimentos = 5  * jogo->tamanho;
    else if (dificuldade == 'M' || dificuldade == 'm') movimentos = 10 * jogo->tamanho;
    else if (dificuldade == 'D' || dificuldade == 'd') movimentos = 15 * jogo->tamanho;
    else return 0;

    int vazioX = jogo->tamanho - 1;
    int vazioY = jogo->tamanho - 1;
    int ultimoMovimento = -1;

    for (int i = 0; i < movimentos; ) {
        int direcao = rand() % 4;

        if ((direcao == 0 && ultimoMovimento == 1) ||
            (direcao == 1 && ultimoMovimento == 0) ||
            (direcao == 2 && ultimoMovimento == 3) ||
            (direcao == 3 && ultimoMovimento == 2)) {
            continue;
        }

        int moveu = 0;
        switch (direcao) {
            case 0: // Move o vazio para a esquerda
                if (vazioY > 0) {
                    jogo->tabuleiro[vazioX][vazioY]     = jogo->tabuleiro[vazioX][vazioY - 1];
                    jogo->tabuleiro[vazioX][vazioY - 1] = VAZIO;
                    vazioY--;
                    moveu = 1;
                }
                break;
            case 1: // Move o vazio para a direita  [BUG CORRIGIDO: era tabuleiro[vazioX-1][vazioY]]
                if (vazioY < jogo->tamanho - 1) {
                    jogo->tabuleiro[vazioX][vazioY]     = jogo->tabuleiro[vazioX][vazioY + 1];
                    jogo->tabuleiro[vazioX][vazioY + 1] = VAZIO;
                    vazioY++;
                    moveu = 1;
                }
                break;
            case 2: // Move o vazio para cima
                if (vazioX > 0) {
                    jogo->tabuleiro[vazioX][vazioY]     = jogo->tabuleiro[vazioX - 1][vazioY];
                    jogo->tabuleiro[vazioX - 1][vazioY] = VAZIO;
                    vazioX--;
                    moveu = 1;
                }
                break;
            case 3: // Move o vazio para baixo
                if (vazioX < jogo->tamanho - 1) {
                    jogo->tabuleiro[vazioX][vazioY]     = jogo->tabuleiro[vazioX + 1][vazioY];
                    jogo->tabuleiro[vazioX + 1][vazioY] = VAZIO;
                    vazioX++;
                    moveu = 1;
                }
                break;
        }

        if (moveu) {
            ultimoMovimento = direcao;
            i++;
        }
    }
    return 1;
}

// Procura a peca solicitada e tenta trocar de lugar com o espaco vazio
int moverPeca(Jogo *jogo, int valor) {
    int vazioX = -1, vazioY = -1, valorX = -1, valorY = -1;
    int n = jogo->tamanho;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (jogo->tabuleiro[i][j] == VAZIO) {
                vazioX = i; vazioY = j;
            } else if (jogo->tabuleiro[i][j] == valor) {
                valorX = i; valorY = j;
            }
        }
    }

    if (valorX == -1 || valorY == -1) return 0;

    if ((valorX == vazioX && abs(valorY - vazioY) == 1) ||
        (valorY == vazioY && abs(valorX - vazioX) == 1)) {
        jogo->tabuleiro[vazioX][vazioY] = valor;
        jogo->tabuleiro[valorX][valorY] = VAZIO;
        jogo->jogadas++;
        return 1;
    }

    return 0;
}
