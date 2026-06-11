# Passo 4 — Classes em runtime (`ClasseEstatica`, `ClasseInstancia`)

**Tamanho:** M · **Depende de:** passos 2, 3

## Objetivo
Representar uma classe **carregada** (com seus campos estáticos) e uma
**instância** de objeto (com seus campos de instância).

## Origem (referência)
- `JVM/headers/StaticClass.hpp` + `JVM/src/StaticClass.cpp`
- `JVM/headers/InstanceClass.hpp` + `JVM/src/InstanceClass.cpp`

## Destino (JVM_V2)
| Novo arquivo | Classe | Papel |
|---|---|---|
| `include/classe_estatica.hpp` + `src/classe_estatica.cpp` | `ClasseEstatica` | guarda `ArquivoClasse*`, `map<string,Valor>` de campos estáticos; inicializa estáticos por descritor |
| `include/classe_instancia.hpp` + `src/classe_instancia.cpp` | `ClasseInstancia : Objeto` | guarda ponteiro para `ClasseEstatica` + `map<string,Valor>` de campos de instância |

## O que adaptar (importante)
- **`ClassFile*` → `ArquivoClasse*`** no construtor e em `get_class_file()`
  (renomear p.ex. `get_arquivo_classe()`).
- **`get_formatted_constant(pool, idx)` → `formatar_constante(pool, idx)`**
  (já existe em `exibidor_classe.hpp`). O `StaticClass.cpp` usa essa função para
  ler nome/descritor dos campos.
- `Value` → `Valor`, `ValueType` → `TipoValor`.
- `InstanceClass : public Object` → `ClasseInstancia : public Objeto`;
  `object_type()` retorna `TipoObjeto::INSTANCIA_CLASSE`.
- O `StaticClass::initialize_file(argv)` da referência tira o `.class` do nome —
  reavaliar onde isso vive (talvez melhor em `AreaMetodos`/`main`, passos 5/9).

## Inicialização de campos estáticos (lógica a preservar)
O construtor varre `fields`, e para cada `ACC_STATIC` cria um `Valor` zerado com
o `TipoValor` derivado do **primeiro char do descritor** (`B C D F I J S Z` →
primitivos; senão `REFERENCIA`). Manter esse switch.

## Critério de aceite
- Carregar um `ArquivoClasse` (via `LeitorClasse`) e construir uma
  `ClasseEstatica` sem crash; `check_field`/`get_value`/`insert_value` funcionam.
- Build geral verde.

## Armadilhas
- A `ClasseEstatica` ainda **não** executa `<clinit>` aqui — só zera os campos.
  A execução do clinit entra no passo 5 (AreaMetodos) + 7/8 (Executor).
- Cuidado com ciclo de include: `classe_instancia.hpp` inclui `classe_estatica.hpp`
  e `objeto.hpp`; `classe_estatica.hpp` inclui `arquivo_classe.hpp`. Sem ciclos.
</content>
