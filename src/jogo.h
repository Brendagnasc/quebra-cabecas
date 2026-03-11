// Brenda Gabrielle Alves Nascimento

#ifndef JOGO_H
#define JOGO_H

#define VAZIO 0
#define MAX 100 
#define TAM 10

typedef struct {
    int **tabuleiro;
    int tamanho;
    int jogadas;
    int jogoEmAndamento;
} Jogo;

void inicializaJogo (Jogo *jogo);
int** criaTabuleiro (int n);
void liberaTabuleiro (Jogo *jogo);
void inicializaTabuleiro (Jogo *jogo);
int jogoCompleto (Jogo *jogo);
int embaralhaTabuleiro (Jogo *jogo, char dificuldade);
int moverPeca (Jogo *jogo, int valor);

#endif 