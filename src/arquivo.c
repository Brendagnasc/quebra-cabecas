#include "arquivo.h"
#include <stdio.h>

//Gravar o estado atual do tabuleiro e a quantidade de jogadas em um arquivo
int salvarJogo (Jogo *jogo, const char* nomeArquivo) {
    FILE *arquivo = fopen (nomeArquivo, "w");
    if (arquivo == NULL) {
        return 0; // Falha ao abrir o arquivo
    }

    // 1 Linha: Dimensao da matriz
    fprintf(arquivo, "%d\n", jogo->tamanho);

    // Matriz de dados 
    for (int i = 0; i < jogo->tamanho; i++) {
        for (int j = 0; j < jogo->tamanho; j++) {
            fprintf(arquivo, "%d ", jogo->tabuleiro[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    // Ultima linha: Quantidade de jogadas
    fprintf(arquivo, "%d\n", jogo->jogadas);

    fclose(arquivo);
    return 1; // Sucesso
}

// Lê um arquivo .txt e reconstroi a matriz do jogo em memoria 
int carregarJogo (Jogo *jogo, const char* nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        return 0; // Falha ao abrir o arquivo
    }

    // Le a dimensao do tabuleiro. Se falhar, o arquivo esta corrompido
    if (fscanf(arquivo, "%d", &jogo->tamanho) != 1) {
        fclose(arquivo);
        return 0; // Falha ao ler a dimensão
    }
    
    jogo->tabuleiro = criaTabuleiro(jogo->tamanho);

    // Preenche a matriz lendo os numeros separados por espaço
    for (int i = 0; i < jogo->tamanho; i++) {
        for (int j = 0; j < jogo->tamanho; j++){
            fscanf(arquivo, "%d", &jogo->tabuleiro[i][j]);
        }
    }

    // Le pontuação atual
    fscanf(arquivo, "%d", &jogo->jogadas);

    fclose(arquivo);
    jogo->jogoEmAndamento = 1; // Marca o jogo como em andamento
    return 1; // Sucesso
}

