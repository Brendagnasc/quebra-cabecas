// Brenda Gabrielle Alves Nascimento
// main.c — Ponto de entrada com interface GTK3

#include <gtk/gtk.h>
#include "jogo.h"
#include "gui.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    AppState app;
    inicializaJogo(&app.jogo);
    app.grade       = NULL;
    app.lbl_jogadas = NULL;
    app.lbl_status  = NULL;
    app.btn_novo    = NULL;
    app.btn_salvar  = NULL;
    app.btn_carregar= NULL;
    app.stack       = NULL;

    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            app.botoes_peca[i][j] = NULL;

    gui_inicializar(&app);
    gtk_main();

    liberaTabuleiro(&app.jogo);
    return 0;
}
