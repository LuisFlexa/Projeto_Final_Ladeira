# Passo 7 — Executor: esqueleto e laço de despacho

**Tamanho:** M · **Depende de:** passos 1–6

## Objetivo
Criar o `Executor` (= `Operations`) **sem** implementar ainda os 202 opcodes:
só a estrutura, a tabela de ponteiros para função, o laço principal e algumas
instruções triviais para validar o mecanismo ponta a ponta.

## Origem (referência)
- `JVM/headers/Operations.hpp` (declaração das 202 funções + `generic_function`)
- `JVM/src/Operations.cpp` linhas ~1–130 (`executeMethods`, `verifyMethod`,
  `populateMultiarray`, `initInstructions`, e exemplos: `nop`, `aconst_null`,
  `iconst_*`).

## Destino (JVM_V2)
`include/executor.hpp` + `src/executor.cpp` — classe `Executor` (singleton).

Elementos essenciais:
- `typedef void (Executor::*funcao_generica)();`
- `funcao_generica tabela_funcoes[202];` preenchida por `init_instrucoes()`.
- `void executar_metodos(ClasseEstatica*)` (= `executeMethods`): empilha o frame
  de `main`, depois o de `<clinit>` se existir, e roda o laço:
  ```cpp
  while (pilha.tamanho() > 0) {
      Frame* topo = pilha.frame_topo();
      u1* code = topo->get_code(topo->pc);
      (this->*tabela_funcoes[code[0]])();   // cada opcode avança o pc
  }
  ```
- `bool verifica_metodo(ClasseEstatica*, nome, descritor)` (= `verifyMethod`).
- `bool is_wide;` (estado para o opcode `wide`).

## O que adaptar
- `Operations` → `Executor`; nomes de método para PT.
- ⚠️ `#include "BasicTypes.hpp"` no `Operations.hpp` → `tipos_runtime.hpp`.
- ⚠️ `get_formatted_constant` → `formatar_constante`.
- ⚠️ **Designated initializers** (`Value v = {.type=..., .data={...}}`): extensão
  do g++. Decidir padrão (helper `faz_valor(...)` recomendado) — afeta TODO o
  passo 8.
- ⚠️ Em `executeMethods` a referência empilha `<clinit>` passando `arguments`
  (os args do main) — `<clinit>` é `()V`, então **não** deve receber args.
  Corrigir ao portar.

## Religar o clinit (fecha o passo 5)
Agora que `Executor`/`PilhaExecucao`/`Frame` existem, voltar em `AreaMetodos::
carregar_classe` e religar o empilhamento do frame `<clinit>` (o `// TODO` do
passo 5).

## Critério de aceite
- Com apenas alguns opcodes implementados (`nop`, `iconst_*`, `return`,
  `getstatic`+`invokevirtual` mínimos para `println`, OU simplesmente `bipush`+
  `ireturn`), executar um `.class` trivial sem crashar no laço.
- Build geral verde.

## Armadilhas
- Cada função de opcode é responsável por **avançar o `pc`** (a referência não
  centraliza isso). Esquecer = laço infinito.
- O laço termina quando a pilha esvazia (último `return`/`func_return`
  desempilha o frame).
</content>
