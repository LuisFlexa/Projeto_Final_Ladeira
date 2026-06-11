# Passo 3 — Objetos de runtime (`Objeto`, `ObjetoString`, `Arranjo`)

**Tamanho:** M · **Depende de:** passo 2

## Objetivo
Criar a hierarquia de objetos que vivem no heap da JVM.

## Origem (referência)
- `JVM/headers/Object.hpp` + (não tem .cpp) — classe-base abstrata.
- `JVM/headers/StrObject.hpp` + `JVM/src/StrObject.cpp` — string imutável.
- `JVM/headers/Array.hpp` + `JVM/src/Array.cpp` — array dinâmico (vector<Value>).

## Destino (JVM_V2)
| Novo arquivo | Classe | Conteúdo |
|---|---|---|
| `include/objeto.hpp` | `Objeto` | base abstrata, `virtual TipoObjeto tipo_objeto() = 0;` |
| `include/objeto_string.hpp` + `src/objeto_string.cpp` | `ObjetoString` | `get_str/set_str`, guarda `std::string` |
| `include/objeto_arranjo.hpp` + `src/objeto_arranjo.cpp` | `Arranjo` | `push_value`, `get_value`, `change_value`, `get_size`, `remove_*` sobre `vector<Valor>` |

## O que adaptar
- `Object` → `Objeto`; `object_type()` → `tipo_objeto()`.
- `StrObject` → `ObjetoString`; `Array` → `Arranjo`.
- `Value` → `Valor`, `ValueType` → `TipoValor` (do passo 2).
- Incluir `tipos_runtime.hpp` em vez de `BaseType.hpp`.
- Atualizar o forward declare `class Objeto;` no `tipos_runtime.hpp` para casar.

## Critério de aceite
- Os três compilam e linkam num `.cpp` de teste que cria um `ObjetoString` e um
  `Arranjo`, insere/lê valores.
- Build geral verde.

## Armadilhas
- `Arranjo` guarda `TipoValor array_type` (tipo dos elementos) — necessário para
  `newarray`/`anewarray` no passo 8.
- `remove_first`/`remove_at` da referência usam `vector::erase` (O(n)); aceitável.
</content>
