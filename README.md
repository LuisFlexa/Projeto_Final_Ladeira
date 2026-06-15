# JVM — Leitor/Exibidor e Motor de Execução de bytecode Java

Projeto final de **Software Básico (UnB)**. Implementa, em **C++14**, uma
Máquina Virtual Java capaz de:

- **Modo leitor (`-r`)** — lê um arquivo `.class` (Java SE 1.0–1.8) e exibe seu
  conteúdo (constant pool, campos, métodos, atributos) em formato textual no
  estilo *jclasslib*.
- **Modo execução (`-e`)** — interpreta os bytecodes do `.class`, executando o
  método `main` do programa.

## Estrutura

```
.
├── Makefile        # build (g++ -std=c++14)
├── include/        # cabeçalhos (.hpp)
├── src/            # implementação (.cpp)
└── examples/       # arquivos .class de teste
```

O fluxo de execução do programa (e sua relação com a arquitetura da JVM dos
slides) está documentado em [`FLUXO.md`](FLUXO.md).

## Como compilar

Requer um `g++` com suporte a C++14. Na máquina de desenvolvimento foi usado o
toolchain **MSYS2/mingw64**; no PowerShell, ajuste o `PATH` antes de compilar:

```powershell
$env:Path = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;" + $env:Path
make
```

Em Linux/macOS basta `make`. O binário gerado é `bin` (ou `bin.exe` no Windows).

## Como executar

### Modo leitor (`-r`) — exige `-o` (arquivo de saída)

```bash
./bin -r=examples/Sum.class -o=saida.txt
```

Lê `Sum.class` e grava o relatório textual em `saida.txt`.

### Modo execução (`-e`)

```bash
./bin -e=examples/Sum.class
```

Interpreta os bytecodes e imprime a saída do programa no terminal. Exemplos:

| Arquivo                  | Saída |
|--------------------------|-------|
| `Sum.class`              | `55` |
| `fatorial.class`         | `720` |
| `FibonacciRec.class`     | série de Fibonacci |
| `Belote.class`           | simulação de um jogo de cartas (usa `Carta`/`Jogador`) |

> Para programas com várias classes (ex.: `Belote`, que depende de `Carta` e
> `Jogador`), todas as classes precisam estar no mesmo diretório do `.class`
> passado em `-e=` — a JVM as carrega sob demanda a partir daí.

## Limitações conhecidas

- Subconjunto da biblioteca padrão: `System.out.print/println`, `String` e
  `Object` são tratados nativamente; classes como `StringBuffer` não são
  suportadas.
- `invokedynamic` não é implementado (requer *bootstrap methods*).
