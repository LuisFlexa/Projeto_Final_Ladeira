# Leitor-Exibidor de bytecodes JVM 8

Projeto final da disciplina de **Software Básico (UnB)**.

Implementa, em C++14, um **leitor/exibidor** de arquivos `.class` do
Java SE 1.0 até 1.8 (major version 45–52). O programa analisa o
arquivo `.class` e gera uma saída textual no estilo do jclasslib,
mostrando todas as seções da classe (General Information, Constant
Pool, Interfaces, Fields, Methods, Attributes) com os índices do pool
de constantes e os valores que eles representam. Mnemônicos dos
bytecodes e seus operandos (incluindo a constante referenciada) são
impressos por método.

## Estrutura

```
leitor-exibidor-refatorado/
├── Makefile
├── README.md
├── examples/                       # arquivos .class de teste
├── include/                        # cabeçalhos
│   ├── args.hpp                    Parser de CLI
│   ├── arquivo_classe.hpp          Modelo do .class
│   ├── exibidor_classe.hpp         Saída textual jclasslib-like
│   ├── leitor_classe.hpp           Parser do formato .class
│   ├── tipos_basicos.hpp           u1/u2/u4 + structs da spec
│   └── util_classe.hpp             Funções utilitárias
└── src/                            # implementações
    ├── args.cpp
    ├── exibidor_classe.cpp
    ├── leitor_classe.cpp
    ├── main.cpp
    └── util_classe.cpp
```

## Como compilar

Requer **g++** com suporte a C++14 (testado com **gcc 15.2 do MSYS2**
em Windows, mas funciona em qualquer sistema POSIX).

```bash
make
```

Gera o executável `bin` (ou `bin.exe` no Windows). Para build com
símbolos de depuração:

```bash
make debug
```

Para limpar:

```bash
make clean
```

## Como rodar

```
./bin -r=<caminho/Classe.class> -o=<saida.txt>
```

Exemplo:

```bash
./bin -r=examples/fatorial.class -o=fatorial_dump.txt
```

A saída em `fatorial_dump.txt` mostra todas as seções da classe no
formato jclasslib: índices do pool de constantes com a constante
representada entre `<...>`, mnemônicos do bytecode com seus operandos
resolvidos.

## Opções de compilação

O `Makefile` usa por padrão:

| flag                     | função                                       |
|--------------------------|----------------------------------------------|
| `-O0`                    | sem otimização (build rápido, debug fácil)   |
| `-std=c++14`             | exige C++14                                  |
| `-Wall -Wextra`          | warnings agressivos                          |
| `-Iinclude`              | path dos headers                             |
| `-fdiagnostics-color`    | erros do gcc colorizados                     |

`make debug` adiciona `-g`.

## Limitações conhecidas

- Atributo `StackMapTable` é apenas pulado, não interpretado em
  detalhe (os bytes são consumidos para manter o ponteiro do parser
  alinhado, mas a tabela não é exibida).
