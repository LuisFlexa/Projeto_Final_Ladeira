# JVM_V2 — leitor-exibidor + motor de execução

Projeto final de **Software Básico (UnB)**. Constrói uma JVM completa em C++14:

- **Modo leitor (`-r`)** — lê e exibe um `.class` (Java SE 1.0–1.8) no estilo
  jclasslib. **Já funciona** (base portada do `leitor-exibidor/`).
- **Modo execução (`-e`)** — interpreta os bytecodes. **Em construção**, sendo
  portado do projeto de referência `JVM/` para a convenção em português.

> Estado atual: **Passo 1 concluído** — a base do leitor-exibidor já compila
> dentro da `JVM_V2/`. O motor de execução entra nos passos seguintes.

## Por onde começar
1. Leia o [`PLANO.md`](PLANO.md) — comparação dos dois projetos, arquitetura,
   mapa de nomes e roadmap.
2. Siga os guias em [`passos/`](passos/), um por vez. Cada passo compila ao final
   e tem critério de aceite.

## Como compilar e rodar

Toolchain MSYS2 nesta máquina. No PowerShell, antes de compilar:

```powershell
$env:Path = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;" + $env:Path
make            # gera bin.exe
```

Modo leitor (funcional):

```bash
./bin -r=examples/fatorial.class -o=out.txt
```

Outros alvos: `make debug` (com `-g`), `make clean`.

## Estrutura

```
JVM_V2/
├── Makefile
├── README.md
├── PLANO.md
├── passos/         # guias do roadmap (1 a 10)
├── include/        # cabeçalhos (cresce a cada passo)
├── src/            # fontes (cresce a cada passo)
├── examples/       # .class de teste
├── saidas/         # saídas de referência (regressão do modo leitor)
└── obj/            # gerado pelo build
```

## Checklist de progresso
- [x] **Passo 1** — Base e estrutura (build verde) · [guia](passos/passo-01-base-e-estrutura.md)
- [ ] **Passo 2** — Tipos de runtime (`Valor`/`TipoValor`/`TipoObjeto`) · [guia](passos/passo-02-tipos-runtime.md)
- [ ] **Passo 3** — Objetos (`Objeto`/`ObjetoString`/`Arranjo`) · [guia](passos/passo-03-objetos-runtime.md)
- [ ] **Passo 4** — Classes runtime (`ClasseEstatica`/`ClasseInstancia`) · [guia](passos/passo-04-classes-runtime.md)
- [ ] **Passo 5** — `AreaMetodos` · [guia](passos/passo-05-area-metodos.md)
- [ ] **Passo 6** — `Frame` + `PilhaExecucao` · [guia](passos/passo-06-frame-e-pilha.md)
- [ ] **Passo 7** — `Executor`: esqueleto + laço · [guia](passos/passo-07-executor-estrutura.md)
- [ ] **Passo 8** — 202 opcodes (8a–8f) · [guia](passos/passo-08-executor-opcodes.md)
  - [ ] 8a constantes/cargas/stores
  - [ ] 8b pilha/aritmética
  - [ ] 8c conversões/comparações
  - [ ] 8d controle de fluxo
  - [ ] 8e campos/invocações
  - [ ] 8f objetos/arrays/diversos
- [ ] **Passo 9** — `main` + `args` (leitor × execução) · [guia](passos/passo-09-integracao-main.md)
- [ ] **Passo 10** — Testes e validação · [guia](passos/passo-10-testes.md)
