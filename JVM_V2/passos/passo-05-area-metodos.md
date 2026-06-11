# Passo 5 — Área de métodos (`AreaMetodos`)

**Tamanho:** M · **Depende de:** passos 1–4

## Objetivo
Singleton que **carrega e faz cache** de classes a partir do `class_path`,
evitando recarregar o mesmo `.class`.

## Origem (referência)
`JVM/headers/MethodArea.hpp` + `JVM/src/MethodArea.cpp`

## Destino (JVM_V2)
`include/area_metodos.hpp` + `src/area_metodos.cpp` — classe `AreaMetodos`
(singleton, igual ao padrão de `LeitorClasse`/`Stack`).

API:
- `ClasseEstatica* carregar_classe(const string& nome)` (= `load_class`)
- `ClasseEstatica* obter_classe(const string& nome)` (= `get_class`)
- membro `string class_path`
- privado: `bool inserir_classe(ClasseEstatica*)`, `map<string,ClasseEstatica*> classes`

## O que adaptar
- `StaticClass` → `ClasseEstatica`; `ClassFile` → `ArquivoClasse`.
- `ClassFileReader::get_instance().read_class_file(fp)` → `LeitorClasse::instancia().ler_arquivo(fp)`.
- `get_formatted_constant` → `formatar_constante` (usado em `inserir_classe` para
  a chave = nome de `this_class`).
- ⚠️ **Bug da referência:** o header declara `get_class`, mas `Frame.cpp` chama
  `get_class_by_name`. Padronizar **um** nome (`obter_classe`) e usar em todo lugar.

## Dependência circular com o Executor (atenção)
`carregar_classe` empilha o frame de `<clinit>` se ele existir, o que usa
`Executor` (`verifyMethod`) e `PilhaExecucao`/`Frame`. Esses ainda não existem nos
passos 5/6. **Solução:** nesta etapa, implementar `carregar_classe` **sem** a parte
do clinit (deixar `// TODO clinit — passo 7`). Religar o clinit ao final do passo 7.

## Critério de aceite
- `carregar_classe("examples/Sum")` retorna uma `ClasseEstatica` válida e cacheia;
  segunda chamada retorna o mesmo ponteiro.
- Build geral verde.

## Armadilhas
- Normalização do nome: a referência concatena `class_path + nome + ".class"` só
  se ainda não terminar em `.class`. Preservar.
- Definir o `class_path` (diretório dos `.class`) — vem do `main` (passo 9).
</content>
