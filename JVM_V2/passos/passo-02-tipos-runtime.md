# Passo 2 — Tipos de runtime (`tipos_runtime.hpp`)

**Tamanho:** P · **Depende de:** passo 1

## Objetivo
Criar o modelo de **valor em tempo de execução** — o que falta no
`leitor-exibidor` (que só tem os structs do `.class` em `tipos_basicos.hpp`).

## Origem (referência)
`JVM/headers/BaseType.hpp`, **apenas** o trecho de runtime:
- `enum ObjectType { CLASS_INSTANCE, STRING_INSTANCE, ARRAY }`
- `enum ValueType { BOOLEAN, BYTE, CHAR, SHORT, INT, FLOAT, LONG, DOUBLE, RETURN_ADDR, REFERENCE, PADDING }`
- `struct Value { ValueType print_type; ValueType type; union {...} data; }`

> A parte de `BaseType.hpp` com os structs do `.class` (ConstantPool, atributos,
> FieldInfo…) **já existe** em `tipos_basicos.hpp` — não duplicar.

## Destino (JVM_V2)
Novo arquivo `include/tipos_runtime.hpp`:
- `TipoObjeto` (= `ObjectType`): `INSTANCIA_CLASSE`, `INSTANCIA_STRING`, `ARRANJO`
- `TipoValor` (= `ValueType`): mesmos membros, em PT se quiser (`BOOLEANO`, `BYTE`,
  `CHAR`, `SHORT`, `INT`, `FLOAT`, `LONG`, `DOUBLE`, `ENDERECO_RETORNO`,
  `REFERENCIA`, `PADDING`)
- `struct Valor` (= `Value`) com `tipo_print`, `tipo` e a `union dados` (incluir
  `Objeto* objeto;` — forward declare `class Objeto;`).

## O que adaptar
- `#include <stdint.h>` (para `int8_t`, `float`, etc.).
- Forward declaration `class Objeto;` (definido no passo 3).
- Decidir convenção de inicialização de `Valor`: a referência usa **designated
  initializers** (`.type = ..., .data = {...}`) — isso é **extensão do g++** em
  C++14 (compila, mas não é padrão). Recomendação: criar um helper inline
  `Valor faz_valor(TipoValor t, ...)` ou usar atribuição campo a campo, para
  manter portabilidade e legibilidade.

## Critério de aceite
- `tipos_runtime.hpp` compila isolado (incluir em um `.cpp` de teste).
- Build geral continua verde.

## Armadilhas
- Não incluir `tipos_runtime.hpp` dentro de `tipos_basicos.hpp` (manter camadas
  separadas; quem precisa de runtime inclui o novo header).
</content>
