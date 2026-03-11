// Interface GTK3 — gui.c

#include "gui.h"
#include "arquivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ─── CSS Global ────────────────────────────────────────────────────────────────
static const char *CSS =
"window {"
"  background-color: #1e1e2e;"
"}"
"/* Tela de menu */"
".titulo {"
"  color: #cba6f7;"
"  font-size: 32px;"
"  font-weight: bold;"
"  font-family: 'Monospace';"
"}"
".subtitulo {"
"  color: #89b4fa;"
"  font-size: 13px;"
"  font-family: 'Monospace';"
"}"
"/* Botões do menu */"
".btn-menu {"
"  background-color: #313244;"
"  color: #cdd6f4;"
"  border-radius: 10px;"
"  border: 1px solid #45475a;"
"  font-size: 15px;"
"  font-weight: bold;"
"  padding: 12px 30px;"
"  font-family: 'Monospace';"
"  transition: all 120ms ease;"
"}"
".btn-menu:hover {"
"  background-color: #45475a;"
"  border-color: #89b4fa;"
"  color: #89b4fa;"
"}"
".btn-perigo {"
"  background-color: #313244;"
"  color: #f38ba8;"
"  border-radius: 10px;"
"  border: 1px solid #45475a;"
"  font-size: 15px;"
"  font-weight: bold;"
"  padding: 12px 30px;"
"  font-family: 'Monospace';"
"}"
".btn-perigo:hover {"
"  background-color: #3e2030;"
"  border-color: #f38ba8;"
"}"
"/* Tabuleiro */"
".tabuleiro {"
"  background-color: #181825;"
"  border-radius: 14px;"
"  padding: 12px;"
"  border: 2px solid #313244;"
"}"
"/* Peças numéricas */"
".peca {"
"  background-color: #313244;"
"  color: #89b4fa;"
"  border-radius: 10px;"
"  border: 2px solid #45475a;"
"  font-size: 20px;"
"  font-weight: bold;"
"  font-family: 'Monospace';"
"  transition: all 80ms ease;"
"}"
".peca:hover {"
"  background-color: #45475a;"
"  border-color: #89b4fa;"
"  color: #cdd6f4;"
"}"
".peca-vazia {"
"  background-color: #181825;"
"  border-radius: 10px;"
"  border: 2px dashed #313244;"
"}"
".peca-vazia:hover {"
"  background-color: #181825;"
"}"
"/* Barra de status */"
".barra-status {"
"  background-color: #181825;"
"  border-radius: 8px;"
"  padding: 6px 16px;"
"}"
".lbl-jogadas {"
"  color: #a6e3a1;"
"  font-size: 14px;"
"  font-weight: bold;"
"  font-family: 'Monospace';"
"}"
".lbl-status {"
"  color: #f9e2af;"
"  font-size: 12px;"
"  font-family: 'Monospace';"
"}"
"/* Vitória */"
".vitoria {"
"  color: #a6e3a1;"
"  font-size: 22px;"
"  font-weight: bold;"
"  font-family: 'Monospace';"
"}"
"/* Barra de ações no jogo */"
".btn-acao {"
"  background-color: #313244;"
"  color: #cdd6f4;"
"  border-radius: 8px;"
"  border: 1px solid #45475a;"
"  font-size: 13px;"
"  font-weight: bold;"
"  padding: 8px 18px;"
"  font-family: 'Monospace';"
"}"
".btn-acao:hover {"
"  background-color: #45475a;"
"  border-color: #89b4fa;"
"  color: #89b4fa;"
"}"
".separador {"
"  background-color: #313244;"
"}";

// ─── Estado global (único instância) ─────────────────────────────────────────
static AppState *g_app = NULL;

// ─── Helpers ──────────────────────────────────────────────────────────────────
static void aplicar_css(GtkWidget *widget, const char *classe) {
    GtkStyleContext *ctx = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(ctx, classe);
}

