# Plano de construção da JVM_V2

> Documento mestre. Aqui está a comparação entre o que **já está pronto**
> (`leitor-exibidor/`) e o que **ainda falta entregar** (o motor de execução,
> que existe de forma incompleta em `JVM/`), mais o roadmap incremental.
>
> **Nada foi implementado ainda.** Este diretório contém só o plano. Cada passo
> tem um guia próprio em [`passos/`](passos/).

---

## 1. Ponto de partida: os dois projetos

### `leitor-exibidor/` — **pronto, limpo, validado** (~2.400 linhas, C++14, PT)
Lê e exibe arquivos `.class` (Java SE 1.0–1.8) no estilo jclasslib. **Não executa
bytecode.** Convenção consistente em português.

| Arquivo | Papel |
|---|---|
| `tipos_basicos.hpp` | `u1/u2/u4` + structs do formato `.class` (constant pool, atributos…) |
| `arquivo_classe.hpp` | `ArquivoClasse` — modelo em memória do `.class` |
| `leitor_classe.{hpp,cpp}` | `LeitorClasse` (singleton) — parser binário |
| `exibidor_classe.{hpp,cpp}` | dump textual estilo jclasslib + `formatar_constante`, `flags_acesso_str`, tabela de mnemônicos |
| `util_classe.{hpp,cpp}` | `UtilClasse` — helpers (versão, utf8, indentação…) |
| `args.{hpp,cpp}` | parser de CLI (`-r=…`, `-o=…`) |
| `main.cpp` | entrada |

### `JVM/` — **referência incompleta e quebrada** (~7.700 linhas, inglês/PT misto)
Tem tudo o que o leitor-exibidor tem **+ o motor de execução**. Mas:

- ⚠️ `src/main.cpp` é só `printf("Hello World")` — **nunca foi integrado**.
- ⚠️ `Makefile` usa `$(TARGET)` que **nunca é definido** — não builda.
- ⚠️ `Frame.hpp` e `Operations.hpp` incluem `"BasicTypes.hpp"`, mas o arquivo
  chama-se `BaseType.hpp` — **não compila como está**.
- ⚠️ Mistura inglês e português; runtime construído sobre `ClassFile` (a versão
  inglesa do `ArquivoClasse`).

**A referência serve como fonte da lógica de execução, não como código a copiar
cru.** Vamos portar a lógica para a convenção do `leitor-exibidor`.

---

## 2. O que falta: o motor de execução

Comparando as duas pastas, o que o `leitor-exibidor` **não tem** e precisa ser
portado de `JVM/`:

| Referência (`JVM/headers` e `src`) | Linhas | Função |
|---|---|---|
| `BaseType.hpp` (parte `Value`/`ValueType`/`ObjectType`) | — | tipos de valor em runtime |
| `Object.hpp` | 12 | classe-base abstrata dos objetos |
| `StrObject.{hpp,cpp}` | 70 | objeto `String` |
| `Array.{hpp,cpp}` | 143 | objeto array |
| `StaticClass.{hpp,cpp}` | 152 | classe carregada + campos estáticos + `<clinit>` |
| `InstanceClass.{hpp,cpp}` | 166 | instância de objeto + campos de instância |
| `MethodArea.{hpp,cpp}` | 128 | carregamento/cache de classes (singleton) |
| `Frame.{hpp,cpp}` | 262 | frame: pilha de operandos, variáveis locais, pc |
| `Stack.{hpp,cpp}` | 108 | pilha de chamadas da JVM (singleton) |
| `Operations.{hpp,cpp}` | 4.780 | **interpretador dos 202 opcodes** |
| `main.cpp` + `ArgParser` | — | integração: modo leitor × modo execução |

> Reaproveitaremos o leitor/exibidor inteiro: a parte de leitura do `.class` da
> referência (`ClassFileReader`/`ClassFileViewer`) é **equivalente** ao que já
> temos, então não a reescrevemos.

---

## 3. Convenção de nomes (referência → JVM_V2)

Lock-in dos nomes em português, seguindo o estilo do `leitor-exibidor`:

| Referência | JVM_V2 (arquivo) | Tipo/classe |
|---|---|---|
| `Value` | `tipos_runtime.hpp` | `Valor` |
| `ValueType` | `tipos_runtime.hpp` | `TipoValor` |
| `ObjectType` | `tipos_runtime.hpp` | `TipoObjeto` |
| `Object` | `objeto.hpp` | `Objeto` |
| `StrObject` | `objeto_string.{hpp,cpp}` | `ObjetoString` |
| `Array` | `objeto_arranjo.{hpp,cpp}` | `Arranjo` |
| `StaticClass` | `classe_estatica.{hpp,cpp}` | `ClasseEstatica` |
| `InstanceClass` | `classe_instancia.{hpp,cpp}` | `ClasseInstancia` |
| `MethodArea` | `area_metodos.{hpp,cpp}` | `AreaMetodos` |
| `Frame` | `frame.{hpp,cpp}` | `Frame` |
| `Stack` | `pilha_execucao.{hpp,cpp}` | `PilhaExecucao` |
| `Operations` | `executor.{hpp,cpp}` | `Executor` |
| `ClassFile` | `arquivo_classe.hpp` | `ArquivoClasse` *(já existe)* |
| `ClassFileReader` | `leitor_classe.*` | `LeitorClasse` *(já existe)* |
| `ClassFileViewer` | `exibidor_classe.*` | *(já existe)* |
| `ClassFileUtils` | `util_classe.*` | `UtilClasse` *(já existe)* |
| `ArgParser` | `args.*` | *(já existe — estender)* |

