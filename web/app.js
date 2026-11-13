// Interface web do Quebra-Cabeças.
// Toda a regra do jogo (embaralhar, mover, verificar vitória, salvar/carregar)
// roda no código C original compilado para WebAssembly; aqui só tem a tela.

const ARQUIVO_TEMP = "/partida.qcb"; // mesmo caminho usado em web/api.c

const $ = (id) => document.getElementById(id);
const telaMenu = $("telaMenu");
const telaJogo = $("telaJogo");
const tabuleiro = $("tabuleiro");
const lblJogadas = $("lblJogadas");
const lblStatus = $("lblStatus");
const dlgNovo = $("dlgNovo");
const dlgVitoria = $("dlgVitoria");
const inpArquivo = $("inpArquivo");

let qc = null;   // funções C expostas pelo WebAssembly
let modulo = null;

createQC().then((m) => {
  modulo = m;
  qc = {
    novo:      m.cwrap("qc_novo", "number", ["number", "number"]),
    tamanho:   m.cwrap("qc_tamanho", "number", []),
    valor:     m.cwrap("qc_valor", "number", ["number", "number"]),
    jogadas:   m.cwrap("qc_jogadas", "number", []),
    mover:     m.cwrap("qc_mover", "number", ["number"]),
    completo:  m.cwrap("qc_completo", "number", []),
    salvar:    m.cwrap("qc_salvar", "number", []),
    carregar:  m.cwrap("qc_carregar", "number", [])
  };
  $("carregando").remove();
  document.querySelectorAll("[data-acao]").forEach((b) => (b.disabled = false));
}).catch((erro) => {
  $("carregando").textContent = "Erro ao carregar o WebAssembly: " + erro;
});

// ---------- telas ----------

function mostrarTela(nome) {
  telaMenu.hidden = nome !== "menu";
  telaJogo.hidden = nome !== "jogo";
}

function setStatus(msg) {
  lblStatus.textContent = msg;
}

// ---------- tabuleiro ----------

function desenharTabuleiro() {
  const n = qc.tamanho();
  tabuleiro.style.setProperty("--n", n);
  tabuleiro.replaceChildren();

  for (let i = 0; i < n; i++) {
    for (let j = 0; j < n; j++) {
      const valor = qc.valor(i, j);
      const botao = document.createElement("button");
      if (valor === 0) {
        botao.className = "peca-vazia";
        botao.disabled = true;
        botao.setAttribute("aria-label", "espaço vazio");
      } else {
        botao.className = "peca";
        botao.textContent = valor;
        botao.addEventListener("click", () => moverPeca(valor));
      }
      tabuleiro.appendChild(botao);
    }
  }
  lblJogadas.textContent = "Jogadas: " + qc.jogadas();
}

function moverPeca(valor) {
  if (!qc.mover(valor)) {
    setStatus("⚠  Peça não pode ser movida para essa posição.");
    return;
  }
  setStatus("");
  desenharTabuleiro();

  if (qc.completo()) {
    $("lblVitoria").textContent = "Total de jogadas: " + qc.jogadas();
    dlgVitoria.showModal();
  }
}

dlgVitoria.addEventListener("close", () => mostrarTela("menu"));

// setas do teclado: move a peça vizinha para o espaço vazio
const DESLOCAMENTO_TECLA = {
  ArrowUp:    [1, 0],  // a peça abaixo do vazio sobe
  ArrowDown:  [-1, 0],
  ArrowLeft:  [0, 1],
  ArrowRight: [0, -1]
};

document.addEventListener("keydown", (e) => {
  const d = DESLOCAMENTO_TECLA[e.key];
  if (!d || telaJogo.hidden || document.querySelector("dialog[open]")) return;
  e.preventDefault();

  const n = qc.tamanho();
  for (let i = 0; i < n; i++) {
    for (let j = 0; j < n; j++) {
      if (qc.valor(i, j) !== 0) continue;
      const li = i + d[0];
      const cj = j + d[1];
      if (li >= 0 && li < n && cj >= 0 && cj < n) moverPeca(qc.valor(li, cj));
      return;
    }
  }
});

// ---------- ações ----------

const acoes = {
  novo() {
    dlgNovo.showModal();
  },

  menu() {
    if (qc.tamanho() && !confirm("Voltar ao menu principal?\nO progresso não salvo será perdido.")) return;
    mostrarTela("menu");
  },

  salvar() {
    if (!qc.salvar()) {
      setStatus("⚠  Nenhum jogo em andamento para salvar.");
      return;
    }
    // o C gravou o arquivo no sistema de arquivos em memória; baixa ele
    const conteudo = modulo.FS.readFile(ARQUIVO_TEMP, { encoding: "utf8" });
    const link = document.createElement("a");
    link.href = URL.createObjectURL(new Blob([conteudo], { type: "text/plain" }));
    link.download = "meu_jogo.qcb";
    link.click();
    URL.revokeObjectURL(link.href);
    setStatus("✅  Jogo salvo com sucesso!");
  },

  carregar() {
    inpArquivo.value = "";
    inpArquivo.click();
  }
};

document.querySelectorAll("[data-acao]").forEach((botao) => {
  botao.addEventListener("click", () => acoes[botao.dataset.acao]());
});

dlgNovo.addEventListener("close", () => {
  if (dlgNovo.returnValue !== "ok") return;
  const n = Math.min(10, Math.max(3, parseInt($("inpTamanho").value, 10) || 4));
  const dificuldade = $("selDificuldade").value.charCodeAt(0);

  qc.novo(n, dificuldade);
  desenharTabuleiro();
  mostrarTela("jogo");
  setStatus("Jogo iniciado! Clique nas peças para movê-las.");
});

inpArquivo.addEventListener("change", async () => {
  const arquivo = inpArquivo.files[0];
  if (!arquivo) return;
  const texto = await arquivo.text();

  // arquivo.c aloca o tabuleiro com o tamanho lido do arquivo antes de
  // validar qualquer coisa, então barra tamanhos absurdos aqui
  const tamanho = parseInt(texto.trim().split(/\s+/)[0], 10);
  if (!(tamanho >= 3 && tamanho <= 10)) {
    setStatus("❌  Erro ao carregar o arquivo.");
    alert("Arquivo inválido: o tamanho do tabuleiro deve ser de 3 a 10.");
    return;
  }

  modulo.FS.writeFile(ARQUIVO_TEMP, texto);
  if (qc.carregar()) {
    desenharTabuleiro();
    mostrarTela("jogo");
    setStatus("✅  Jogo carregado com sucesso!");
  } else {
    mostrarTela(qc.tamanho() ? "jogo" : "menu");
    setStatus("❌  Erro ao carregar o arquivo.");
    alert("Não foi possível carregar esse arquivo de jogo.");
  }
});