static void remover_css(GtkWidget *widget, const char *classe) {
    GtkStyleContext *ctx = gtk_widget_get_style_context(widget);
    gtk_style_context_remove_class(ctx, classe);
}

static void set_status(AppState *app, const char *msg) {
    gtk_label_set_text(GTK_LABEL(app->lbl_status), msg);
}

static void atualizar_jogadas(AppState *app) {
    char buf[64];
    snprintf(buf, sizeof(buf), "Jogadas: %d", app->jogo.jogadas);
    gtk_label_set_text(GTK_LABEL(app->lbl_jogadas), buf);
}

// ─── Atualiza o tabuleiro visual ──────────────────────────────────────────────
void gui_atualizar_tabuleiro(AppState *app) {
    int n = app->jogo.tamanho;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            GtkWidget *btn = app->botoes_peca[i][j];
            if (!btn) continue;

            int val = app->jogo.tabuleiro[i][j];
            if (val == VAZIO) {
                gtk_button_set_label(GTK_BUTTON(btn), "");
                gtk_widget_set_sensitive(btn, FALSE);
                remover_css(btn, "peca");
                aplicar_css(btn, "peca-vazia");
            } else {
                char label[8];
                snprintf(label, sizeof(label), "%d", val);
                gtk_button_set_label(GTK_BUTTON(btn), label);
                gtk_widget_set_sensitive(btn, TRUE);
                remover_css(btn, "peca-vazia");
                aplicar_css(btn, "peca");
            }
        }
    }
    atualizar_jogadas(app);
}

// ─── Callback: clique em uma peça ─────────────────────────────────────────────
typedef struct { AppState *app; int linha; int col; } DadosPeca;

static void cb_peca_clicada(GtkWidget *widget, gpointer data) {
    (void)widget;
    DadosPeca *d = (DadosPeca *)data;
    AppState  *app = d->app;
    // Lê o valor ATUAL da posição — corrige o bug de peça fixada no valor inicial
    int        val = app->jogo.tabuleiro[d->linha][d->col];

    // Tenta mover — a lógica original é preservada
    if (!moverPeca(&app->jogo, val)) {
        set_status(app, "⚠  Peça não pode ser movida para essa posição.");
        return;
    }

    gui_atualizar_tabuleiro(app);
    set_status(app, "");

    if (jogoCompleto(&app->jogo)) {
        // Mostra diálogo de vitória
        GtkWidget *dlg = gtk_message_dialog_new(
            GTK_WINDOW(app->janela),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "🎉 Parabéns! Você completou o quebra-cabeças!"
        );
        char msg[128];
        snprintf(msg, sizeof(msg), "Total de jogadas: %d", app->jogo.jogadas);
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dlg), "%s", msg);
        gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);

        liberaTabuleiro(&app->jogo);
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "menu");
    }
}

// ─── Constrói o grid do tabuleiro ─────────────────────────────────────────────
static void construir_grade(AppState *app) {
    // Remove grade antiga se existir
    if (app->grade) {
        gtk_widget_destroy(app->grade);
        app->grade = NULL;
    }

    int n = app->jogo.tamanho;
    int tam_btn = (n <= 4) ? 80 : (n <= 6) ? 66 : (n <= 8) ? 56 : 48;

    app->grade = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(app->grade), 6);
    gtk_grid_set_column_spacing(GTK_GRID(app->grade), 6);
    aplicar_css(app->grade, "tabuleiro");

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int val = app->jogo.tabuleiro[i][j];

            GtkWidget *btn = gtk_button_new();
            gtk_widget_set_size_request(btn, tam_btn, tam_btn);

            // Armazena dados para o callback
            DadosPeca *d = g_new(DadosPeca, 1);
            d->app   = app;
            d->linha = i;
            d->col   = j;
            g_signal_connect_data(btn, "clicked",
                G_CALLBACK(cb_peca_clicada), d,
                (GClosureNotify)g_free, 0);

            if (val == VAZIO) {
                gtk_button_set_label(GTK_BUTTON(btn), "");
                gtk_widget_set_sensitive(btn, FALSE);
                aplicar_css(btn, "peca-vazia");
            } else {
                char label[8];
                snprintf(label, sizeof(label), "%d", val);
                gtk_button_set_label(GTK_BUTTON(btn), label);
                aplicar_css(btn, "peca");
            }

            app->botoes_peca[i][j] = btn;
            gtk_grid_attach(GTK_GRID(app->grade), btn, j, i, 1, 1);
        }
    }

    // Insere a grade no container correto (box_jogo_centro)
    GtkWidget *box_jogo_centro = g_object_get_data(G_OBJECT(app->janela), "box_jogo_centro");
    if (box_jogo_centro) {
        gtk_box_pack_start(GTK_BOX(box_jogo_centro), app->grade, TRUE, TRUE, 0);
        gtk_widget_show_all(app->grade);
    }
}

