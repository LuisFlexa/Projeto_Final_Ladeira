# Passo 10 — Testes e validação

**Tamanho:** M · **Depende de:** passos 1–9

## Objetivo
Garantir que a JVM_V2 (a) não regrediu no leitor/exibidor e (b) executa
corretamente os `examples/`.

## Regressão do leitor/exibidor
Para cada `.class` em `examples/`, rodar `-r` e comparar com `saidas/`:
```powershell
$env:Path = "C:\msys64\mingw64\bin;C:\msys64\usr\bin;" + $env:Path
foreach ($c in Get-ChildItem examples\*.class) {
    .\bin -r="examples\$($c.Name)" -o="tmp.txt"
    if (Compare-Object (Get-Content "saidas\$($c.BaseName).txt") (Get-Content "tmp.txt")) {
        "DIFERENTE: $($c.Name)"
    }
}
```
Adaptar do `JVM/run_tests.sh` (referência) se preferir Bash/MSYS2.

## Validação da execução
Para os exemplos executáveis, comparar a saída de `-e` com a saída do **`java`
real** quando disponível:
```
java -cp examples Sum        # referência
./bin -e=examples/Sum.class  # nossa JVM
```
Exemplos com `main` / aritmética / impressão: `Sum`, `soma_certo`,
`soma_errado`, `fatorial`, `fibonacci`, `FibonacciRec`, `vetor2`, `vetor_8`,
`tableswitch`, `lookupswitch`, `double_*`, `multi`, `method_test`,
`interface_test`. (Classes como `Carta`/`Jogador`/`Belote` podem depender de
várias classes — exigem `class_path` correto.)

## Critério de aceite
- 100% dos `examples` passam na regressão do leitor.
- Os exemplos executáveis batem com a saída do `java`.

## Ferramentas opcionais (do Makefile da referência)
- `make cppcheck` — análise estática.
- `make valgrind` — vazamentos (em Linux/MSYS2).

## Armadilhas
- Diferenças de fim de linha (CRLF×LF) podem gerar falso-positivo no diff —
  normalizar antes de comparar.
- Float/double: pequenas diferenças de formatação na impressão podem exigir
  tolerância na comparação.
</content>
