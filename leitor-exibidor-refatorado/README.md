# Leitor-Exibidor + Executor de bytecodes JVM 8

Projeto final da disciplina de **Software Básico (UnB)**.

Implementa, em C++14, duas funcionalidades a partir de arquivos `.class`
do Java SE 1.0 até 1.8 (major version 45–52):

1. **Leitor/Exibidor** — analisa o arquivo `.class` e gera uma saída
   textual no estilo do jclasslib, mostrando todas as seções da classe
   (General Information, Constant Pool, Interfaces, Fields, Methods,
   Attributes) com os índices do pool de constantes e os valores que
   eles representam. Mnemônicos dos bytecodes e seus operandos
   (incluindo a constante referenciada) são impressos por método.
2. **Executor** — interpreta os bytecodes da classe (até 202 opcodes
   da JVM 8), suportando aritmética inteira, ponto flutuante, vetores,
   `invokespecial`/`invokestatic`/`invokevirtual`, controle de fluxo,
   classes/instâncias/campos, `String`, etc.

## Estrutura

```
leitor-exibidor-refatorado/
├── Makefile
├── README.md
├── examples/        # 21 arquivos .class de teste
├── include/         # cabeçalhos (.hpp)
│   ├── area_metodos.hpp        AreaMetodos (singleton)
│   ├── args.hpp                Parser de CLI
│   ├── arquivo_classe.hpp      Modelo do .class
│   ├── classe_estatica.hpp     Class object em runtime
│   ├── executor.hpp            Interpretador de bytecodes
│   ├── exibidor_classe.hpp     Saída textual jclasslib-like
│   ├── instancia_classe.hpp    Instância criada por `new`
│   ├── leitor_classe.hpp       Parser do formato .class
│   ├── objeto_base.hpp         Raiz da hierarquia de objetos
│   ├── objeto_str.hpp          java.lang.String simulada
│   ├── pilha_jvm.hpp           Pilha de quadros
│   ├── quadro.hpp              Frame de execução
│   ├── tipos_basicos.hpp       u1/u2/u4 + structs da spec
│   ├── util_classe.hpp         Funções utilitárias
│   └── vetor_jvm.hpp           Array Java simulado
└── src/             # implementações (.cpp)
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

### Modo Leitor/Exibidor

```
./bin -r=<caminho/Classe.class> -o=<saida.txt>
```

Exemplo:

```bash
./bin -r=examples/fatorial.class -o=fatorial_dump.txt
```

A saída em `fatorial_dump.txt` mostra todas as seções da classe.

### Modo Executor

```
./bin -e=<caminho/Classe.class>
```

Exemplo:

```bash
./bin -e=examples/fatorial.class
# 720
```

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

- Não há suporte real para a biblioteca padrão Java
  (`java.lang.System.out.println` é tratado como caso especial; chamar
  qualquer outra classe do JRE — ex. `java.lang.StringBuffer` — falha
  com mensagem de "classe não encontrada"). Os exemplos de teste foram
  escritos para usar apenas o subconjunto suportado.
- Atributo `StackMapTable` é apenas pulado, não interpretado.
- Não há GC: tudo o que é alocado vive até o fim do processo.
