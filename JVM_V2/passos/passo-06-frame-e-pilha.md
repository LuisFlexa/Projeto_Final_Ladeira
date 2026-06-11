# Passo 6 — Frame e pilha de execução (`Frame`, `PilhaExecucao`)

**Tamanho:** M · **Depende de:** passos 1–5

## Objetivo
Modelar um **frame** de método (pilha de operandos, variáveis locais, `pc`,
atributo Code) e a **pilha de chamadas** da JVM.

## Origem (referência)
- `JVM/headers/Frame.hpp` + `JVM/src/Frame.cpp`
- `JVM/headers/Stack.hpp` + `JVM/src/Stack.cpp`

## Destino (JVM_V2)
| Novo arquivo | Classe | Papel |
|---|---|---|
| `include/frame.hpp` + `src/frame.cpp` | `Frame` | dois construtores (instância/estático), `pc`, `push/pop_operand_stack`, `get/set_local_variable`, `get_code`, localizar `Code`/`Exceptions` |
| `include/pilha_execucao.hpp` + `src/pilha_execucao.cpp` | `PilhaExecucao` | singleton: `empilhar_frame`, `frame_topo`, `desempilhar_frame`, `tamanho` |

## O que adaptar
- ⚠️ **Bug crítico da referência:** `Frame.hpp` inclui `"BasicTypes.hpp"`, que
  **não existe**. Trocar por `#include "tipos_runtime.hpp"`.
- `StaticClass`→`ClasseEstatica`, `InstanceClass`→`ClasseInstancia`,
  `Value`→`Valor`, `MethodArea`→`AreaMetodos`.
- `get_formatted_constant`→`formatar_constante`;
  `ClassFileUtils::compare_utf8_str`→`UtilClasse::utf8_igual_a`.
- ⚠️ Em `obterMethodNamed`, a referência chama `methodArea.get_class_by_name(...)`
  — usar o nome padronizado no passo 5 (`obter_classe`).
- `Stack` → `PilhaExecucao` (renomear para não colidir com `std::stack`; a
  referência tem o vício de `Stack& Stack = ...` que some com o rename).

## Lógica a preservar (Frame)
- Construtor copia `arguments` para `local_variables[0..n]`.
- `obterMethodNamed` sobe a hierarquia (super_class) procurando nome+descritor.
- `encontrarAttributes` acha `Code` e `Exceptions` nos atributos do método.
- `get_constant_pool` devolve a pool da `ClasseEstatica` resolvida.

## Critério de aceite
- Construir um `Frame` para o método `main` de um `examples/*.class` sem crash;
  `get_code(0)` aponta para o primeiro opcode; locals e operand stack operam.
- Build geral verde.

## Armadilhas
- `FRAME_MAX_SIZE`/StackOverflow está comentado na referência — decidir se ativa.
- `obterMethodNamed` seta `class_runtime` (efeito colateral) — manter, pois o
  Frame precisa saber em que classe o método foi achado (pool correta).
</content>
