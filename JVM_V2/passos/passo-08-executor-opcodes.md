# Passo 8 — Os 202 opcodes (o grande)

**Tamanho:** GG (~70% do esforço) · **Depende de:** passo 7

## Objetivo
Portar todas as 202 instruções de `JVM/src/Operations.cpp` para `executor.cpp`,
na convenção da JVM_V2. Por ser enorme (4.505 linhas), **divida em 6 sub-passos**
por categoria — cada um compila e pode ser commitado isoladamente.

## Regras gerais (valem para todo opcode)
- Cada função: pega `Frame* topo = pilha.frame_topo();`, lê operandos via
  `topo->get_code(topo->pc)`, manipula a pilha de operandos / locals, e **avança
  o `pc`** pelo tamanho da instrução. **Nunca esquecer o `pc++`.**
- Substituições recorrentes: `Value`→`Valor`, `ValueType`→`TipoValor`,
  `Stack`→`PilhaExecucao`, `get_formatted_constant`→`formatar_constante`,
  designated initializers → helper `faz_valor(...)` (ver passo 2/7).
- A tabela `tabela_mnemonicos` em `exibidor_classe.hpp` lista os opcodes na ordem
  do índice — útil para conferir cobertura.

## Sub-passos (faixas em `JVM/src/Operations.cpp`)

### 8a — Constantes, cargas e armazenamentos · linhas ~78–1624
`nop`, `aconst_null`, `iconst_*`, `lconst_*`, `fconst_*`, `dconst_*`, `bipush`
(L299), `sipush`, `ldc`/`ldc_w`/`ldc2_w`, `*load`(L483)/`*load_0..3`/`*aload`,
`*store`(L1051)/`*store_0..3`/`*astore`.
→ Exercita `ldc` (constant pool), arrays (`*aload`/`*astore`) e locals.

### 8b — Pilha e aritmética · linhas ~1625–2332
`pop`(L1625), `pop2`, `dup`/`dup_x1`/`dup_x2`/`dup2`/`dup2_x1`/`dup2_x2`, `swap`;
`iadd`(L1760)…`drem`, `*neg`, shifts (`ishl`…`lushr`), `iand`…`lxor`,
`iinc`(L2307).

### 8c — Conversões e comparações · linhas ~2333–2712
`i2l`(L2333)…`i2s` (todas as conversões numéricas);
`lcmp`(L2584), `fcmpl`/`fcmpg`, `dcmpl`/`dcmpg`.

### 8d — Controle de fluxo · linhas ~2713–3168
`ifeq`(L2713)…`ifle`, `if_icmpeq`(L2815)…`if_icmple`, `if_acmpeq`/`if_acmpne`;
`func_goto`(L2967), `jsr`, `ret`; `tableswitch`(L3014), `lookupswitch`(L3058);
`ireturn`(L3096)…`areturn`, `func_return`(L3163).
→ ⚠️ `goto`/`return` são palavras-chave de C++: a referência usa `func_goto`/
`func_return`. Manter esse padrão (ou `op_goto`/`op_return`).
→ Os `*return` **desempilham o frame** e propagam o valor de retorno ao frame
chamador — ponto crítico para o laço terminar.

### 8e — Campos e invocações · linhas ~3169–3817
`getstatic`(L3169), `putstatic`, `getfield`, `putfield`;
`invokevirtual`(L3423), `invokespecial`, `invokestatic`, `invokeinterface`.
→ Mais complexo: resolve `Fieldref`/`Methodref` no pool, carrega a classe via
`AreaMetodos`, monta `vector<Valor>` de argumentos a partir do descritor, cria
novo `Frame`. Aqui mora o `System.out.println` (tratamento especial das classes
nativas `java/lang/...`). **Vale um commit só para esta categoria.**

### 8f — Objetos, arrays e diversos · linhas ~3818–4505
`func_new`(L3818), `newarray`(L3849), `anewarray`, `arraylength`(L4001),
`athrow`, `checkcast`, `instanceof`, `monitorenter`/`monitorexit` (geralmente
no-op nesta JVM), `wide`(L4154), `multianewarray`(L4162, usa `populateMultiarray`),
`ifnull`(L4236)/`ifnonnull`, `goto_w`(L4270)/`jsr_w`.

## Critério de aceite (por sub-passo)
- Compila e linka; o array `tabela_funcoes[]` tem o índice correspondente
  apontando para a função certa em `init_instrucoes()`.
- Ao terminar 8a–8d: executar um `.class` aritmético simples (ex.: `Sum`,
  `soma_certo`) e bater o resultado.
- Ao terminar 8e: `println` imprime corretamente.
- Ao terminar 8f: `fibonacci`, `fatorial`, `vetor*`, `tableswitch`/`lookupswitch`.

## Armadilhas
- **Categorias largas (long/double) ocupam 2 slots** na pilha/locals em alguns
  pontos — conferir como a referência trata (`PADDING`).
- Sinais e overflow: usar os tipos exatos (`int32_t`, `int64_t`) do `Valor`.
- `invoke*`: o `this`/`objectref` é desempilhado **antes** dos argumentos (ordem).
</content>
