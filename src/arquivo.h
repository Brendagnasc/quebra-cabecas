// Brenda Gabrielle Alves Nascimento

#ifndef ARQUIVO_H
#define ARQUIVO_H

#include "jogo.h"

int salvarJogo(Jogo *jogo, const char* nomeArquivo);
int carregarJogo(Jogo *jogo, const char* nomeArquivo);

#endif