// ─── Diálogo: Novo Jogo ────────────────────────────────────────────────────────
static void cb_novo_jogo(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppState *app = (AppState *)data;

    // Diálogo de configuração
    GtkWidget *dlg = gtk_dialog_new_with_buttons(
        "Novo Jogo",
        GTK_WINDOW(app->janela),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Iniciar", GTK_RESPONSE_OK,
        "_Cancelar", GTK_RESPONSE_CANCEL,
        NULL
    );
    gtk_window_set_default_size(GTK_WINDOW(dlg), 320, 220);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    gtk_container_set_border_width(GTK_CONTAINER(content), 20);
    gtk_box_set_spacing(GTK_BOX(content), 14);

    // Tamanho
    GtkWidget *lbl_tam = gtk_label_new("Tamanho do tabuleiro (3–10):");
    gtk_widget_set_halign(lbl_tam, GTK_ALIGN_START);
    GtkWidget *spin = gtk_spin_button_new_with_range(3, 10, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), 4);

    // Dificuldade
    GtkWidget *lbl_dif = gtk_label_new("Dificuldade:");
    gtk_widget_set_halign(lbl_dif, GTK_ALIGN_START);
    GtkWidget *combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), "F", "🟢  Fácil");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), "M", "🔵  Médio");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), "D", "🔴  Difícil");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 1);

    gtk_box_pack_start(GTK_BOX(content), lbl_tam, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), spin,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), lbl_dif, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), combo,   FALSE, FALSE, 0);
    gtk_widget_show_all(dlg);

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_OK) {
        int tam = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin));
        const char *dif_id = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo));
        char dif = dif_id ? dif_id[0] : 'M';

        gtk_widget_destroy(dlg);

        // Cria o jogo usando a lógica original
        if (app->jogo.jogoEmAndamento) liberaTabuleiro(&app->jogo);

        app->jogo.tabuleiro = criaTabuleiro(tam);
        app->jogo.tamanho   = tam;
        inicializaTabuleiro(&app->jogo);
        app->jogo.jogoEmAndamento = 1;
        embaralhaTabuleiro(&app->jogo, dif);

        construir_grade(app);
        gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "jogo");
        set_status(app, "Jogo iniciado! Clique nas peças para movê-las.");
        atualizar_jogadas(app);
    } else {
        gtk_widget_destroy(dlg);
    }
}

// ─── Diálogo: Salvar ──────────────────────────────────────────────────────────
static void cb_salvar(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppState *app = (AppState *)data;

    if (!app->jogo.jogoEmAndamento) {
        set_status(app, "⚠  Nenhum jogo em andamento para salvar.");
        return;
    }

    GtkWidget *dlg = gtk_file_chooser_dialog_new(
        "Salvar Jogo",
        GTK_WINDOW(app->janela),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancelar", GTK_RESPONSE_CANCEL,
        "_Salvar",   GTK_RESPONSE_ACCEPT,
        NULL
    );
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dlg), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dlg), "meu_jogo.qcb");

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        if (salvarJogo(&app->jogo, path)) {
            set_status(app, "✅  Jogo salvo com sucesso!");
        } else {
            set_status(app, "❌  Erro ao salvar o arquivo.");
        }
        g_free(path);
    }
    gtk_widget_destroy(dlg);
}

