# -*- coding: utf-8 -*-
"""
portar_executor.py

Gera JVM_V2/src/executor.cpp a partir de JVM/src/Operations.cpp (referencia).
Faz duas transformacoes:
  1. converte os designated initializers (Value v = {.type=..., .data={...}})
     em chamadas aos helpers faz_valor_* de tipos_runtime.hpp;
  2. mapeia os nomes da referencia (ingles) para a convencao do projeto (PT).

Mantem nossas proprias executar_metodos / verifica_metodo / instrucao_nao_
implementada (escritas a mao), e extrai da referencia apenas popula_multiarranjo,
os opcodes (nop..jsr_w) e init_instrucoes.
"""
import re
import io

REF = r"JVM/src/Operations.cpp"
OUT = r"JVM_V2/src/executor.cpp"

with io.open(REF, "r", encoding="utf-8", errors="replace") as f:
    text = f.read()

# ---------------------------------------------------------------------------
# 1) Transformacao dos blocos "Value x = { ... };" -> faz_valor_*
# ---------------------------------------------------------------------------
enum_map = {
    'BOOLEAN': 'BOOLEANO', 'BYTE': 'BYTE', 'CHAR': 'CHAR', 'SHORT': 'SHORT',
    'INT': 'INT', 'FLOAT': 'FLOAT', 'LONG': 'LONG', 'DOUBLE': 'DOUBLE',
    'RETURN_ADDR': 'ENDERECO_RETORNO', 'REFERENCE': 'REFERENCIA',
    'PADDING': 'PADDING',
}
field_to_helper = {
    'int_value': ('faz_valor_int', 'INT'),
    'long_value': ('faz_valor_long', 'LONG'),
    'float_value': ('faz_valor_float', 'FLOAT'),
    'double_value': ('faz_valor_double', 'DOUBLE'),
    'boolean_value': ('faz_valor_booleano', 'BOOLEANO'),
    'byte_value': ('faz_valor_byte', 'BYTE'),
    'char_value': ('faz_valor_char', 'CHAR'),
    'short_value': ('faz_valor_short', 'SHORT'),
    'object': ('faz_valor_referencia', 'REFERENCIA'),
    'return_address': ('faz_valor_endereco_retorno', 'ENDERECO_RETORNO'),
}
ourtype_default_helper = {
    'PADDING': ('faz_valor_padding', ''),
    'INT': ('faz_valor_int', '0'),
    'BOOLEANO': ('faz_valor_booleano', 'false'),
    'BYTE': ('faz_valor_byte', '0'),
    'CHAR': ('faz_valor_char', '0'),
    'SHORT': ('faz_valor_short', '0'),
    'FLOAT': ('faz_valor_float', '0'),
    'LONG': ('faz_valor_long', '0'),
    'DOUBLE': ('faz_valor_double', '0'),
    'REFERENCIA': ('faz_valor_referencia', 'NULL'),
    'ENDERECO_RETORNO': ('faz_valor_endereco_retorno', '0'),
}

block_re = re.compile(
    r'(Value\s+)?([A-Za-z_]\w*)\s*=\s*\{((?:[^{}]|\{[^{}]*\})*)\}\s*;', re.S)
pt_re = re.compile(r'print_type\s*=\s*ValueType::(\w+)')
ty_re = re.compile(r'(?<!print_)type\s*=\s*ValueType::(\w+)')
dt_re = re.compile(r'data\s*=\s*\{\s*\.(\w+)\s*=\s*(.*?)\s*\}', re.S)


def repl_block(m):
    kw = m.group(1)
    name = m.group(2)
    inner = m.group(3)
    pt = pt_re.search(inner)
    ty = ty_re.search(inner)
    dt = dt_re.search(inner)
    our_type = enum_map[ty.group(1)] if ty else None
    our_print = enum_map[pt.group(1)] if pt else our_type
    prefix = 'Valor ' if kw else ''
    if dt:
        field = dt.group(1)
        expr = dt.group(2).strip()
        helper, natural = field_to_helper[field]
        call = '%s(%s)' % (helper, expr)
    else:
        t = our_type if our_type else 'INT'
        helper, default = ourtype_default_helper[t]
        natural = t
        call = '%s()' % helper if default == '' else '%s(%s)' % (helper, default)
    lines = ['%s%s = %s;' % (prefix, name, call)]
    if our_type and our_type != natural:
        lines.append('%s.tipo = %s;' % (name, our_type))
    if our_print and our_print != natural:
        lines.append('%s.tipo_print = %s;' % (name, our_print))
    return '\n    '.join(lines)