### Adaptações recorrentes ao portar
1. **`ClassFile*` → `ArquivoClasse*`** (layout idêntico; o runtime passa a usar o
   modelo que o `LeitorClasse` já produz).
2. **`get_formatted_constant(pool, idx)` → `formatar_constante(pool, idx)`** (já
   existe em `exibidor_classe.hpp`).
3. **`#include "BasicTypes.hpp"` → `#include "tipos_runtime.hpp"`** (corrige o bug
   da referência).
4. **`using namespace std;`** — o leitor-exibidor evita; manter o estilo dele
   (qualificar `std::`) é opcional, mas recomendado por consistência.
5. **Mensagens/asserts** podem ficar em PT.

---

## 4. Arquitetura final (camadas)

```
                 main  (args: -r leitor/exibidor | -e execução)
                   │
   ┌───────────────┼────────────────────────────┐
   │ LEITURA       │ EXIBIÇÃO                     │ EXECUÇÃO (novo)
   │ LeitorClasse  │ ExibidorClasse               │ Executor
   │   → ArquivoClasse  ← formatar_constante       │   usa PilhaExecucao(Frame*)
   └───────────────┴────────────────────────────┘        │
                                                    AreaMetodos → ClasseEstatica
                                                          │
                                          ClasseInstancia / Arranjo / ObjetoString
                                                          │
                                          tipos_runtime (Valor/TipoValor/TipoObjeto)
                                                          │
                                          tipos_basicos (structs do .class) [base comum]
```

Ordem de dependência (de baixo para cima): `tipos_basicos` → `tipos_runtime` →
`objeto`/`objeto_string`/`objeto_arranjo` → `classe_estatica`/`classe_instancia`
→ `area_metodos` → `frame` → `pilha_execucao` → `executor` → `main`.

**Esta é exatamente a ordem dos passos.**

---

## 5. Roadmap incremental

Cada passo é **independentemente compilável** (acrescenta arquivos sem quebrar o
que já funciona) e tem critério de aceite. Detalhes em `passos/`.

| Passo | Guia | Escopo | Tamanho |
|---|---|---|---|
| 1 | [passo-01](passos/passo-01-base-e-estrutura.md) | Base: copiar leitor-exibidor para JVM_V2, Makefile, estrutura, build verde | P |
| 2 | [passo-02](passos/passo-02-tipos-runtime.md) | `tipos_runtime.hpp` — `Valor`, `TipoValor`, `TipoObjeto` | P |
| 3 | [passo-03](passos/passo-03-objetos-runtime.md) | `Objeto`, `ObjetoString`, `Arranjo` | M |
| 4 | [passo-04](passos/passo-04-classes-runtime.md) | `ClasseEstatica` + `ClasseInstancia` | M |
| 5 | [passo-05](passos/passo-05-area-metodos.md) | `AreaMetodos` (carregamento/cache) | M |
| 6 | [passo-06](passos/passo-06-frame-e-pilha.md) | `Frame` + `PilhaExecucao` | M |
| 7 | [passo-07](passos/passo-07-executor-estrutura.md) | `Executor`: esqueleto, tabela de despacho, laço | M |
| 8 | [passo-08](passos/passo-08-executor-opcodes.md) | Os 202 opcodes (subdividido em 8a–8f) | **GG** |
| 9 | [passo-09](passos/passo-09-integracao-main.md) | `main` + `args`: modo leitor × execução | M |
| 10 | [passo-10](passos/passo-10-testes.md) | Testes e validação com os `examples/` | M |

P = pequeno, M = médio, GG = grande (o passo 8 é ~70% do esforço).

---

## 6. Como buildar e testar (toolchain)

Toolchain detectado nesta máquina (MSYS2):
- `g++` → `C:\msys64\mingw64\bin\g++.exe`
- `make` → `C:\msys64\usr\bin\make.exe`

No PowerShell, antes de compilar:
```powershell
$env:Path = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;" + $env:Path
make            # gera bin.exe
```

**Estratégia de teste:** o `leitor-exibidor` já tem saídas de referência em
`saidas/`. A cada passo até o 7, o modo leitor (`-r`) deve continuar produzindo
saída idêntica. A partir do passo 8/9, comparamos a execução (`-e`) dos
`examples/` com a saída do `java` real quando possível (ver passo-10).

---

## 7. Convenções cross-cutting (valem para todos os passos)

- **C++14** (não usar recursos de C++17+), `-Wall -Wextra`, como no Makefile atual.
- **Comentários e nomes em português**, seguindo o tom do `leitor-exibidor`.
- **Sem quebrar build anterior:** cada passo compila ao final.
- **Gerência de memória:** o `LeitorClasse` aloca o `ArquivoClasse` com `new[]`
  bruto. Por enquanto, seguir o mesmo padrão (não introduzir smart pointers
  isoladamente); padronizar fica como tarefa final opcional.
- **Commits:** um commit por passo (ou sub-passo do 8), mensagem em PT.
</content>
</invoke>