// ─── Diálogo: Carregar ────────────────────────────────────────────────────────
static void cb_carregar(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppState *app = (AppState *)data;

    GtkWidget *dlg = gtk_file_chooser_dialog_new(
        "Carregar Jogo",
        GTK_WINDOW(app->janela),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancelar", GTK_RESPONSE_CANCEL,
        "_Abrir",    GTK_RESPONSE_ACCEPT,
        NULL
    );

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        gtk_widget_destroy(dlg);

        if (app->jogo.jogoEmAndamento) liberaTabuleiro(&app->jogo);

        if (carregarJogo(&app->jogo, path)) {
            g_free(path);
            construir_grade(app);
            gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "jogo");
            set_status(app, "✅  Jogo carregado com sucesso!");
            atualizar_jogadas(app);
        } else {
            g_free(path);
            set_status(app, "❌  Erro ao carregar o arquivo.");
        }
    } else {
        gtk_widget_destroy(dlg);
    }
}

// ─── Callback: Voltar ao menu ─────────────────────────────────────────────────
static void cb_voltar_menu(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppState *app = (AppState *)data;

    if (app->jogo.jogoEmAndamento) {
        // Pergunta se quer salvar antes de sair
        GtkWidget *dlg = gtk_message_dialog_new(
            GTK_WINDOW(app->janela),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Voltar ao menu principal?"
        );
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dlg),
            "O progresso não salvo será perdido.");
        int resp = gtk_dialog_run(GTK_DIALOG(dlg));
        gtk_widget_destroy(dlg);
        if (resp != GTK_RESPONSE_YES) return;
    }

    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "menu");
}

// ─── Tela: Menu Principal ─────────────────────────────────────────────────────
static GtkWidget *criar_tela_menu(AppState *app) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(box), 40);

    // Título
    GtkWidget *titulo = gtk_label_new("🧩 Quebra-Cabeças");
    aplicar_css(titulo, "titulo");
    gtk_box_pack_start(GTK_BOX(box), titulo, FALSE, FALSE, 0);

    GtkWidget *sub = gtk_label_new("Deslize as peças e complete o puzzle!");
    aplicar_css(sub, "subtitulo");
    gtk_box_pack_start(GTK_BOX(box), sub, FALSE, FALSE, 8);

    // Separador
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    aplicar_css(sep, "separador");
    gtk_box_pack_start(GTK_BOX(box), sep, FALSE, FALSE, 10);

    // Botões
    struct { const char *label; GCallback cb; } btns[] = {
        { "▶   Novo Jogo",         G_CALLBACK(cb_novo_jogo) },
        { "📂  Carregar Jogo",     G_CALLBACK(cb_carregar)  },
    };
    for (int i = 0; i < 2; i++) {
        GtkWidget *b = gtk_button_new_with_label(btns[i].label);
        gtk_widget_set_size_request(b, 260, 52);
        aplicar_css(b, "btn-menu");
        g_signal_connect(b, "clicked", btns[i].cb, app);
        gtk_box_pack_start(GTK_BOX(box), b, FALSE, FALSE, 0);
    }

    return box;
}