text = block_re.sub(repl_block, text)

# ---------------------------------------------------------------------------
# 2) Substituicoes de nome (ordem importa)
# ---------------------------------------------------------------------------
subs = [
    # singleton / pilha
    ('Stack::get_instance()', 'PilhaExecucao::instancia()'),
    ('Stack& ', 'PilhaExecucao& '),
    ('->get_top_frame()', '->frame_topo()'),
    ('.get_top_frame()', '.frame_topo()'),
    ('->push_frame(', '->empilhar_frame('),
    ('.push_frame(', '.empilhar_frame('),
    ('->pop_frame()', '->desempilhar_frame()'),
    ('.pop_frame()', '.desempilhar_frame()'),
    # area de metodos
    ('MethodArea::get_instance()', 'AreaMetodos::instancia()'),
    ('MethodArea', 'AreaMetodos'),
    ('load_class_by_name(', 'carregar_classe('),
    # classes/arquivo
    ('get_class_file()', 'get_arquivo_classe()'),
    ('ClassFile', 'ArquivoClasse'),
    ('get_formatted_constant', 'formatar_constante'),
    ('get_class_runtime()', 'get_classe_runtime()'),
    # campos de ClasseEstatica (getstatic/putstatic) -> nomes proprios
    ('class_runtime->field_exists(', 'class_runtime->check_field('),
    ('class_runtime->get_value_from_field(', 'class_runtime->get_value('),
    ('class_runtime->insert_value_into_field(', 'class_runtime->insert_value('),
    # objetos do heap
    ('ObjectType::CLASS_INSTANCE', 'INSTANCIA_CLASSE'),
    ('ObjectType::STRING_INSTANCE', 'INSTANCIA_STRING'),
    ('ObjectType::ARRAY', 'ARRANJO'),
    ('object_type()', 'tipo_objeto()'),
    ('InstanceClass', 'ClasseInstancia'),
    ('StaticClass', 'ClasseEstatica'),
    ('StrObject', 'ObjetoString'),
    ('Array', 'Arranjo'),
    ('Object', 'Objeto'),
    # enums ValueType
    ('ValueType::BOOLEAN', 'BOOLEANO'),
    ('ValueType::RETURN_ADDR', 'ENDERECO_RETORNO'),
    ('ValueType::REFERENCE', 'REFERENCIA'),
    ('ValueType::', ''),
    ('ValueType', 'TipoValor'),
    # union de dados
    ('.data.boolean_value', '.dados.valor_booleano'),
    ('.data.byte_value', '.dados.valor_byte'),
    ('.data.char_value', '.dados.valor_char'),
    ('.data.short_value', '.dados.valor_short'),
    ('.data.int_value', '.dados.valor_int'),
    ('.data.float_value', '.dados.valor_float'),
    ('.data.long_value', '.dados.valor_long'),
    ('.data.double_value', '.dados.valor_double'),
    ('.data.object', '.dados.objeto'),
    ('.data.return_address', '.dados.endereco_retorno'),
    # campos tipo/print_type
    ('.print_type', '.tipo_print'),
    ('.type', '.tipo'),
    # funcoes/membros
    ('Operations::', 'Executor::'),
    ('generic_functions_arr', 'tabela_funcoes'),
    ('populateMultiarray', 'popula_multiarranjo'),
    ('initInstructions', 'init_instrucoes'),
]
for a, b in subs:
    text = text.replace(a, b)

# 'Value' como palavra inteira -> 'Valor' (declaracoes, stack<Value>, vector<Value>);
# nao toca identificadores como keyValue/staticValue (V precedido de letra).
text = re.sub(r'\bValue\b', 'Valor', text)

# ---------------------------------------------------------------------------
# 3) Extracao das secoes
# ---------------------------------------------------------------------------
i_pop = text.index('void Executor::popula_multiarranjo')
i_nop = text.index('void Executor::nop()')
i_ini = text.index('void Executor::init_instrucoes()')

popula = text[i_pop:i_nop].rstrip()
opcodes = text[i_nop:i_ini].rstrip()
init_fn = text[i_ini:].rstrip()

# injeta o preenchimento default no inicio de init_instrucoes
brace = init_fn.index('{')
default_fill = (
    "\n    // Por padrao todo opcode cai no fallback de 'nao implementado'.\n"
    "    for (int i = 0; i < 202; i++) {\n"
    "        tabela_funcoes[i] = &Executor::instrucao_nao_implementada;\n"
    "    }\n")
