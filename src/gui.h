// Brenda Gabrielle Alves Nascimento
// Interface GTK3 para o Quebra-Cabeças

#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "jogo.h"

// Paleta de cores
#define COR_FUNDO       "#1e1e2e"
#define COR_PAINEL      "#313244"
#define COR_BOTAO       "#45475a"
#define COR_BOTAO_HOV   "#585b70"
#define COR_PECA        "#89b4fa"
#define COR_PECA_HOV    "#b4d0ff"
#define COR_VAZIO       "#181825"
#define COR_TEXTO       "#cdd6f4"
#define COR_DESTAQUE    "#a6e3a1"
#define COR_ERRO        "#f38ba8"
#define COR_TITULO      "#cba6f7"

// Estado da aplicação
typedef struct {
    Jogo          jogo;
    GtkWidget    *janela;
    GtkWidget    *grade;          // Grid das peças
    GtkWidget    *lbl_jogadas;
    GtkWidget    *lbl_status;
    GtkWidget    *btn_novo;
    GtkWidget    *btn_salvar;
    GtkWidget    *btn_carregar;
    GtkWidget    *stack;          // Para trocar entre telas
    GtkWidget    *botoes_peca[10][10]; // Referências aos botões do tabuleiro
} AppState;

// Funções públicas
void gui_inicializar(AppState *app);
void gui_atualizar_tabuleiro(AppState *app);
void gui_mostrar_tela_menu(AppState *app);
void gui_mostrar_tela_jogo(AppState *app);

#endif
