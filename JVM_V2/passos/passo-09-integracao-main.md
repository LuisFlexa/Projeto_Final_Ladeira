# Passo 9 — Integração: `main` e `args` (modo leitor × execução)

**Tamanho:** M · **Depende de:** passos 1–8

## Objetivo
Ligar tudo: um único binário com **dois modos** — leitor/exibidor (`-r`) e
execução (`-e`).

## Origem (referência)
- `JVM/headers/ArgParser.hpp` + `JVM/src/ArgParser.cpp` (já tem o flag `execute`
  e o modo `-e=`).
- `JVM/src/main.cpp` — ⚠️ **inútil** (só `printf("Hello World")`). O `main` de
  verdade precisa ser escrito do zero, juntando leitor + executor.

## Destino (JVM_V2)
- Estender `include/args.hpp` + `src/args.cpp`: adicionar `bool execute;` ao
  `Parameters` e o parsing de `-e=arquivo.class` (espelhar a referência, mas em
  PT). Atualizar `validar_parametros` para a regra: precisa de `-r`+`-o` (leitura)
  **ou** `-e` (execução).
- Reescrever `src/main.cpp`:
  ```
  params = analisar_args(argc, argv)
  if (!validar_parametros(&params)) return 1;
  abrir arquivo .class
  arq = LeitorClasse::instancia().ler_arquivo(fp)
  if (params.execute) {
      AreaMetodos.class_path = diretório do .class
      ClasseEstatica* c = new ClasseEstatica(arq)   // ou via AreaMetodos.carregar_classe
      Executor::instancia().executar_metodos(c)
  } else {
      saida = fopen(params.output_file_path, "w")
      escrever_arquivo(arq, saida)                  // modo leitor (já existe)
  }
  ```

## O que adaptar
- `parse_args`/`validate_parameters` → `analisar_args`/`validar_parametros`
  (nomes já usados no leitor) + campo `execute`.
- Definir `class_path` a partir do diretório do `.class` passado em `-e=` (para a
  `AreaMetodos` achar as outras classes do programa).
- Conferir nome da classe × nome do arquivo com `UtilClasse::conferir_nome_classe`
  (opcional, já existe).

## Critério de aceite
- `./bin -r=examples/fatorial.class -o=out.txt` → idêntico a `saidas/fatorial.txt`
  (regressão do leitor intacta).
- `./bin -e=examples/Sum.class` → executa e produz a saída esperada do programa.

## Armadilhas
- O modo execução normalmente **não** usa `-o` (saída vai pro stdout do programa
  Java executado).
- Garantir que `validar_parametros` não exija `-o` quando `execute == true`.
</content>