init_fn = init_fn[:brace + 1] + default_fill + init_fn[brace + 1:]

# ---------------------------------------------------------------------------
# 4) Montagem do arquivo final
# ---------------------------------------------------------------------------
preamble = r'''/*
 * executor.cpp
 *
 * Implementacao do Executor (= Operations da referencia). Passo 8: conjunto
 * completo de opcodes. Portado de JVM/src/Operations.cpp por transformacao
 * mecanica (ver JVM_V2/portar_executor.py): nomes em PT e designated
 * initializers convertidos para os helpers faz_valor_* de tipos_runtime.hpp.
 *
 * executar_metodos / verifica_metodo / instrucao_nao_implementada sao nossas
 * (passo 7); o restante (popula_multiarranjo, opcodes, init_instrucoes) vem
 * da referencia, adaptado.
 */
#include "executor.hpp"
#include "exibidor_classe.hpp"  // formatar_constante, tabela_mnemonicos
#include "area_metodos.hpp"
#include "objeto.hpp"
#include "objeto_string.hpp"
#include "objeto_arranjo.hpp"
#include "classe_instancia.hpp"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <vector>

using std::string;
using std::vector;
using std::stack;
using std::cout;
using std::cerr;
using std::endl;
using std::isnan;
using std::pow;

Executor::Executor()
    : is_wide(false)
{
    init_instrucoes();
}

Executor::~Executor()
{
}

void Executor::executar_metodos(ClasseEstatica* classe_runtime)
{
    PilhaExecucao& pilha = PilhaExecucao::instancia();

    // Argumento do main: o array de String[] da linha de comando (vazio aqui).
    vector<Valor> argumentos;
    argumentos.push_back(faz_valor_referencia(new Arranjo(REFERENCIA)));

    pilha.empilhar_frame(
        new Frame(classe_runtime, "main", "([Ljava/lang/String;)V", argumentos));

    // <clinit> e' ()V: roda antes do main, mas NAO recebe os argumentos do main.
    if (verifica_metodo(classe_runtime, "<clinit>", "()V")) {
        pilha.empilhar_frame(new Frame(classe_runtime, "<clinit>", "()V"));
    }

    // Laco principal: roda ate a pilha esvaziar (ultimo return desempilha).
    while (pilha.tamanho() > 0) {
        Frame* topo = pilha.frame_topo();
        u1* code = topo->get_code(topo->pc);
        (this->*tabela_funcoes[code[0]])();  // cada opcode avanca o pc
    }
}

bool Executor::verifica_metodo(ClasseEstatica* classe_runtime,
                               const string& nome, const string& descritor)
{
    ArquivoClasse* arquivo = classe_runtime->get_arquivo_classe();

    for (int i = 0; i < arquivo->methods_count; i++) {
        MethodInfo metodo = arquivo->methods[i];
        string nome_metodo = formatar_constante(arquivo->constant_pool, metodo.name_index);
        string desc_metodo = formatar_constante(arquivo->constant_pool, metodo.descriptor_index);
        if (nome_metodo == nome && desc_metodo == descritor) {
            return true;
        }
    }
    return false;
}

void Executor::instrucao_nao_implementada()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    u1* code = topo->get_code(topo->pc);
    u1 opcode = code[0];

    const string& mnem = (opcode < 202) ? tabela_mnemonicos[opcode] : string("???");
    cerr << "Opcode nao implementado: 0x" << std::hex << (int)opcode
         << std::dec << " (" << mnem << ")." << endl;
    exit(2);
}

/* ------------------------------------------------------------------------- */
/* popula_multiarranjo (multianewarray)                                      */
/* ------------------------------------------------------------------------- */
'''

with io.open(OUT, "w", encoding="utf-8") as f:
    f.write(preamble)
    f.write(popula)
    f.write("\n\n")
    f.write("/* ----------------------------------------------------------------------- */\n")
    f.write("/* Opcodes                                                                 */\n")
    f.write("/* ----------------------------------------------------------------------- */\n")
    f.write(opcodes)
    f.write("\n\n")
    f.write("/* ----------------------------------------------------------------------- */\n")
    f.write("/* Tabela de despacho                                                      */\n")
    f.write("/* ----------------------------------------------------------------------- */\n")
    f.write(init_fn)
    f.write("\n")

print("OK: gerado", OUT)