// ─── Tela: Jogo ───────────────────────────────────────────────────────────────
static GtkWidget *criar_tela_jogo(AppState *app) {
    // Layout principal: coluna central
    GtkWidget *box_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_set_border_width(GTK_CONTAINER(box_principal), 16);

    // ── Barra superior ──
    GtkWidget *box_topo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(box_topo), FALSE);

    GtkWidget *btn_voltar = gtk_button_new_with_label("← Menu");
    aplicar_css(btn_voltar, "btn-acao");
    g_signal_connect(btn_voltar, "clicked", G_CALLBACK(cb_voltar_menu), app);

    GtkWidget *btn_novo2 = gtk_button_new_with_label("▶ Novo");
    aplicar_css(btn_novo2, "btn-acao");
    g_signal_connect(btn_novo2, "clicked", G_CALLBACK(cb_novo_jogo), app);

    GtkWidget *btn_salvar2 = gtk_button_new_with_label("💾 Salvar");
    aplicar_css(btn_salvar2, "btn-acao");
    g_signal_connect(btn_salvar2, "clicked", G_CALLBACK(cb_salvar), app);

    GtkWidget *btn_carregar2 = gtk_button_new_with_label("📂 Carregar");
    aplicar_css(btn_carregar2, "btn-acao");
    g_signal_connect(btn_carregar2, "clicked", G_CALLBACK(cb_carregar), app);

    // Espacor
    GtkWidget *espacor = gtk_label_new("");
    gtk_widget_set_hexpand(espacor, TRUE);

    // Jogadas
    app->lbl_jogadas = gtk_label_new("Jogadas: 0");
    aplicar_css(app->lbl_jogadas, "lbl-jogadas");

    gtk_box_pack_start(GTK_BOX(box_topo), btn_voltar,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_topo), btn_novo2,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_topo), btn_salvar2,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_topo), btn_carregar2,FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_topo), espacor,      TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(box_topo), app->lbl_jogadas, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_principal), box_topo, FALSE, FALSE, 10);

    // ── Separador ──
    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    aplicar_css(sep, "separador");
    gtk_box_pack_start(GTK_BOX(box_principal), sep, FALSE, FALSE, 8);

    // ── Centro: onde a grade vai ser inserida ──
    GtkWidget *box_centro = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(box_centro, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box_centro, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(box_centro, TRUE);
    // Salva referência para inserir a grade depois
    g_object_set_data(G_OBJECT(app->janela), "box_jogo_centro", box_centro);
    gtk_box_pack_start(GTK_BOX(box_principal), box_centro, TRUE, TRUE, 0);

    // ── Barra de status ──
    GtkWidget *box_status = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    aplicar_css(box_status, "barra-status");
    gtk_container_set_border_width(GTK_CONTAINER(box_status), 6);

    app->lbl_status = gtk_label_new("");
    aplicar_css(app->lbl_status, "lbl-status");
    gtk_widget_set_halign(app->lbl_status, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(box_status), app->lbl_status, TRUE, TRUE, 0);

    gtk_box_pack_end(GTK_BOX(box_principal), box_status, FALSE, FALSE, 8);

    return box_principal;
}

// ─── Ponto de entrada da GUI ──────────────────────────────────────────────────
void gui_inicializar(AppState *app) {
    g_app = app;

    // Aplica folha de estilos CSS global
    GtkCssProvider *css_prov = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_prov, CSS, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_prov),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    // Janela principal
    app->janela = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->janela), "Quebra-Cabeças");
    gtk_window_set_default_size(GTK_WINDOW(app->janela), 700, 580);
    gtk_window_set_position(GTK_WINDOW(app->janela), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(app->janela), 0);
    g_signal_connect(app->janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Stack (troca entre menu e jogo)
    app->stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(app->stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(GTK_STACK(app->stack), 200);

    GtkWidget *tela_menu = criar_tela_menu(app);
    GtkWidget *tela_jogo = criar_tela_jogo(app);

    gtk_stack_add_named(GTK_STACK(app->stack), tela_menu, "menu");
    gtk_stack_add_named(GTK_STACK(app->stack), tela_jogo, "jogo");
    gtk_stack_set_visible_child_name(GTK_STACK(app->stack), "menu");

    gtk_container_add(GTK_CONTAINER(app->janela), app->stack);
    gtk_widget_show_all(app->janela);
}
