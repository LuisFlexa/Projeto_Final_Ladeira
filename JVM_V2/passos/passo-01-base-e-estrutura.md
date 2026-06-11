# Passo 1 — Base e estrutura (build verde)

**Tamanho:** P · **Depende de:** nada

## Objetivo
Ter o `JVM_V2/` compilando como uma cópia fiel do `leitor-exibidor`, com a
estrutura de pastas e o Makefile prontos para receber o motor de execução.
Ao final, `make` gera `bin`/`bin.exe` e o modo leitor funciona igual ao original.

## O que fazer
1. Copiar para `JVM_V2/`:
   - `include/` (todos os `.hpp`)
   - `src/` (todos os `.cpp`)
   - `examples/` (os `.class` de teste)
   - `saidas/` (saídas de referência para comparação de regressão)
   - `.clangd`, `.gitignore`
2. Criar o `Makefile` baseado no do `leitor-exibidor` (já correto: alvo `bin`,
   `-std=c++14 -Wall -Wextra`, lista de `FONTES`). **Não** copiar o Makefile da
   `JVM/` (está quebrado — `$(TARGET)` indefinido).
3. Adaptar o `README.md` para a JVM_V2 (mencionar que terá modo execução).

## Estrutura final
```
JVM_V2/
├── Makefile
├── README.md
├── PLANO.md
├── passos/
├── include/        # headers (cresce a cada passo)
├── src/            # fontes (cresce a cada passo)
├── examples/       # .class de teste
├── saidas/         # saídas de referência (regressão do leitor)
└── obj/            # gerado pelo build
```

## Critério de aceite
- `make` compila sem erros nem warnings novos.
- `./bin -r=examples/fatorial.class -o=out.txt` produz saída idêntica a
  `saidas/fatorial.txt` (e o mesmo para os demais `examples/`).

## Armadilhas
- Manter `-std=c++14` (não subir para C++17).
- No MSYS2/Windows o Makefile do leitor já força `TMP/TEMP` local — manter.
</content>
