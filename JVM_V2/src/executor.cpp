/*
 * executor.cpp
 *
 * Implementacao do Executor (= Operations da referencia). Passo 8: conjunto
 * completo de opcodes. Portado de JVM/src/Operations.cpp: nomes em PT e
 * designated initializers convertidos para os helpers faz_valor_* de
 * tipos_runtime.hpp.
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

    // Argumento do main: o arranjo de String[] da linha de comando (vazio aqui).
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
        u1* codigo = topo->get_code(topo->pc);
        (this->*tabela_funcoes[codigo[0]])();  // cada opcode avanca o pc
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
    u1* codigo = topo->get_code(topo->pc);
    u1 opcode = codigo[0];

    const string& mnem = (opcode < 202) ? tabela_mnemonicos[opcode] : string("???");
    cerr << "Opcode nao implementado: 0x" << std::hex << (int)opcode
         << std::dec << " (" << mnem << ")." << endl;
    exit(2);
}

/* ------------------------------------------------------------------------- */
/* popula_multiarranjo (multianewarray)                                      */
/* ------------------------------------------------------------------------- */
void Executor::popula_multiarranjo(Arranjo* arranjo, TipoValor tipo_valor, stack<int> contagem)
{
    int curr_count = contagem.top();
    contagem.pop();
    TipoValor tipo_arranjo = (contagem.size() > 1) ? REFERENCIA : tipo_valor;
    if (contagem.size() == 0) {
        for (int i = 0; i < curr_count; i++) {
            Valor sub_array_value = faz_valor_long(0);
            arranjo->push_value(sub_array_value);
        }
    } else {
        for (int i = 0; i < curr_count; i++) {
            Arranjo* subarranjo = new Arranjo(tipo_arranjo);
            popula_multiarranjo(subarranjo, tipo_valor, contagem);
            Valor sub_array_value = faz_valor_referencia(subarranjo);
            arranjo->push_value(sub_array_value);
        }
    }
}

/* ----------------------------------------------------------------------- */
/* Opcodes                                                                 */
/* ----------------------------------------------------------------------- */
void Executor::nop()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    frame_corrente->pc++;
}

void Executor::aconst_null()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_referencia(NULL);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iconst_m1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_int(-1);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iconst_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_int(0);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iconst_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_int(1);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iconst_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_int(2);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iconst_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_int(3);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iconst_4()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_int(4);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iconst_5()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_int(5);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::lconst_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor padding = faz_valor_padding();
    Valor valor = faz_valor_long(0);
    frame_corrente->push_operand_stack(padding);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::lconst_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor padding = faz_valor_padding();
    Valor valor = faz_valor_long(1);
    frame_corrente->push_operand_stack(padding);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::fconst_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_float(0);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::fconst_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_float(1);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::fconst_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = faz_valor_float(2);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::dconst_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor padding = faz_valor_padding();
    Valor valor = faz_valor_double(0);
    frame_corrente->push_operand_stack(padding);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::dconst_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor padding = faz_valor_padding();
    Valor valor = faz_valor_double(1);
    frame_corrente->push_operand_stack(padding);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::bipush()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte = codigo[1];
    Valor valor = faz_valor_int((int32_t)(int8_t)byte);
    valor.tipo_print = BYTE;
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc += 2;
}

void Executor::sipush()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t valor_short = (byte1 << 8) | byte2;
    Valor valor = faz_valor_int((int32_t)(int16_t)valor_short);
    valor.tipo_print = SHORT;
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc += 3;
}

void Executor::ldc()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 indice = codigo[1];
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    ConstantPoolInfo entrada = constant_pool[indice - 1];
    Valor valor;
    if (entrada.tag == ConstStr) {
        ConstantPoolInfo utf8_entry = constant_pool[entrada.info.str_info.string_index - 1];
        assert(utf8_entry.tag == ConstUtf8);
        u1* bytes = utf8_entry.info.utf8_info.bytes;
        char utf8_str[utf8_entry.info.utf8_info.length + 1];
        int i;
        for (i = 0; i < utf8_entry.info.utf8_info.length; i++) {
            utf8_str[i] = bytes[i];
        }
        utf8_str[i] = '\0';
        valor = faz_valor_referencia(new ObjetoString(utf8_str));
    } else if (entrada.tag == ConstInt) {
        valor = faz_valor_int((int32_t)entrada.info.int_info.bytes);
    } else if (entrada.tag == ConstFloat) {
        u4 bytes_float = entrada.info.float_info.bytes;
        int s = ((bytes_float >> 31) == 0) ? 1 : -1;
        int e = ((bytes_float >> 23) & 0xff);
        int m = (e == 0) ? (bytes_float & 0x7fffff) << 1 : (bytes_float & 0x7fffff) | 0x800000;
        float numero = s * m * pow(2, e - 150);
        valor = faz_valor_float(numero);
    } else {
        
        cerr << "ldc trying to access invalid constant pool element " << entrada.tag << endl;
        exit(1);
    }
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc += 2;
}

void Executor::ldc_w()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    u2 indice = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    ConstantPoolInfo entrada = constant_pool[indice - 1];
    Valor valor;
    if (entrada.tag == ConstStr) {
        ConstantPoolInfo utf8_entry = constant_pool[entrada.info.str_info.string_index - 1];
        assert(utf8_entry.tag == ConstUtf8);
        u1* bytes = utf8_entry.info.utf8_info.bytes;
        char utf8_str[utf8_entry.info.utf8_info.length + 1];
        int i;
        for (i = 0; i < utf8_entry.info.utf8_info.length; i++) {
            utf8_str[i] = bytes[i];
        }
        utf8_str[i] = '\0';
        valor = faz_valor_referencia(new ObjetoString(utf8_str));
    } else if (entrada.tag == ConstInt) {
        valor = faz_valor_int((int)entrada.info.int_info.bytes);
    } else if (entrada.tag == ConstFloat) {
        u4 bytes_float = entrada.info.float_info.bytes;
        int s = ((bytes_float >> 31) == 0) ? 1 : -1;
        int e = ((bytes_float >> 23) & 0xff);
        int m = (e == 0) ? (bytes_float & 0x7fffff) << 1 : (bytes_float & 0x7fffff) | 0x800000;
        float numero = s * m * pow(2, e - 150);
        valor = faz_valor_float(numero);
    } else {
        cerr << "ldc_w trying to access invalid constant pool element " << entrada.tag << endl;
        exit(1);
    }
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc += 3;
}

void Executor::ldc2_w()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    u2 indice = (byte1 << 8) | byte2;
    ConstantPoolInfo* arquivo_classe = *(frame_corrente->get_constant_pool());
    ConstantPoolInfo entrada = arquivo_classe[indice - 1];
    Valor valor;
    if (entrada.tag == ConstLong) {
        u4 bytes_altos = entrada.info.long_info.high_bytes;
        u4 bytes_baixos = entrada.info.long_info.low_bytes;
        int64_t numero_long = ((int64_t)bytes_altos << 32) + bytes_baixos;
        valor = faz_valor_long(numero_long);
        Valor padding = faz_valor_padding();
        frame_corrente->push_operand_stack(padding);
    } else if (entrada.tag == ConstDouble) {
        u4 bytes_altos = entrada.info.double_info.high_bytes;
        u4 bytes_baixos = entrada.info.double_info.low_bytes;
        int64_t numero_long = ((int64_t)bytes_altos << 32) + bytes_baixos;
        int32_t s = ((numero_long >> 63) == 0) ? 1 : -1;
        int32_t e = (int32_t)((numero_long >> 52) & 0x7ffL);
        int64_t m = (e == 0) ? (numero_long & 0xfffffffffffffL) << 1 : (numero_long & 0xfffffffffffffL) | 0x10000000000000L;
        double numero_double = s * m * pow(2, e - 1075);
        valor = faz_valor_double(numero_double);
        Valor padding = faz_valor_padding();
        frame_corrente->push_operand_stack(padding);
    } else {
        cerr << "ldc2_w trying to access invalid constant pool element " << entrada.tag << endl;
        exit(1);
    }
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc += 3;
}

void Executor::iload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > indice));
    Valor valor = frame_corrente->get_local_variable_value(indice);
    assert(valor.tipo == INT);
    frame_corrente->push_operand_stack(valor);
}

void Executor::lload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > (indice + 1)));
    Valor valor = frame_corrente->get_local_variable_value(indice);
    assert(valor.tipo == LONG);
    Valor padding = faz_valor_padding();
    frame_corrente->push_operand_stack(padding);
    frame_corrente->push_operand_stack(valor);
}

void Executor::fload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > indice));
    Valor valor = frame_corrente->get_local_variable_value(indice);
    assert(valor.tipo == FLOAT);
    frame_corrente->push_operand_stack(valor);
}

void Executor::dload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > (indice + 1)));
    Valor valor = frame_corrente->get_local_variable_value(indice);
    assert(valor.tipo == DOUBLE);
    Valor padding = faz_valor_padding();
    frame_corrente->push_operand_stack(padding);
    frame_corrente->push_operand_stack(valor);
}

void Executor::aload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > indice));
    Valor valor = frame_corrente->get_local_variable_value(indice);
    assert(valor.tipo == REFERENCIA);
    frame_corrente->push_operand_stack(valor);
}

void Executor::iload_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(0);
    assert(valor.tipo == INT);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iload_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(1);
    assert(valor.tipo == INT);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iload_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(2);
    assert(valor.tipo == INT);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iload_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(3);
    assert(valor.tipo == INT);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::lload_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor;
    valor = frame_corrente->get_local_variable_value(1);
    assert(valor.tipo == PADDING);
    frame_corrente->push_operand_stack(valor);
    valor = frame_corrente->get_local_variable_value(0);
    assert(valor.tipo == LONG);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::lload_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor;
    valor = frame_corrente->get_local_variable_value(2);
    assert(valor.tipo == PADDING);
    frame_corrente->push_operand_stack(valor);
    valor = frame_corrente->get_local_variable_value(1);
    assert(valor.tipo == LONG);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::lload_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor;
    valor = frame_corrente->get_local_variable_value(3);
    assert(valor.tipo == PADDING);
    frame_corrente->push_operand_stack(valor);
    valor = frame_corrente->get_local_variable_value(2);
    assert(valor.tipo == LONG);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::lload_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor;
    valor = frame_corrente->get_local_variable_value(4);
    assert(valor.tipo == PADDING);
    frame_corrente->push_operand_stack(valor);
    valor = frame_corrente->get_local_variable_value(3);
    assert(valor.tipo == LONG);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::fload_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(0);
    assert(valor.tipo == FLOAT);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::fload_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(1);
    assert(valor.tipo == FLOAT);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::fload_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(2);
    assert(valor.tipo == FLOAT);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::fload_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(3);
    assert(valor.tipo == FLOAT);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::dload_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor;
    valor = frame_corrente->get_local_variable_value(1);
    assert(valor.tipo == PADDING);
    frame_corrente->push_operand_stack(valor);
    valor = frame_corrente->get_local_variable_value(0);
    assert(valor.tipo == DOUBLE);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::dload_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor;
    valor = frame_corrente->get_local_variable_value(2);
    assert(valor.tipo == PADDING);
    frame_corrente->push_operand_stack(valor);
    valor = frame_corrente->get_local_variable_value(1);
    assert(valor.tipo == DOUBLE);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::dload_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor;
    valor = frame_corrente->get_local_variable_value(3);
    assert(valor.tipo == PADDING);
    frame_corrente->push_operand_stack(valor);
    valor = frame_corrente->get_local_variable_value(2);
    assert(valor.tipo == DOUBLE);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::dload_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor;
    valor = frame_corrente->get_local_variable_value(4);
    assert(valor.tipo == PADDING);
    frame_corrente->push_operand_stack(valor);
    valor = frame_corrente->get_local_variable_value(3);
    assert(valor.tipo == DOUBLE);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::aload_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(0);
    assert(valor.tipo == REFERENCIA);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::aload_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(1);
    assert(valor.tipo == REFERENCIA);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::aload_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(2);
    assert(valor.tipo == REFERENCIA);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::aload_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->get_local_variable_value(3);
    assert(valor.tipo == REFERENCIA);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::iaload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    frame_corrente->push_operand_stack(arranjo->get_value(indice.dados.valor_int));
    frame_corrente->pc++;
}

void Executor::laload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if ((signed)indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor padding = faz_valor_padding();
    frame_corrente->push_operand_stack(padding);
    frame_corrente->push_operand_stack(arranjo->get_value(indice.dados.valor_int));
    frame_corrente->pc++;
}

void Executor::faload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    frame_corrente->push_operand_stack(arranjo->get_value(indice.dados.valor_int));
    frame_corrente->pc++;
}

void Executor::daload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor padding = faz_valor_padding();
    frame_corrente->push_operand_stack(padding);
    frame_corrente->push_operand_stack(arranjo->get_value(indice.dados.valor_int));
    frame_corrente->pc++;
}

void Executor::aaload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    frame_corrente->push_operand_stack(arranjo->get_value(indice.dados.valor_int));
    frame_corrente->pc++;
}

void Executor::baload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor valor = arranjo->get_value(indice.dados.valor_int);
    assert(valor.tipo == BOOLEANO || valor.tipo == BYTE);
    if (valor.tipo == BOOLEANO) {
        valor = faz_valor_int(valor.dados.valor_booleano);
    valor.tipo_print = BOOLEANO;
    } else {
        valor = faz_valor_int((int32_t)valor.dados.valor_byte);
    valor.tipo_print = BYTE;
    }
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::caload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor char_value = arranjo->get_value(indice.dados.valor_int);
    char_value = faz_valor_int(char_value.dados.valor_char);
    char_value.tipo_print = CHAR;
    frame_corrente->push_operand_stack(char_value);
    frame_corrente->pc++;
}

void Executor::saload()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor valor_short = arranjo->get_value(indice.dados.valor_int);
    valor_short = faz_valor_int((int32_t)valor_short.dados.valor_short);
    valor_short.tipo_print = SHORT;
    frame_corrente->push_operand_stack(valor_short);
    frame_corrente->pc++;
}

void Executor::istore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > indice));
    frame_corrente->set_local_variable(valor, indice);
}

void Executor::lstore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == LONG);
    frame_corrente->pop_operand_stack();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > (indice + 1)));
    frame_corrente->set_local_variable(valor, indice);
    Valor padding = faz_valor_padding();
    frame_corrente->set_local_variable(padding, indice + 1);
}

void Executor::fstore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == FLOAT);
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > indice));
    frame_corrente->set_local_variable(valor, indice);
}

void Executor::dstore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == DOUBLE);
    frame_corrente->pop_operand_stack();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > (indice + 1)));
    frame_corrente->set_local_variable(valor, indice);
    Valor padding = faz_valor_padding();
    frame_corrente->set_local_variable(padding, indice + 1);
}

void Executor::astore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == REFERENCIA);
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    int16_t indice = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
        frame_corrente->pc += 3;
        is_wide = false;
    } else {
        frame_corrente->pc += 2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > indice));
    frame_corrente->set_local_variable(valor, indice);
}

void Executor::istore_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    frame_corrente->set_local_variable(valor, 0);
    frame_corrente->pc++;
}

void Executor::istore_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    frame_corrente->set_local_variable(valor, 1);
    frame_corrente->pc++;
}

void Executor::istore_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    frame_corrente->set_local_variable(valor, 2);
    frame_corrente->pc++;
}

void Executor::istore_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    frame_corrente->set_local_variable(valor, 3);
    frame_corrente->pc++;
}

void Executor::lstore_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == LONG);
    frame_corrente->set_local_variable(valor, 0);
    valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == PADDING);
    frame_corrente->set_local_variable(valor, 1);
    frame_corrente->pc++;
}

void Executor::lstore_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == LONG);
    frame_corrente->set_local_variable(valor, 1);
    valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == PADDING);
    frame_corrente->set_local_variable(valor, 2);
    frame_corrente->pc++;
}

void Executor::lstore_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == LONG);
    frame_corrente->set_local_variable(valor, 2);
    valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == PADDING);
    frame_corrente->set_local_variable(valor, 3);
    frame_corrente->pc++;
}

void Executor::lstore_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == LONG);
    frame_corrente->set_local_variable(valor, 3);
    valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == PADDING);
    frame_corrente->set_local_variable(valor, 4);
    frame_corrente->pc++;
}

void Executor::fstore_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == FLOAT);
    frame_corrente->set_local_variable(valor, 0);
    frame_corrente->pc++;
}

void Executor::fstore_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == FLOAT);
    frame_corrente->set_local_variable(valor, 1);
    frame_corrente->pc++;
}

void Executor::fstore_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == FLOAT);
    frame_corrente->set_local_variable(valor, 2);
    frame_corrente->pc++;
}

void Executor::fstore_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == FLOAT);
    frame_corrente->set_local_variable(valor, 3);
    frame_corrente->pc++;
}

void Executor::dstore_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == DOUBLE);
    frame_corrente->set_local_variable(valor, 0);
    valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == PADDING);
    frame_corrente->set_local_variable(valor, 1);
    frame_corrente->pc++;
}

void Executor::dstore_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == DOUBLE);
    frame_corrente->set_local_variable(valor, 1);
    valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == PADDING);
    frame_corrente->set_local_variable(valor, 2);
    frame_corrente->pc++;
}

void Executor::dstore_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == DOUBLE);
    frame_corrente->set_local_variable(valor, 2);
    valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == PADDING);
    frame_corrente->set_local_variable(valor, 3);
    frame_corrente->pc++;
}

void Executor::dstore_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == DOUBLE);
    frame_corrente->set_local_variable(valor, 3);
    valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == PADDING);
    frame_corrente->set_local_variable(valor, 4);
    frame_corrente->pc++;
}

void Executor::astore_0()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == REFERENCIA);
    frame_corrente->set_local_variable(valor, 0);
    frame_corrente->pc++;
}

void Executor::astore_1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == REFERENCIA);
    frame_corrente->set_local_variable(valor, 1);
    frame_corrente->pc++;
}

void Executor::astore_2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == REFERENCIA);
    frame_corrente->set_local_variable(valor, 2);
    frame_corrente->pc++;
}

void Executor::astore_3()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == REFERENCIA);
    frame_corrente->set_local_variable(valor, 3);
    frame_corrente->pc++;
}

void Executor::iastore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int >= (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    valor.tipo_print = INT;
    assert(valor.tipo == arranjo->array_content_type());
    arranjo->change_value(indice.dados.valor_int, valor);
    frame_corrente->pc++;
}

void Executor::lastore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == LONG);
    Valor padding = frame_corrente->pop_operand_stack();
    assert(padding.tipo == PADDING);
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int >= (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(valor.tipo == arranjo->array_content_type());
    arranjo->change_value(indice.dados.valor_int, valor);
    frame_corrente->pc++;
}

void Executor::fastore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == FLOAT);
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int >= (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(valor.tipo == arranjo->array_content_type());
    arranjo->change_value(indice.dados.valor_int, valor);
    frame_corrente->pc++;
}

void Executor::dastore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == DOUBLE);
    Valor padding = frame_corrente->pop_operand_stack();
    assert(padding.tipo == PADDING);
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int >= (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(valor.tipo == arranjo->array_content_type());
    arranjo->change_value(indice.dados.valor_int, valor);
    frame_corrente->pc++;
}

void Executor::aastore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == REFERENCIA);
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int >= (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    arranjo->change_value(indice.dados.valor_int, valor);
    frame_corrente->pc++;
}

void Executor::bastore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    assert(arranjo->array_content_type() == BOOLEANO || arranjo->array_content_type() == BYTE);
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    if (arranjo->array_content_type() == BOOLEANO) {
        valor = faz_valor_booleano((valor.dados.valor_int != 0) ? true : false);
    } else {
        valor = faz_valor_byte((int8_t)valor.dados.valor_int);
    }
    arranjo->change_value(indice.dados.valor_int, valor);
    frame_corrente->pc++;
}

void Executor::castore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    valor = faz_valor_char((uint8_t)valor.dados.valor_int);
    arranjo->change_value(indice.dados.valor_int, valor);
    frame_corrente->pc++;
}

void Executor::sastore()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Arranjo* arranjo;
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    Valor indice = frame_corrente->pop_operand_stack();
    assert(indice.tipo == INT);
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    arranjo = (Arranjo*)arrayref.dados.objeto;
    if (arranjo == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (indice.dados.valor_int > (signed)arranjo->get_size() || indice.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    valor = faz_valor_short((int16_t)valor.dados.valor_int);
    arranjo->change_value(indice.dados.valor_int, valor);
    frame_corrente->pc++;
}

void Executor::pop()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo != LONG);
    assert(valor.tipo != DOUBLE);
    frame_corrente->pc++;
}

void Executor::pop2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    frame_corrente->pc++;
}

void Executor::dup()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo != LONG);
    assert(valor.tipo != DOUBLE);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->push_operand_stack(valor);
    frame_corrente->pc++;
}

void Executor::dup_x1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo != LONG);
    assert(valor_1.tipo != DOUBLE);
    Valor valor_2 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo != LONG);
    assert(valor_2.tipo != DOUBLE);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::dup_x2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_3 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo != LONG);
    assert(valor_1.tipo != DOUBLE);
    assert(valor_3.tipo != LONG);
    assert(valor_3.tipo != DOUBLE);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->push_operand_stack(valor_3);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::dup2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo != LONG);
    assert(valor_2.tipo != DOUBLE);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::dup2_x1()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_3 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo != LONG);
    assert(valor_2.tipo != DOUBLE);
    assert(valor_3.tipo != LONG);
    assert(valor_3.tipo != DOUBLE);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->push_operand_stack(valor_3);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::dup2_x2()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_3 = frame_corrente->pop_operand_stack();
    Valor value_4 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo != LONG);
    assert(valor_2.tipo != DOUBLE);
    assert(value_4.tipo != LONG);
    assert(value_4.tipo != DOUBLE);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->push_operand_stack(value_4);
    frame_corrente->push_operand_stack(valor_3);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::swap()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo != LONG);
    assert(valor_1.tipo != DOUBLE);
    assert(valor_2.tipo != LONG);
    assert(valor_2.tipo != DOUBLE);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->push_operand_stack(valor_2);
    frame_corrente->pc++;
}

void Executor::iadd()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    valor_1.dados.valor_int = valor_1.dados.valor_int + (valor_2.dados.valor_int);
    valor_1.tipo_print = INT;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::ladd()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);
    valor_1.dados.valor_long = valor_1.dados.valor_long + (valor_2.dados.valor_long);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::fadd()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == FLOAT);
    assert(valor_1.tipo == FLOAT);
    valor_1.dados.valor_float = valor_1.dados.valor_float + (valor_2.dados.valor_float);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::dadd()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == DOUBLE);
    assert(valor_1.tipo == DOUBLE);
    valor_1.dados.valor_double = valor_1.dados.valor_double + (valor_2.dados.valor_double);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::isub()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    valor_1.tipo_print = INT;
    valor_1.dados.valor_int = valor_1.dados.valor_int - (valor_2.dados.valor_int);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lsub()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);
    valor_1.dados.valor_long = valor_1.dados.valor_long - (valor_2.dados.valor_long);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::fsub()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == FLOAT);
    assert(valor_1.tipo == FLOAT);
    valor_1.dados.valor_float = valor_1.dados.valor_float - (valor_2.dados.valor_float);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::dsub()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == DOUBLE);
    assert(valor_1.tipo == DOUBLE);
    valor_1.dados.valor_double = valor_1.dados.valor_double - (valor_2.dados.valor_double);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::imul()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    valor_1.tipo_print = INT;
    valor_1.dados.valor_int = valor_1.dados.valor_int * (valor_2.dados.valor_int);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lmul()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);
    valor_1.dados.valor_long = valor_1.dados.valor_long * (valor_2.dados.valor_long);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::fmul()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == FLOAT);
    assert(valor_1.tipo == FLOAT);
    valor_1.dados.valor_float = valor_1.dados.valor_float * (valor_2.dados.valor_float);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::dmul()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == DOUBLE);
    assert(valor_1.tipo == DOUBLE);
    valor_1.dados.valor_double = valor_1.dados.valor_double * (valor_2.dados.valor_double);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::idiv()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    if (valor_2.dados.valor_int == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    valor_1.tipo_print = INT;
    valor_1.dados.valor_int = valor_1.dados.valor_int / (valor_2.dados.valor_int);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::ldiv()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);
    if (valor_2.dados.valor_long == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    valor_1.dados.valor_long = valor_1.dados.valor_long / (valor_2.dados.valor_long);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::fdiv()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == FLOAT);
    assert(valor_1.tipo == FLOAT);
    if (valor_2.dados.valor_float == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    valor_1.dados.valor_float = valor_1.dados.valor_float / (valor_2.dados.valor_float);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::ddiv()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == DOUBLE);
    assert(valor_1.tipo == DOUBLE);
    if (valor_2.dados.valor_double == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    valor_1.dados.valor_double = valor_1.dados.valor_double / (valor_2.dados.valor_double);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::irem()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    if (valor_2.dados.valor_int == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    valor_1.tipo_print = INT;
    valor_1.dados.valor_int = valor_1.dados.valor_int - (valor_1.dados.valor_int / valor_2.dados.valor_int) * valor_2.dados.valor_int;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lrem()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);
    if (valor_2.dados.valor_long == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    valor_1.dados.valor_long = valor_1.dados.valor_long - (valor_1.dados.valor_long / valor_2.dados.valor_long) * valor_2.dados.valor_long;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::frem()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == FLOAT);
    assert(valor_1.tipo == FLOAT);
    if (valor_2.dados.valor_float == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    valor_1.dados.valor_float = valor_1.dados.valor_float
        - ((uint32_t)(valor_1.dados.valor_float / valor_2.dados.valor_float)) * valor_2.dados.valor_float;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::drem()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_2.tipo == DOUBLE);
    assert(valor_1.tipo == DOUBLE);
    if (valor_2.dados.valor_double == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    valor_1.dados.valor_double = valor_1.dados.valor_double
        - ((uint64_t)(valor_1.dados.valor_double / valor_2.dados.valor_double)) * valor_2.dados.valor_double;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::ineg()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == INT);
    valor_1.tipo_print = INT;
    valor_1.dados.valor_int = -valor_1.dados.valor_int;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lneg()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();

    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == LONG);
    valor_1.dados.valor_long = -valor_1.dados.valor_long;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::fneg()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == FLOAT);
    valor_1.dados.valor_float = -valor_1.dados.valor_float;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::dneg()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();

    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == DOUBLE);
    valor_1.dados.valor_double = -valor_1.dados.valor_double;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::ishl()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);

    valor_2.dados.valor_int = 0x1f & valor_2.dados.valor_int;
    valor_1.dados.valor_int = valor_1.dados.valor_int << valor_2.dados.valor_int;
    valor_1.tipo_print = INT;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lshl()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == LONG);
    valor_2.dados.valor_long = 0x3f & valor_2.dados.valor_long;
    valor_1.dados.valor_long = (valor_1.dados.valor_long) << valor_2.dados.valor_int;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::ishr()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);

    valor_2.dados.valor_int = 0x1f & valor_2.dados.valor_int;
    valor_1.dados.valor_int = valor_1.dados.valor_int >> valor_2.dados.valor_int;
    valor_1.tipo_print = INT;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lshr()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == LONG);

    valor_2.dados.valor_long = 0x3f & valor_2.dados.valor_long;
    valor_1.dados.valor_long = valor_1.dados.valor_long >> valor_2.dados.valor_long;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::iushr()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    valor_2.dados.valor_int = 0x1f & valor_2.dados.valor_int;
    valor_1.dados.valor_int = valor_1.dados.valor_int >> valor_2.dados.valor_int;
    if (valor_1.dados.valor_int < 0) {
        valor_1.dados.valor_int = valor_1.dados.valor_int + (2 << ~(valor_2.dados.valor_int));
    }
    valor_1.tipo_print = INT;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lushr()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == LONG);
    valor_2.dados.valor_int = 0x3f & valor_2.dados.valor_int;
    valor_1.dados.valor_long = valor_1.dados.valor_long >> valor_2.dados.valor_int;
    if (valor_1.dados.valor_long < 0) {
        valor_1.dados.valor_long = valor_1.dados.valor_long + ((int64_t)2 << ~(valor_2.dados.valor_int));
    }
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::iand()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    valor_1.tipo_print = INT;
    valor_1.dados.valor_int = valor_1.dados.valor_int & valor_2.dados.valor_int;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::land()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);

    valor_1.dados.valor_long = valor_1.dados.valor_long & valor_2.dados.valor_long;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::ior()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    valor_1.tipo_print = INT;
    valor_1.dados.valor_int = valor_1.dados.valor_int | valor_2.dados.valor_int;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lor()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);
    valor_1.dados.valor_long = valor_1.dados.valor_long | valor_2.dados.valor_long;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::ixor()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_2.tipo == INT);
    assert(valor_1.tipo == INT);
    valor_1.tipo_print = INT;
    valor_1.dados.valor_int = valor_1.dados.valor_int ^ valor_2.dados.valor_int;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lxor()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);
    valor_1.dados.valor_long = valor_1.dados.valor_long ^ valor_2.dados.valor_long;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::iinc()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u2 indice = 0;
    if (is_wide) {
        indice = (codigo[1] << 8) | codigo[2];
    } else {
        indice += codigo[1];
    }
    Valor variavel_local = frame_corrente->get_local_variable_value(indice);
    assert(variavel_local.tipo == INT);
    int32_t inc;
    if (is_wide) {
        uint16_t constant = (codigo[3] << 8) | codigo[4];
        inc = (int32_t)(int16_t)constant;
    } else {
        inc = (int32_t)(int8_t)codigo[2];
    }
    variavel_local.dados.valor_int += inc;
    frame_corrente->set_local_variable(variavel_local, indice);
    frame_corrente->pc += is_wide ? 5 : 3;
    is_wide = false;
}

void Executor::i2l()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == INT);
    Valor padding = faz_valor_padding();
    frame_corrente->push_operand_stack(padding);
    valor_1 = faz_valor_long((int64_t)valor_1.dados.valor_int);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::i2f()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == INT);
    valor_1 = faz_valor_float((float)valor_1.dados.valor_int);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::i2d()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == INT);
    Valor padding = faz_valor_padding();
    frame_corrente->push_operand_stack(padding);
    valor_1 = faz_valor_double((double)valor_1.dados.valor_int);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::l2i()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == LONG);
    valor_1 = faz_valor_int((int32_t)valor_1.dados.valor_int);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::l2f()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == LONG);
    valor_1 = faz_valor_float((float)valor_1.dados.valor_long);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::l2d()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_1.tipo == LONG);
    valor_1 = faz_valor_double((double)valor_1.dados.valor_long);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::f2i()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == FLOAT);
    valor_1 = faz_valor_int((int32_t)valor_1.dados.valor_float);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::f2l()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == FLOAT);
    Valor padding = faz_valor_padding();
    frame_corrente->push_operand_stack(padding);
    valor_1 = faz_valor_long((int64_t)valor_1.dados.valor_float);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::f2d()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == FLOAT);
    Valor padding = faz_valor_padding();
    frame_corrente->push_operand_stack(padding);
    valor_1 = faz_valor_double((double)valor_1.dados.valor_float);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::d2i()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == DOUBLE);
    valor_1 = faz_valor_int((int32_t)valor_1.dados.valor_double);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::d2l()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();

    assert(valor_1.tipo == DOUBLE);
    valor_1 = faz_valor_long((int64_t)valor_1.dados.valor_double);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::d2f()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == DOUBLE);
    valor_1 = faz_valor_float((float)valor_1.dados.valor_double);
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::i2b()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == INT);
    valor_1 = faz_valor_int((int32_t)(int8_t)valor_1.dados.valor_int);
    valor_1.tipo_print = BYTE;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::i2c()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == INT);
    valor_1 = faz_valor_char((uint8_t)valor_1.dados.valor_int);
    valor_1.tipo = INT;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::i2s()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    assert(valor_1.tipo == INT);
    valor_1 = faz_valor_short((int16_t)valor_1.dados.valor_int);
    valor_1.tipo = INT;
    frame_corrente->push_operand_stack(valor_1);
    frame_corrente->pc++;
}

void Executor::lcmp()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor resultado = faz_valor_int(0);
    assert(valor_2.tipo == LONG);
    assert(valor_1.tipo == LONG);
    if (valor_1.dados.valor_long > valor_2.dados.valor_long) {
        resultado.dados.valor_int = 1;
    } else if (valor_1.dados.valor_long == valor_2.dados.valor_long) {
        resultado.dados.valor_int = 0;
    } else {
        resultado.dados.valor_int = -1;
    }
    frame_corrente->push_operand_stack(resultado);
    frame_corrente->pc++;
}

void Executor::fcmpl()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    Valor resultado = faz_valor_int(0);
    assert(valor_2.tipo == FLOAT);
    assert(valor_1.tipo == FLOAT);
    if (isnan(valor_1.dados.valor_float) || isnan(valor_2.dados.valor_float)) {
        resultado.dados.valor_int = -1;
    } else if (valor_1.dados.valor_float > valor_2.dados.valor_float) {
        resultado.dados.valor_int = 1;
    } else if (valor_1.dados.valor_float == valor_2.dados.valor_float) {
        resultado.dados.valor_int = 0;
    } else {
        resultado.dados.valor_int = -1;
    }
    frame_corrente->push_operand_stack(resultado);
    frame_corrente->pc++;
}

void Executor::fcmpg()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    Valor resultado = faz_valor_int(0);
    assert(valor_2.tipo == FLOAT);
    assert(valor_1.tipo == FLOAT);
    if (isnan(valor_1.dados.valor_float) || isnan(valor_2.dados.valor_float)) {
        resultado.dados.valor_int = 1;
    } else if (valor_1.dados.valor_float > valor_2.dados.valor_float) {
        resultado.dados.valor_int = 1;
    } else if (valor_1.dados.valor_float == valor_2.dados.valor_float) {
        resultado.dados.valor_int = 0;
    } else {
        resultado.dados.valor_int = -1;
    }
    frame_corrente->push_operand_stack(resultado);
    frame_corrente->pc++;
}

void Executor::dcmpl()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor resultado = faz_valor_int(0);
    assert(valor_2.tipo == DOUBLE);
    assert(valor_1.tipo == DOUBLE);
    if (isnan(valor_1.dados.valor_double) || isnan(valor_2.dados.valor_double)) {
        resultado.dados.valor_int = -1;
    } else if (valor_1.dados.valor_double > valor_2.dados.valor_double) {
        resultado.dados.valor_int = 1;
    } else if (valor_1.dados.valor_double == valor_2.dados.valor_double) {
        resultado.dados.valor_int = 0;
    } else {
        resultado.dados.valor_int = -1;
    }
    frame_corrente->push_operand_stack(resultado);
    frame_corrente->pc++;
}

void Executor::dcmpg()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_2 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor valor_1 = frame_corrente->pop_operand_stack();
    frame_corrente->pop_operand_stack();
    Valor resultado = faz_valor_int(0);
    assert(valor_2.tipo == DOUBLE);
    assert(valor_1.tipo == DOUBLE);
    if (isnan(valor_1.dados.valor_double) || isnan(valor_2.dados.valor_double)) {
        resultado.dados.valor_int = 1;
    } else if (valor_1.dados.valor_double > valor_2.dados.valor_double) {
        resultado.dados.valor_int = 1;
    } else if (valor_1.dados.valor_double == valor_2.dados.valor_double) {
        resultado.dados.valor_int = 0;
    } else {
        resultado.dados.valor_int = -1;
    }
    frame_corrente->push_operand_stack(resultado);
    frame_corrente->pc++;
}

void Executor::ifeq()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    if (valor.dados.valor_int == 0) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::ifne()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    if (valor.dados.valor_int != 0) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::iflt()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    if (valor.dados.valor_int < 0) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::ifge()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    if (valor.dados.valor_int >= 0) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::ifgt()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    if (valor.dados.valor_int > 0) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::ifle()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor = frame_corrente->pop_operand_stack();
    assert(valor.tipo == INT);
    if (valor.dados.valor_int <= 0) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::if_icmpeq()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor value2 = frame_corrente->pop_operand_stack();
    Valor value1 = frame_corrente->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int == value2.dados.valor_int) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::if_icmpne()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor value2 = frame_corrente->pop_operand_stack();
    Valor value1 = frame_corrente->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int != value2.dados.valor_int) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::if_icmplt()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor value2 = frame_corrente->pop_operand_stack();
    Valor value1 = frame_corrente->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int < value2.dados.valor_int) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::if_icmpge()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor value2 = frame_corrente->pop_operand_stack();
    Valor value1 = frame_corrente->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int >= value2.dados.valor_int) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::if_icmpgt()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor value2 = frame_corrente->pop_operand_stack();
    Valor value1 = frame_corrente->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int > value2.dados.valor_int) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::if_icmple()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor value2 = frame_corrente->pop_operand_stack();
    Valor value1 = frame_corrente->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int <= value2.dados.valor_int) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::if_acmpeq()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor value2 = frame_corrente->pop_operand_stack();
    Valor value1 = frame_corrente->pop_operand_stack();
    assert(value1.tipo == REFERENCIA);
    assert(value2.tipo == REFERENCIA);
    if (value1.dados.objeto == value2.dados.objeto) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::if_acmpne()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor value2 = frame_corrente->pop_operand_stack();
    Valor value1 = frame_corrente->pop_operand_stack();
    assert(value1.tipo == REFERENCIA);
    assert(value2.tipo == REFERENCIA);
    if (value1.dados.objeto != value2.dados.objeto) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t deslocamento_desvio = (byte1 << 8) | byte2;
        frame_corrente->pc += deslocamento_desvio;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::func_goto()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    int16_t deslocamento_desvio = (byte1 << 8) | byte2;
    frame_corrente->pc += deslocamento_desvio;
}

void Executor::jsr()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    int16_t deslocamento_desvio = (byte1 << 8) | byte2;
    Valor endereco_retorno = faz_valor_endereco_retorno(frame_corrente->pc + 3);
    frame_corrente->push_operand_stack(endereco_retorno);
    frame_corrente->pc += deslocamento_desvio;
}

void Executor::ret()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    uint16_t indice = (uint16_t)byte1;
    if (is_wide) {
        u1 byte2 = codigo[2];
        indice = (byte1 << 8) | byte2;
    }
    assert(((int16_t)(frame_corrente->get_local_variables_vector_size()) > indice));
    Valor valor = frame_corrente->get_local_variable_value(indice);
    assert(valor.tipo == ENDERECO_RETORNO);
    frame_corrente->set_local_variable(valor, indice);
    frame_corrente->pc = valor.dados.endereco_retorno;
    is_wide = false;
}

void Executor::tableswitch()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 padding = 4 - (frame_corrente->pc + 1) % 4;
    padding = (padding == 4) ? 0 : padding;
    u1 defaultbyte1 = codigo[padding + 1];
    u1 defaultbyte2 = codigo[padding + 2];
    u1 defaultbyte3 = codigo[padding + 3];
    u1 defaultbyte4 = codigo[padding + 4];
    int32_t bytes_default = (defaultbyte1 << 24) | (defaultbyte2 << 16) | (defaultbyte3 << 8) | defaultbyte4;
    u1 lowbyte1 = codigo[padding + 5];
    u1 lowbyte2 = codigo[padding + 6];
    u1 lowbyte3 = codigo[padding + 7];
    u1 lowbyte4 = codigo[padding + 8];
    uint32_t lowbytes = (lowbyte1 << 24) | (lowbyte2 << 16) | (lowbyte3 << 8) | lowbyte4;
    u1 highbyte1 = codigo[padding + 9];
    u1 highbyte2 = codigo[padding + 10];
    u1 highbyte3 = codigo[padding + 11];
    u1 highbyte4 = codigo[padding + 12];
    uint32_t highbytes = (highbyte1 << 24) | (highbyte2 << 16) | (highbyte3 << 8) | highbyte4;
    Valor valor_chave = frame_corrente->pop_operand_stack();
    assert(valor_chave.tipo == INT);
    int32_t chave = valor_chave.dados.valor_int;
    uint32_t i;
    uint32_t indice_base = padding + 13;
    int32_t offsets = highbytes - lowbytes + 1;
    bool matched = false;
    for (i = 0; i < (unsigned)offsets; i++) {
        if ((unsigned)chave == (unsigned)lowbytes) {
            int32_t deslocamento = (codigo[indice_base] << 24) | (codigo[indice_base + 1] << 16) | (codigo[indice_base + 2] << 8) | codigo[indice_base + 3];
            frame_corrente->pc += deslocamento;
            matched = true;
            break;
        }
        lowbytes++;
        indice_base += 4;
    }
    if (!matched) {
        frame_corrente->pc += bytes_default;
    }
}

void Executor::lookupswitch()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 padding = 4 - (frame_corrente->pc + 1) % 4;
    padding = (padding == 4) ? 0 : padding;
    u1 defaultbyte1 = codigo[padding + 1];
    u1 defaultbyte2 = codigo[padding + 2];
    u1 defaultbyte3 = codigo[padding + 3];
    u1 defaultbyte4 = codigo[padding + 4];
    int32_t bytes_default = (defaultbyte1 << 24) | (defaultbyte2 << 16) | (defaultbyte3 << 8) | defaultbyte4;
    u1 npairs1 = codigo[padding + 5];
    u1 npairs2 = codigo[padding + 6];
    u1 npairs3 = codigo[padding + 7];
    u1 npairs4 = codigo[padding + 8];
    uint32_t npairs = (npairs1 << 24) | (npairs2 << 16) | (npairs3 << 8) | npairs4;
    Valor valor_chave = frame_corrente->pop_operand_stack();
    assert(valor_chave.tipo == INT);
    int32_t chave = valor_chave.dados.valor_int;
    uint32_t i;
    uint32_t indice_base = padding + 9;
    bool matched = false;
    for (i = 0; i < npairs; i++) {
        int32_t match = (codigo[indice_base] << 24) | (codigo[indice_base + 1] << 16) | (codigo[indice_base + 2] << 8) | codigo[indice_base + 3];
        if (chave == match) {
            int32_t deslocamento = (codigo[indice_base + 4] << 24) | (codigo[indice_base + 5] << 16) | (codigo[indice_base + 6] << 8) | codigo[indice_base + 7];
            frame_corrente->pc += deslocamento;
            matched = true;
            break;
        }
        indice_base += 8;
    }
    if (!matched) {
        frame_corrente->pc += bytes_default;
    }
}

void Executor::ireturn()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_retorno = frame_corrente->pop_operand_stack();
    assert(valor_retorno.tipo == INT);
    pilha_execucao.desempilhar_frame();
    Frame* novo_frame_topo = pilha_execucao.frame_topo();
    novo_frame_topo->push_operand_stack(valor_retorno);
}

void Executor::lreturn()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_retorno = frame_corrente->pop_operand_stack();
    assert(valor_retorno.tipo == LONG);
    assert(frame_corrente->pop_operand_stack().tipo == PADDING);
    pilha_execucao.desempilhar_frame();
    Frame* novo_frame_topo = pilha_execucao.frame_topo();
    Valor padding = faz_valor_padding();
    novo_frame_topo->push_operand_stack(padding);
    novo_frame_topo->push_operand_stack(valor_retorno);
}

void Executor::freturn()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_retorno = frame_corrente->pop_operand_stack();
    assert(valor_retorno.tipo == FLOAT);
    pilha_execucao.desempilhar_frame();
    Frame* novo_frame_topo = pilha_execucao.frame_topo();
    novo_frame_topo->push_operand_stack(valor_retorno);
}

void Executor::dreturn()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_retorno = frame_corrente->pop_operand_stack();
    assert(valor_retorno.tipo == DOUBLE);
    assert(frame_corrente->pop_operand_stack().tipo == PADDING);
    pilha_execucao.desempilhar_frame();
    Frame* novo_frame_topo = pilha_execucao.frame_topo();
    Valor padding = faz_valor_padding();
    novo_frame_topo->push_operand_stack(padding);
    novo_frame_topo->push_operand_stack(valor_retorno);
}

void Executor::areturn()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_retorno = frame_corrente->pop_operand_stack();
    assert(valor_retorno.tipo == REFERENCIA);
    pilha_execucao.desempilhar_frame();
    Frame* novo_frame_topo = pilha_execucao.frame_topo();
    novo_frame_topo->push_operand_stack(valor_retorno);
}

void Executor::func_return()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    pilha_execucao.desempilhar_frame();
}

void Executor::getstatic()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_campo = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_campo = constant_pool[indice_campo - 1];
    assert(cp_campo.tag == ConstFieldRef);
    ConstFieldRefInfo ref_campo = cp_campo.info.field_ref_info;
    string nome_classe = formatar_constante(constant_pool, ref_campo.class_index);
    ConstantPoolInfo cp_nome_tipo = constant_pool[ref_campo.name_and_type_index - 1];
    assert(cp_nome_tipo.tag == ConstNameType);
    ConstNameTypeInfo nome_tipo_campo = cp_nome_tipo.info.name_type_info;
    string nome_campo = formatar_constante(constant_pool, nome_tipo_campo.name_index);
    string descritor_campo = formatar_constante(constant_pool, nome_tipo_campo.descriptor_index);

    if (nome_classe == "java/lang/System" && descritor_campo == "Ljava/io/PrintStream;") {
        frame_corrente->pc += 3;
        return;
    }

    AreaMetodos& area_metodos = AreaMetodos::instancia();
    ClasseEstatica* classe_runtime = area_metodos.carregar_classe(nome_classe);
    while (classe_runtime != NULL) {
        if (classe_runtime->check_field(nome_campo) == false) {
            if (classe_runtime->get_arquivo_classe()->super_class == 0) {
                classe_runtime = NULL;
            } else {
                string nome_super_classe = formatar_constante(classe_runtime->get_arquivo_classe()->constant_pool,
                    classe_runtime->get_arquivo_classe()->super_class);
                classe_runtime = area_metodos.carregar_classe(nome_super_classe);
            }
        } else {
            break;
        }
    }
    if (classe_runtime == NULL) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }

    if (pilha_execucao.frame_topo() != frame_corrente)
        return;
    Valor valor_estatico = classe_runtime->get_value(nome_campo);
    switch (valor_estatico.tipo) {
    case BOOLEANO:
        valor_estatico.tipo = INT;
        valor_estatico.tipo_print = BOOLEANO;
        break;
    case BYTE:
        valor_estatico.tipo = INT;
        valor_estatico.tipo_print = BYTE;
        break;
    case SHORT:
        valor_estatico.tipo = INT;
        valor_estatico.tipo_print = SHORT;
        break;
    case INT:
        valor_estatico.tipo = INT;
        valor_estatico.tipo_print = INT;
        break;
    default:
        break;
    }
    if (valor_estatico.tipo == DOUBLE || valor_estatico.tipo == LONG) {
        Valor valor_padding = faz_valor_padding();
        frame_corrente->push_operand_stack(valor_padding);
    }
    frame_corrente->push_operand_stack(valor_estatico);
    frame_corrente->pc += 3;
}

void Executor::putstatic()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_campo = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_campo = constant_pool[indice_campo - 1];
    assert(cp_campo.tag == ConstFieldRef);
    ConstFieldRefInfo ref_campo = cp_campo.info.field_ref_info;
    string nome_classe = formatar_constante(constant_pool, ref_campo.class_index);
    ConstantPoolInfo cp_nome_tipo = constant_pool[ref_campo.name_and_type_index - 1];
    assert(cp_nome_tipo.tag == ConstNameType);
    ConstNameTypeInfo nome_tipo_campo = cp_nome_tipo.info.name_type_info;
    string nome_campo = formatar_constante(constant_pool, nome_tipo_campo.name_index);
    string descritor_campo = formatar_constante(constant_pool, nome_tipo_campo.descriptor_index);
    AreaMetodos& area_metodos = AreaMetodos::instancia();
    ClasseEstatica* classe_runtime = area_metodos.carregar_classe(nome_classe);
    while (classe_runtime != NULL) {
        if (classe_runtime->check_field(nome_campo) == false) {
            if (classe_runtime->get_arquivo_classe()->super_class == 0) {
                classe_runtime = NULL;
            } else {
                string nome_super_classe = formatar_constante(classe_runtime->get_arquivo_classe()->constant_pool,
                    classe_runtime->get_arquivo_classe()->super_class);
                classe_runtime = area_metodos.carregar_classe(nome_super_classe);
            }
        } else {
            break;
        }
    }
    if (classe_runtime == NULL) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }

    if (pilha_execucao.frame_topo() != frame_corrente)
        return;
    Valor valor_topo = frame_corrente->pop_operand_stack();
    if (valor_topo.tipo == DOUBLE || valor_topo.tipo == LONG) {
        frame_corrente->pop_operand_stack();
    } else {
        switch (descritor_campo[0]) {
        case 'B':
            valor_topo.tipo = BYTE;
            valor_topo.tipo_print = BYTE;
            break;
        case 'C':
            valor_topo.tipo = CHAR;
            valor_topo.tipo = CHAR;
            break;
        case 'S':
            valor_topo.tipo = SHORT;
            valor_topo.tipo = SHORT;
            break;
        case 'Z':
            valor_topo.tipo = BOOLEANO;
            valor_topo.tipo = BOOLEANO;
            break;
        }
    }
    classe_runtime->insert_value(valor_topo, nome_campo);
    frame_corrente->pc += 3;
}

void Executor::getfield()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_campo = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_campo = constant_pool[indice_campo - 1];
    assert(cp_campo.tag == ConstFieldRef);
    ConstFieldRefInfo ref_campo = cp_campo.info.field_ref_info;
    string nome_classe = formatar_constante(constant_pool, ref_campo.class_index);
    ConstantPoolInfo cp_nome_tipo = constant_pool[ref_campo.name_and_type_index - 1];
    assert(cp_nome_tipo.tag == ConstNameType);
    ConstNameTypeInfo nome_tipo_campo = cp_nome_tipo.info.name_type_info;
    string nome_campo = formatar_constante(constant_pool, nome_tipo_campo.name_index);
    string descritor_campo = formatar_constante(constant_pool, nome_tipo_campo.descriptor_index);
    Valor valor_objeto = frame_corrente->pop_operand_stack();
    assert(valor_objeto.tipo == REFERENCIA);
    Objeto* objeto = valor_objeto.dados.objeto;
    assert(objeto->tipo_objeto() == INSTANCIA_CLASSE);
    ClasseInstancia* instancia_classe = (ClasseInstancia*)objeto;
    if (!instancia_classe->field_exists(nome_campo)) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }
    Valor valor_campo = instancia_classe->get_value_from_field(nome_campo);
    switch (valor_campo.tipo) {
    case BOOLEANO:
        valor_campo.tipo = INT;
        valor_campo.tipo_print = BOOLEANO;
        break;
    case BYTE:
        valor_campo.tipo = INT;
        valor_campo.tipo_print = BYTE;
        break;
    case SHORT:
        valor_campo.tipo = INT;
        valor_campo.tipo_print = SHORT;
        break;
    case INT:
        valor_campo.tipo = INT;
        valor_campo.tipo_print = INT;
        break;
    default:
        break;
    }
    if (valor_campo.tipo == DOUBLE || valor_campo.tipo == LONG) {
        Valor valor_padding = faz_valor_padding();
        frame_corrente->push_operand_stack(valor_padding);
    }
    frame_corrente->push_operand_stack(valor_campo);
    frame_corrente->pc += 3;
}

void Executor::putfield()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_campo = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_campo = constant_pool[indice_campo - 1];
    assert(cp_campo.tag == ConstFieldRef);
    ConstFieldRefInfo ref_campo = cp_campo.info.field_ref_info;
    string nome_classe = formatar_constante(constant_pool, ref_campo.class_index);
    ConstantPoolInfo cp_nome_tipo = constant_pool[ref_campo.name_and_type_index - 1];
    assert(cp_nome_tipo.tag == ConstNameType);
    ConstNameTypeInfo nome_tipo_campo = cp_nome_tipo.info.name_type_info;
    string nome_campo = formatar_constante(constant_pool, nome_tipo_campo.name_index);
    string descritor_campo = formatar_constante(constant_pool, nome_tipo_campo.descriptor_index);
    Valor valor_a_inserir = frame_corrente->pop_operand_stack();
    if (valor_a_inserir.tipo == DOUBLE || valor_a_inserir.tipo == LONG) {
        frame_corrente->pop_operand_stack();
    } else {
        switch (descritor_campo[0]) {
        case 'B':
            valor_a_inserir.tipo = BYTE;
            valor_a_inserir.tipo_print = BYTE;
            break;
        case 'C':
            valor_a_inserir.tipo = CHAR;
            valor_a_inserir.tipo_print = CHAR;
            break;
        case 'S':
            valor_a_inserir.tipo = SHORT;
            valor_a_inserir.tipo_print = SHORT;
            break;
        case 'Z':
            valor_a_inserir.tipo = BOOLEANO;
            valor_a_inserir.tipo_print = BOOLEANO;
            break;
        }
    }
    Valor valor_objeto = frame_corrente->pop_operand_stack();
    assert(valor_objeto.tipo == REFERENCIA);
    Objeto* objeto = valor_objeto.dados.objeto;
    assert(objeto->tipo_objeto() == INSTANCIA_CLASSE);
    ClasseInstancia* instancia_classe = (ClasseInstancia*)objeto;
    instancia_classe->insert_value_into_field(valor_a_inserir, nome_campo);
    frame_corrente->pc += 3;
}

void Executor::invokevirtual()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    stack<Valor> backup_pilha_operandos = frame_corrente->copy_operand_stack();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_metodo = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_metodo = constant_pool[indice_metodo - 1];
    assert(cp_metodo.tag == ConstMethodRef);
    ConstMethodRefInfo info_metodo = cp_metodo.info.method_ref_info;
    string nome_classe = formatar_constante(constant_pool, info_metodo.class_index);
    ConstantPoolInfo cp_nome_tipo = constant_pool[info_metodo.name_and_type_index - 1];
    assert(cp_nome_tipo.tag == ConstNameType);
    ConstNameTypeInfo nome_tipo_metodo = cp_nome_tipo.info.name_type_info;
    string nome_metodo = formatar_constante(constant_pool, nome_tipo_metodo.name_index);
    string descritor_metodo = formatar_constante(constant_pool, nome_tipo_metodo.descriptor_index);
    if (nome_classe.find("java/") != string::npos) {

        if (nome_classe == "java/io/PrintStream" && (nome_metodo == "print" || nome_metodo == "println")) {
            if (descritor_metodo != "()V") {
                Valor valor_print = frame_corrente->pop_operand_stack();
                if (valor_print.tipo == INT) {
                    switch (valor_print.tipo_print) {
                    case BOOLEANO:
                        cout << (valor_print.dados.valor_booleano == 0 ? "false" : "true");
                        break;
                    case BYTE:
                        cout << (int)valor_print.dados.valor_byte;
                        break;
                    case CHAR:
                        cout << valor_print.dados.valor_char;
                        break;
                    case SHORT:
                        cout << valor_print.dados.valor_short;
                        break;
                    default:
                        cout << valor_print.dados.valor_int;
                        break;
                    }
                } else {
                    switch (valor_print.tipo) {
                    case DOUBLE:
                        frame_corrente->pop_operand_stack();
                        cout << valor_print.dados.valor_double;
                        break;
                    case FLOAT:
                        cout << valor_print.dados.valor_float;
                        break;
                    case LONG:
                        frame_corrente->pop_operand_stack();
                        cout << valor_print.dados.valor_long;
                        break;
                    case REFERENCIA:
                        assert(valor_print.dados.objeto->tipo_objeto() == INSTANCIA_STRING);
                        cout << ((ObjetoString*)valor_print.dados.objeto)->get_str().c_str();
                        break;
                    case BOOLEANO:
                        cout << (valor_print.dados.valor_booleano == 0 ? "false" : "true");
                        break;
                    case BYTE:
                        cout << (int)valor_print.dados.valor_byte;
                        break;
                    case CHAR:
                        cout << valor_print.dados.valor_char;
                        break;
                    case SHORT:
                        cout << valor_print.dados.valor_short;
                        break;
                    default:
                        cerr << "Invalid print type:" << valor_print.tipo << endl;
                        exit(1);
                        break;
                    }
                }
            }
            if (nome_metodo == "println")
                cout << "\n";
        } else if (nome_classe == "java/lang/String" && nome_metodo == "equals") {
            Valor valor_str1 = frame_corrente->pop_operand_stack();
            Valor valor_str2 = frame_corrente->pop_operand_stack();
            assert(valor_str1.tipo == REFERENCIA);
            assert(valor_str2.tipo == REFERENCIA);
            assert(valor_str1.dados.objeto->tipo_objeto() == INSTANCIA_STRING);
            assert(valor_str2.dados.objeto->tipo_objeto() == INSTANCIA_STRING);
            ObjetoString* str1 = (ObjetoString*)valor_str1.dados.objeto;
            ObjetoString* str2 = (ObjetoString*)valor_str2.dados.objeto;
            Valor resultado = faz_valor_int(0);
            if (str1->get_str() == str2->get_str()) {
                resultado.dados.valor_int = 1;
            } else {
                resultado.dados.valor_int = 0;
            }
            frame_corrente->push_operand_stack(resultado);
        } else if (nome_classe == "java/lang/String" && nome_metodo == "length") {
            Valor valor_str = frame_corrente->pop_operand_stack();
            assert(valor_str.tipo == REFERENCIA);
            assert(valor_str.dados.objeto->tipo_objeto() == INSTANCIA_STRING);
            ObjetoString* str = (ObjetoString*)valor_str.dados.objeto;
            Valor resultado = faz_valor_int((int32_t)(str->get_str()).size());
            frame_corrente->push_operand_stack(resultado);
        } else {
            cerr << "Call to invalid instance method: " << nome_metodo << endl;
            exit(1);
        }
    } else {
        uint16_t nargs = 0;
        uint16_t i = 1;
        while (descritor_metodo[i] != ')') {
            char tipo_base = descritor_metodo[i];
            if (tipo_base == 'D' || tipo_base == 'J') {
                nargs += 2;
            } else if (tipo_base == 'L') {
                nargs++;
                while (descritor_metodo[++i] != ';')
                    ;
            } else if (tipo_base == '[') {
                nargs++;
                while (descritor_metodo[++i] == '[')
                    ;
                if (descritor_metodo[i] == 'L')
                    while (descritor_metodo[++i] != ';')
                        ;
            } else {
                nargs++;
            }
            i++;
        }
        vector<Valor> args;
        for (int i = 0; i < nargs; i++) {
            Valor valor = frame_corrente->pop_operand_stack();
            if (valor.tipo == PADDING) {
                args.insert(args.begin() + 1, valor);
            } else {
                args.insert(args.begin(), valor);
            }
        }
        Valor valor_objeto = frame_corrente->pop_operand_stack();
        assert(valor_objeto.tipo == REFERENCIA);
        args.insert(args.begin(), valor_objeto);
        Objeto* objeto = valor_objeto.dados.objeto;
        assert(objeto->tipo_objeto() == INSTANCIA_CLASSE);
        ClasseInstancia* instance = (ClasseInstancia*)objeto;
        AreaMetodos& area_metodos = AreaMetodos::instancia();
        ClasseEstatica* classe_runtime = area_metodos.carregar_classe(nome_classe);
        Frame* novo_frame = new Frame(instance, classe_runtime, nome_metodo, descritor_metodo, args);

        if (pilha_execucao.frame_topo() != frame_corrente) {
            frame_corrente->load_operand_stack(backup_pilha_operandos);
            delete novo_frame;
            return;
        }
        pilha_execucao.empilhar_frame(novo_frame);
    }
    frame_corrente->pc += 3;
}

void Executor::invokespecial()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    stack<Valor> backup_pilha_operandos = frame_corrente->copy_operand_stack();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_metodo = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_metodo = constant_pool[indice_metodo - 1];
    assert(cp_metodo.tag == ConstMethodRef);
    ConstMethodRefInfo info_metodo = cp_metodo.info.method_ref_info;
    string nome_classe = formatar_constante(constant_pool, info_metodo.class_index);
    ConstantPoolInfo cp_nome_tipo = constant_pool[info_metodo.name_and_type_index - 1];
    assert(cp_nome_tipo.tag == ConstNameType);
    ConstNameTypeInfo nome_tipo_metodo = cp_nome_tipo.info.name_type_info;
    string nome_metodo = formatar_constante(constant_pool, nome_tipo_metodo.name_index);
    string descritor_metodo = formatar_constante(constant_pool, nome_tipo_metodo.descriptor_index);

    if ((nome_classe == "java/lang/Object" || nome_classe == "java/lang/String") && nome_metodo == "<init>") {
        if (nome_classe == "java/lang/String") {
            frame_corrente->pop_operand_stack();
        }
        frame_corrente->pc += 3;
        return;
    }

    if (nome_classe.find("java/") != string::npos) {
        cerr << "Call to invalid special method: " << nome_metodo << endl;
        exit(1);
    } else {
        uint16_t nargs = 0;
        uint16_t i = 1;
        while (descritor_metodo[i] != ')') {
            char tipo_base = descritor_metodo[i];
            if (tipo_base == 'D' || tipo_base == 'J') {
                nargs += 2;
            } else if (tipo_base == 'L') {
                nargs++;
                while (descritor_metodo[++i] != ';')
                    ;
            } else if (tipo_base == '[') {
                nargs++;
                while (descritor_metodo[++i] == '[')
                    ;
                if (descritor_metodo[i] == 'L')
                    while (descritor_metodo[++i] != ';')
                        ;
            } else {
                nargs++;
            }
            i++;
        }
        vector<Valor> args;
        for (int i = 0; i < nargs; i++) {
            Valor valor = frame_corrente->pop_operand_stack();
            if (valor.tipo == PADDING) {
                args.insert(args.begin() + 1, valor);
            } else {
                args.insert(args.begin(), valor);
            }
        }
        Valor valor_objeto = frame_corrente->pop_operand_stack();
        assert(valor_objeto.tipo == REFERENCIA);
        args.insert(args.begin(), valor_objeto);
        Objeto* objeto = valor_objeto.dados.objeto;
        assert(objeto->tipo_objeto() == INSTANCIA_CLASSE);
        ClasseInstancia* instance = (ClasseInstancia*)objeto;
        AreaMetodos& area_metodos = AreaMetodos::instancia();
        ClasseEstatica* classe_runtime = area_metodos.carregar_classe(nome_classe);
        Frame* novo_frame = new Frame(instance, classe_runtime, nome_metodo, descritor_metodo, args);

        if (pilha_execucao.frame_topo() != frame_corrente) {
            frame_corrente->load_operand_stack(backup_pilha_operandos);
            delete novo_frame;
            return;
        }
        pilha_execucao.empilhar_frame(novo_frame);
    }
    frame_corrente->pc += 3;
}

void Executor::invokestatic()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    stack<Valor> backup_pilha_operandos = frame_corrente->copy_operand_stack();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_metodo = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_metodo = constant_pool[indice_metodo - 1];
    assert(cp_metodo.tag == ConstMethodRef);
    ConstMethodRefInfo info_metodo = cp_metodo.info.method_ref_info;
    string nome_classe = formatar_constante(constant_pool, info_metodo.class_index);
    ConstantPoolInfo cp_nome_tipo = constant_pool[info_metodo.name_and_type_index - 1];
    assert(cp_nome_tipo.tag == ConstNameType);
    ConstNameTypeInfo nome_tipo_metodo = cp_nome_tipo.info.name_type_info;
    string nome_metodo = formatar_constante(constant_pool, nome_tipo_metodo.name_index);
    string descritor_metodo = formatar_constante(constant_pool, nome_tipo_metodo.descriptor_index);
    if (nome_classe == "java/lang/Object" && nome_metodo == "registerNatives") {
        frame_corrente->pc += 3;
        return;
    }
    if (nome_classe.find("java/") != string::npos) {
        cerr << "Call to invalid static method: " << nome_metodo << endl;
        exit(1);
    } else {
        uint16_t nargs = 0;
        uint16_t i = 1;
        while (descritor_metodo[i] != ')') {
            char tipo_base = descritor_metodo[i];
            if (tipo_base == 'D' || tipo_base == 'J') {
                nargs += 2;
            } else if (tipo_base == 'L') {
                nargs++;
                while (descritor_metodo[++i] != ';')
                    ;
            } else if (tipo_base == '[') {
                nargs++;
                while (descritor_metodo[++i] == '[')
                    ;
                if (descritor_metodo[i] == 'L')
                    while (descritor_metodo[++i] != ';')
                        ;
            } else {
                nargs++;
            }
            i++;
        }
        vector<Valor> args;
        for (int i = 0; i < nargs; i++) {
            Valor valor = frame_corrente->pop_operand_stack();
            if (valor.tipo == PADDING) {
                args.insert(args.begin() + 1, valor);
            } else {
                args.insert(args.begin(), valor);
            }
        }
        AreaMetodos& area_metodos = AreaMetodos::instancia();
        ClasseEstatica* classe_runtime = area_metodos.carregar_classe(nome_classe);
        Frame* novo_frame = new Frame(classe_runtime, nome_metodo, descritor_metodo, args);

        if (pilha_execucao.frame_topo() != frame_corrente) {
            frame_corrente->load_operand_stack(backup_pilha_operandos);
            delete novo_frame;
            return;
        }
        pilha_execucao.empilhar_frame(novo_frame);
    }
    frame_corrente->pc += 3;
}

void Executor::invokeinterface()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    stack<Valor> backup_pilha_operandos = frame_corrente->copy_operand_stack();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_metodo = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_metodo = constant_pool[indice_metodo - 1];
    assert(cp_metodo.tag == ConstMethodRef || cp_metodo.tag == ConstInterfaceMethodRef);
    ConstMethodRefInfo info_metodo = cp_metodo.info.method_ref_info;
    string nome_classe = formatar_constante(constant_pool, info_metodo.class_index);
    ConstantPoolInfo cp_nome_tipo = constant_pool[info_metodo.name_and_type_index - 1];
    assert(cp_nome_tipo.tag == ConstNameType);
    ConstNameTypeInfo nome_tipo_metodo = cp_nome_tipo.info.name_type_info;
    string nome_metodo = formatar_constante(constant_pool, nome_tipo_metodo.name_index);
    string descritor_metodo = formatar_constante(constant_pool, nome_tipo_metodo.descriptor_index);
    if (nome_classe.find("java/") != string::npos) {
        cerr << "Call to invalid interface method: " << nome_metodo << endl;
        exit(1);
    } else {
        uint16_t nargs = 0;
        uint16_t i = 1;
        while (descritor_metodo[i] != ')') {
            char tipo_base = descritor_metodo[i];
            if (tipo_base == 'D' || tipo_base == 'J') {
                nargs += 2;
            } else if (tipo_base == 'L') {
                nargs++;
                while (descritor_metodo[++i] != ';')
                    ;
            } else if (tipo_base == '[') {
                nargs++;
                while (descritor_metodo[++i] == '[')
                    ;
                if (descritor_metodo[i] == 'L')
                    while (descritor_metodo[++i] != ';')
                        ;
            } else {
                nargs++;
            }
            i++;
        }
        vector<Valor> args;
        for (int i = 0; i < nargs; i++) {
            Valor valor = frame_corrente->pop_operand_stack();
            if (valor.tipo == PADDING) {
                args.insert(args.begin() + 1, valor);
            } else {
                args.insert(args.begin(), valor);
            }
        }
        Valor valor_objeto = frame_corrente->pop_operand_stack();
        assert(valor_objeto.tipo == REFERENCIA);
        args.insert(args.begin(), valor_objeto);
        Objeto* objeto = valor_objeto.dados.objeto;
        assert(objeto->tipo_objeto() == INSTANCIA_CLASSE);
        ClasseInstancia* instance = (ClasseInstancia*)objeto;
        AreaMetodos& area_metodos = AreaMetodos::instancia();
        area_metodos.carregar_classe(nome_classe);
        Frame* novo_frame = new Frame(instance, instance->get_classe_runtime(), nome_metodo, descritor_metodo, args);

        if (pilha_execucao.frame_topo() != frame_corrente) {
            frame_corrente->load_operand_stack(backup_pilha_operandos);
            delete novo_frame;
            return;
        }
        pilha_execucao.empilhar_frame(novo_frame);
    }
    frame_corrente->pc += 5;
}

void Executor::func_new()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_classe = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_classe = constant_pool[indice_classe - 1];
    assert(cp_classe.tag == ConstClass);
    ConstClassInfo info_classe = cp_classe.info.class_info;
    string nome_classe = formatar_constante(constant_pool, info_classe.name_index);
    Objeto* objeto;
    if (nome_classe == "java/lang/String") {
        objeto = new ObjetoString();
    } else {
        AreaMetodos& area_metodos = AreaMetodos::instancia();
        ClasseEstatica* classe_runtime = area_metodos.carregar_classe(nome_classe);
        objeto = new ClasseInstancia(classe_runtime);
    }

    Valor objectref = faz_valor_referencia(objeto);
    frame_corrente->push_operand_stack(objectref);
    frame_corrente->pc += 3;
}

void Executor::newarray()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor contagem = frame_corrente->pop_operand_stack();
    assert(contagem.tipo == INT);
    if (contagem.dados.valor_int < 0) {
        cerr << "NegativeArranjoSizeException" << endl;
        exit(1);
    }
    Arranjo* arranjo = nullptr;
    Valor valor;
    valor.dados.valor_long = 0;
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    switch (codigo[1]) {
    case 4:
        arranjo = new Arranjo(BOOLEANO);
        valor = faz_valor_booleano(false);
        for (int i = 0; i < contagem.dados.valor_int; i++) {
            arranjo->push_value(valor);
        }
        break;
    case 5:
        arranjo = new Arranjo(CHAR);
        valor = faz_valor_char(0);
        for (int i = 0; i < contagem.dados.valor_int; i++) {
            arranjo->push_value(valor);
        }
        break;
    case 6:
        arranjo = new Arranjo(FLOAT);
        valor = faz_valor_float(0);
        for (int i = 0; i < contagem.dados.valor_int; i++) {
            arranjo->push_value(valor);
        }
        break;
    case 7:
        arranjo = new Arranjo(DOUBLE);
        valor = faz_valor_double(0);
        for (int i = 0; i < contagem.dados.valor_int; i++) {
            arranjo->push_value(valor);
        }
        break;
    case 8:
        arranjo = new Arranjo(BYTE);
        valor = faz_valor_byte(0);
        for (int i = 0; i < contagem.dados.valor_int; i++) {
            arranjo->push_value(valor);
        }
        break;
    case 9:
        arranjo = new Arranjo(SHORT);
        valor = faz_valor_short(0);
        for (int i = 0; i < contagem.dados.valor_int; i++) {
            arranjo->push_value(valor);
        }
        break;
    case 10:
        arranjo = new Arranjo(INT);
        valor = faz_valor_int(0);
        for (int i = 0; i < contagem.dados.valor_int; i++) {
            arranjo->push_value(valor);
        }
        break;
    case 11:
        arranjo = new Arranjo(LONG);
        valor = faz_valor_long(0);
        for (int i = 0; i < contagem.dados.valor_int; i++) {
            arranjo->push_value(valor);
        }
        break;
    }
    Valor arrayref = faz_valor_referencia(arranjo);
    frame_corrente->push_operand_stack(arrayref);
    frame_corrente->pc += 2;
}

void Executor::anewarray()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor contagem = frame_corrente->pop_operand_stack();
    assert(contagem.tipo == INT);
    if (contagem.dados.valor_int < 0) {
        cerr << "NegativeArranjoSizeException" << endl;
        exit(1);
    }
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    uint16_t indice_classe = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_classe = constant_pool[indice_classe - 1];
    assert(cp_classe.tag == ConstClass);
    ConstClassInfo info_classe = cp_classe.info.class_info;
    string nome_classe = formatar_constante(constant_pool, info_classe.name_index);
    if (nome_classe != "java/lang/String") {
        int i = 0;
        while (nome_classe[i] == '[')
            i++;
        if (nome_classe[i] == 'L') {
            AreaMetodos& area_metodos = AreaMetodos::instancia();
            area_metodos.carregar_classe(nome_classe.substr(i + 1, nome_classe.size() - i - 2));
        }
    }

    Valor objectref = faz_valor_referencia(new Arranjo(REFERENCIA));

    Valor valor_null = faz_valor_referencia(NULL);
    for (int i = 0; i < contagem.dados.valor_int; i++) {
        ((Arranjo*)objectref.dados.objeto)->push_value(valor_null);
    }
    frame_corrente->push_operand_stack(objectref);
    frame_corrente->pc += 3;
}

void Executor::arraylength()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor arrayref = frame_corrente->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    if (arrayref.dados.objeto == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    Valor length = faz_valor_int((int)((Arranjo*)arrayref.dados.objeto)->get_size());
    frame_corrente->push_operand_stack(length);
    frame_corrente->pc++;
}

void Executor::athrow()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    frame_corrente->pc++;
}

void Executor::checkcast()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    AreaMetodos& area_metodos = AreaMetodos::instancia();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    u2 indice_cp = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    ConstantPoolInfo elemento_cp = constant_pool[indice_cp - 1];
    assert(elemento_cp.tag == ConstClass);
    string nome_classe = formatar_constante(constant_pool, indice_cp);
    Valor valor_objectref = frame_corrente->pop_operand_stack();
    assert(valor_objectref.tipo == REFERENCIA);
    Valor valor_resultado = faz_valor_int(0);
    if (valor_objectref.dados.objeto == NULL) {
        cerr << "ClassCastException" << endl;
        exit(1);
    } else {
        Objeto* obj = valor_objectref.dados.objeto;
        if (obj->tipo_objeto() == INSTANCIA_CLASSE) {
            ClasseInstancia* instancia_classe = (ClasseInstancia*)obj;
            ClasseEstatica* classe_runtime = instancia_classe->get_classe_runtime();
            bool found = false;
            while (!found) {
                ArquivoClasse* arquivo_classe = classe_runtime->get_arquivo_classe();
                string nome_classe_atual = formatar_constante(arquivo_classe->constant_pool, arquivo_classe->this_class);
                if (nome_classe_atual == nome_classe) {
                    found = true;
                } else {
                    if (arquivo_classe->super_class == 0) {
                        break;
                    } else {
                        string nome_super_classe = formatar_constante(arquivo_classe->constant_pool, arquivo_classe->this_class);
                        classe_runtime = area_metodos.carregar_classe(nome_super_classe);
                    }
                }
            }
            valor_resultado.dados.valor_int = found ? 1 : 0;
        } else if (obj->tipo_objeto() == INSTANCIA_STRING) {
            valor_resultado.dados.valor_int = (nome_classe == "java/lang/String" || nome_classe == "java/lang/Object") ? 1 : 0;
        } else {
            if (nome_classe == "java/lang/Object") {
                valor_resultado.dados.valor_int = 1;
            } else {
                valor_resultado.dados.valor_int = 0;
            }
        }
    }
    frame_corrente->push_operand_stack(valor_resultado);
    frame_corrente->pc += 3;
}

void Executor::instanceof()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    AreaMetodos& area_metodos = AreaMetodos::instancia();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    u2 indice_cp = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    ConstantPoolInfo elemento_cp = constant_pool[indice_cp - 1];
    assert(elemento_cp.tag == ConstClass);
    string nome_classe = formatar_constante(constant_pool, indice_cp);
    Valor valor_objectref = frame_corrente->pop_operand_stack();
    assert(valor_objectref.tipo == REFERENCIA);
    Valor valor_resultado = faz_valor_int(0);
    if (valor_objectref.dados.objeto == NULL) {
        valor_resultado.dados.valor_int = 0;
    } else {
        Objeto* obj = valor_objectref.dados.objeto;
        if (obj->tipo_objeto() == INSTANCIA_CLASSE) {
            ClasseInstancia* instancia_classe = (ClasseInstancia*)obj;
            ClasseEstatica* classe_runtime = instancia_classe->get_classe_runtime();
            bool found = false;
            while (!found) {
                ArquivoClasse* arquivo_classe = classe_runtime->get_arquivo_classe();
                string nome_classe_atual = formatar_constante(arquivo_classe->constant_pool, arquivo_classe->this_class);
                if (nome_classe_atual == nome_classe) {
                    found = true;
                } else {
                    if (arquivo_classe->super_class == 0) {
                        break;
                    } else {
                        string nome_super_classe = formatar_constante(arquivo_classe->constant_pool, arquivo_classe->this_class);
                        classe_runtime = area_metodos.carregar_classe(nome_super_classe);
                    }
                }
            }
            valor_resultado.dados.valor_int = found ? 1 : 0;
        } else if (obj->tipo_objeto() == INSTANCIA_STRING) {
            valor_resultado.dados.valor_int = (nome_classe == "java/lang/String" || nome_classe == "java/lang/Object") ? 1 : 0;
        } else {
            if (nome_classe == "java/lang/Object") {
                valor_resultado.dados.valor_int = 1;
            } else {
                valor_resultado.dados.valor_int = 0;
            }
        }
    }
    frame_corrente->push_operand_stack(valor_resultado);
    frame_corrente->pc += 3;
}

void Executor::monitorenter()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    frame_corrente->pc++;
}

void Executor::monitorexit()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    frame_corrente->pc++;
}

void Executor::wide()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    is_wide = true;
    frame_corrente->pc++;
}

void Executor::multianewarray()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    ConstantPoolInfo* constant_pool = *(frame_corrente->get_constant_pool());
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    u1 dimensions = codigo[3];
    assert(dimensions >= 1);
    uint16_t indice_classe = (byte1 << 8) | byte2;
    ConstantPoolInfo cp_classe = constant_pool[indice_classe - 1];
    assert(cp_classe.tag == ConstClass);
    ConstClassInfo info_classe = cp_classe.info.class_info;
    string nome_classe = formatar_constante(constant_pool, info_classe.name_index);

    TipoValor tipo_valor;
    int i = 0;
    while (nome_classe[i] == '[')
        i++;
    string tipo_multiarranjo = nome_classe.substr(i + 1, nome_classe.size() - i - 2);
    switch (nome_classe[i]) {
    case 'L':
        if (tipo_multiarranjo != "java/lang/String") {
            AreaMetodos& area_metodos = AreaMetodos::instancia();
            area_metodos.carregar_classe(tipo_multiarranjo);
        }
        tipo_valor = REFERENCIA;
        break;
    case 'B':
        tipo_valor = BYTE;
        break;
    case 'C':
        tipo_valor = CHAR;
        break;
    case 'D':
        tipo_valor = DOUBLE;
        break;
    case 'F':
        tipo_valor = FLOAT;
        break;
    case 'I':
        tipo_valor = INT;
        break;
    case 'J':
        tipo_valor = LONG;
        break;
    case 'S':
        tipo_valor = SHORT;
        break;
    case 'Z':
        tipo_valor = BOOLEANO;
        break;
    default:
        cerr << "Invalid descriptor in multianewarray" << endl;
        exit(1);
    }
    stack<int> contagem;
    for (int i = 0; i < dimensions; i++) {
        Valor tamanho_dim = frame_corrente->pop_operand_stack();
        assert(tamanho_dim.tipo == INT);
        contagem.push(tamanho_dim.dados.valor_int);
    }
    Arranjo* arranjo = new Arranjo((dimensions > 1) ? REFERENCIA : tipo_valor);
    popula_multiarranjo(arranjo, tipo_valor, contagem);
    Valor valor_arranjo = faz_valor_referencia(arranjo);
    frame_corrente->push_operand_stack(valor_arranjo);
    frame_corrente->pc += 4;
}

void Executor::ifnull()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_referencia = frame_corrente->pop_operand_stack();
    assert(valor_referencia.tipo == REFERENCIA);
    if (valor_referencia.dados.objeto == NULL) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t branch = (byte1 << 8) | byte2;
        frame_corrente->pc += branch;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::ifnonnull()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    Valor valor_referencia = frame_corrente->pop_operand_stack();
    assert(valor_referencia.tipo == REFERENCIA);
    if (valor_referencia.dados.objeto != NULL) {
        u1* codigo = frame_corrente->get_code(frame_corrente->pc);
        u1 byte1 = codigo[1];
        u1 byte2 = codigo[2];
        int16_t branch = (byte1 << 8) | byte2;
        frame_corrente->pc += branch;
    } else {
        frame_corrente->pc += 3;
    }
}

void Executor::goto_w()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    u1 byte3 = codigo[3];
    u1 byte4 = codigo[4];
    int32_t deslocamento_desvio = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
    frame_corrente->pc += deslocamento_desvio;
}

void Executor::jsr_w()
{
    PilhaExecucao& pilha_execucao = PilhaExecucao::instancia();
    Frame* frame_corrente = pilha_execucao.frame_topo();
    u1* codigo = frame_corrente->get_code(frame_corrente->pc);
    u1 byte1 = codigo[1];
    u1 byte2 = codigo[2];
    u1 byte3 = codigo[3];
    u1 byte4 = codigo[4];
    int32_t deslocamento_desvio = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
    Valor endereco_retorno = faz_valor_endereco_retorno(frame_corrente->pc + 5);
    frame_corrente->push_operand_stack(endereco_retorno);
    frame_corrente->pc += deslocamento_desvio;
    assert((int32_t)frame_corrente->pc < (int32_t)frame_corrente->get_code_size());
}

/* ----------------------------------------------------------------------- */
/* Tabela de despacho                                                      */
/* ----------------------------------------------------------------------- */
void Executor::init_instrucoes()
{
    // Por padrao todo opcode cai no fallback de 'nao implementado'.
    for (int i = 0; i < 202; i++) {
        tabela_funcoes[i] = &Executor::instrucao_nao_implementada;
    }

    tabela_funcoes[0x00] = &Executor::nop;
    tabela_funcoes[0x01] = &Executor::aconst_null;
    tabela_funcoes[0x02] = &Executor::iconst_m1;
    tabela_funcoes[0x03] = &Executor::iconst_0;
    tabela_funcoes[0x04] = &Executor::iconst_1;
    tabela_funcoes[0x05] = &Executor::iconst_2;
    tabela_funcoes[0x06] = &Executor::iconst_3;
    tabela_funcoes[0x07] = &Executor::iconst_4;
    tabela_funcoes[0x08] = &Executor::iconst_5;
    tabela_funcoes[0x09] = &Executor::lconst_0;
    tabela_funcoes[0x0a] = &Executor::lconst_1;
    tabela_funcoes[0x0b] = &Executor::fconst_0;
    tabela_funcoes[0x0c] = &Executor::fconst_1;
    tabela_funcoes[0x0d] = &Executor::fconst_2;
    tabela_funcoes[0x0e] = &Executor::dconst_0;
    tabela_funcoes[0x0f] = &Executor::dconst_1;
    tabela_funcoes[0x10] = &Executor::bipush;
    tabela_funcoes[0x11] = &Executor::sipush;
    tabela_funcoes[0x12] = &Executor::ldc;
    tabela_funcoes[0x13] = &Executor::ldc_w;
    tabela_funcoes[0x14] = &Executor::ldc2_w;
    tabela_funcoes[0x15] = &Executor::iload;
    tabela_funcoes[0x16] = &Executor::lload;
    tabela_funcoes[0x17] = &Executor::fload;
    tabela_funcoes[0x18] = &Executor::dload;
    tabela_funcoes[0x19] = &Executor::aload;
    tabela_funcoes[0x1a] = &Executor::iload_0;
    tabela_funcoes[0x1b] = &Executor::iload_1;
    tabela_funcoes[0x1c] = &Executor::iload_2;
    tabela_funcoes[0x1d] = &Executor::iload_3;
    tabela_funcoes[0x1e] = &Executor::lload_0;
    tabela_funcoes[0x1f] = &Executor::lload_1;
    tabela_funcoes[0x20] = &Executor::lload_2;
    tabela_funcoes[0x21] = &Executor::lload_3;
    tabela_funcoes[0x22] = &Executor::fload_0;
    tabela_funcoes[0x23] = &Executor::fload_1;
    tabela_funcoes[0x24] = &Executor::fload_2;
    tabela_funcoes[0x25] = &Executor::fload_3;
    tabela_funcoes[0x26] = &Executor::dload_0;
    tabela_funcoes[0x27] = &Executor::dload_1;
    tabela_funcoes[0x28] = &Executor::dload_2;
    tabela_funcoes[0x29] = &Executor::dload_3;
    tabela_funcoes[0x2a] = &Executor::aload_0;
    tabela_funcoes[0x2b] = &Executor::aload_1;
    tabela_funcoes[0x2c] = &Executor::aload_2;
    tabela_funcoes[0x2d] = &Executor::aload_3;
    tabela_funcoes[0x2e] = &Executor::iaload;
    tabela_funcoes[0x2f] = &Executor::laload;
    tabela_funcoes[0x30] = &Executor::faload;
    tabela_funcoes[0x31] = &Executor::daload;
    tabela_funcoes[0x32] = &Executor::aaload;
    tabela_funcoes[0x33] = &Executor::baload;
    tabela_funcoes[0x34] = &Executor::caload;
    tabela_funcoes[0x35] = &Executor::saload;
    tabela_funcoes[0x36] = &Executor::istore;
    tabela_funcoes[0x37] = &Executor::lstore;
    tabela_funcoes[0x38] = &Executor::fstore;
    tabela_funcoes[0x39] = &Executor::dstore;
    tabela_funcoes[0x3a] = &Executor::astore;
    tabela_funcoes[0x3b] = &Executor::istore_0;
    tabela_funcoes[0x3c] = &Executor::istore_1;
    tabela_funcoes[0x3d] = &Executor::istore_2;
    tabela_funcoes[0x3e] = &Executor::istore_3;
    tabela_funcoes[0x3f] = &Executor::lstore_0;
    tabela_funcoes[0x40] = &Executor::lstore_1;
    tabela_funcoes[0x41] = &Executor::lstore_2;
    tabela_funcoes[0x42] = &Executor::lstore_3;
    tabela_funcoes[0x43] = &Executor::fstore_0;
    tabela_funcoes[0x44] = &Executor::fstore_1;
    tabela_funcoes[0x45] = &Executor::fstore_2;
    tabela_funcoes[0x46] = &Executor::fstore_3;
    tabela_funcoes[0x47] = &Executor::dstore_0;
    tabela_funcoes[0x48] = &Executor::dstore_1;
    tabela_funcoes[0x49] = &Executor::dstore_2;
    tabela_funcoes[0x4a] = &Executor::dstore_3;
    tabela_funcoes[0x4b] = &Executor::astore_0;
    tabela_funcoes[0x4c] = &Executor::astore_1;
    tabela_funcoes[0x4d] = &Executor::astore_2;
    tabela_funcoes[0x4e] = &Executor::astore_3;
    tabela_funcoes[0x4f] = &Executor::iastore;
    tabela_funcoes[0x50] = &Executor::lastore;
    tabela_funcoes[0x51] = &Executor::fastore;
    tabela_funcoes[0x52] = &Executor::dastore;
    tabela_funcoes[0x53] = &Executor::aastore;
    tabela_funcoes[0x54] = &Executor::bastore;
    tabela_funcoes[0x55] = &Executor::castore;
    tabela_funcoes[0x56] = &Executor::sastore;
    tabela_funcoes[0x57] = &Executor::pop;
    tabela_funcoes[0x58] = &Executor::pop2;
    tabela_funcoes[0x59] = &Executor::dup;
    tabela_funcoes[0x5a] = &Executor::dup2_x1;
    tabela_funcoes[0x5b] = &Executor::dup2_x2;
    tabela_funcoes[0x5c] = &Executor::dup2;
    tabela_funcoes[0x5d] = &Executor::dup2_x1;
    tabela_funcoes[0x5e] = &Executor::dup2_x2;
    tabela_funcoes[0x5f] = &Executor::swap;
    tabela_funcoes[0x60] = &Executor::iadd;
    tabela_funcoes[0x61] = &Executor::ladd;
    tabela_funcoes[0x62] = &Executor::fadd;
    tabela_funcoes[0x63] = &Executor::dadd;
    tabela_funcoes[0x64] = &Executor::isub;
    tabela_funcoes[0x65] = &Executor::lsub;
    tabela_funcoes[0x66] = &Executor::fsub;
    tabela_funcoes[0x67] = &Executor::dsub;
    tabela_funcoes[0x68] = &Executor::imul;
    tabela_funcoes[0x69] = &Executor::lmul;
    tabela_funcoes[0x6a] = &Executor::fmul;
    tabela_funcoes[0x6b] = &Executor::dmul;
    tabela_funcoes[0x6c] = &Executor::idiv;
    tabela_funcoes[0x6d] = &Executor::ldiv;
    tabela_funcoes[0x6e] = &Executor::fdiv;
    tabela_funcoes[0x6f] = &Executor::ddiv;
    tabela_funcoes[0x70] = &Executor::irem;
    tabela_funcoes[0x71] = &Executor::lrem;
    tabela_funcoes[0x72] = &Executor::frem;
    tabela_funcoes[0x73] = &Executor::drem;
    tabela_funcoes[0x74] = &Executor::ineg;
    tabela_funcoes[0x75] = &Executor::lneg;
    tabela_funcoes[0x76] = &Executor::fneg;
    tabela_funcoes[0x77] = &Executor::dneg;
    tabela_funcoes[0x78] = &Executor::ishl;
    tabela_funcoes[0x79] = &Executor::lshl;
    tabela_funcoes[0x7a] = &Executor::ishr;
    tabela_funcoes[0x7b] = &Executor::lshr;
    tabela_funcoes[0x7c] = &Executor::iushr;
    tabela_funcoes[0x7d] = &Executor::lushr;
    tabela_funcoes[0x7e] = &Executor::iand;
    tabela_funcoes[0x7f] = &Executor::land;
    tabela_funcoes[0x80] = &Executor::ior;
    tabela_funcoes[0x81] = &Executor::lor;
    tabela_funcoes[0x82] = &Executor::ixor;
    tabela_funcoes[0x83] = &Executor::lxor;
    tabela_funcoes[0x84] = &Executor::iinc;
    tabela_funcoes[0x85] = &Executor::i2l;
    tabela_funcoes[0x86] = &Executor::i2f;
    tabela_funcoes[0x87] = &Executor::i2d;
    tabela_funcoes[0x88] = &Executor::l2i;
    tabela_funcoes[0x89] = &Executor::l2f;
    tabela_funcoes[0x8a] = &Executor::l2d;
    tabela_funcoes[0x8b] = &Executor::f2i;
    tabela_funcoes[0x8c] = &Executor::f2l;
    tabela_funcoes[0x8d] = &Executor::f2d;
    tabela_funcoes[0x8e] = &Executor::d2i;
    tabela_funcoes[0x8f] = &Executor::d2l;
    tabela_funcoes[0x90] = &Executor::d2f;
    tabela_funcoes[0x91] = &Executor::i2b;
    tabela_funcoes[0x92] = &Executor::i2c;
    tabela_funcoes[0x93] = &Executor::i2s;
    tabela_funcoes[0x94] = &Executor::lcmp;
    tabela_funcoes[0x95] = &Executor::fcmpl;
    tabela_funcoes[0x96] = &Executor::fcmpg;
    tabela_funcoes[0x97] = &Executor::dcmpl;
    tabela_funcoes[0x98] = &Executor::dcmpg;
    tabela_funcoes[0x99] = &Executor::ifeq;
    tabela_funcoes[0x9a] = &Executor::ifne;
    tabela_funcoes[0x9b] = &Executor::iflt;
    tabela_funcoes[0x9c] = &Executor::ifge;
    tabela_funcoes[0x9d] = &Executor::ifgt;
    tabela_funcoes[0x9e] = &Executor::ifle;
    tabela_funcoes[0x9f] = &Executor::if_icmpeq;
    tabela_funcoes[0xa0] = &Executor::if_icmpne;
    tabela_funcoes[0xa1] = &Executor::if_icmplt;
    tabela_funcoes[0xa2] = &Executor::if_icmpge;
    tabela_funcoes[0xa3] = &Executor::if_icmpgt;
    tabela_funcoes[0xa4] = &Executor::if_icmple;
    tabela_funcoes[0xa5] = &Executor::if_acmpeq;
    tabela_funcoes[0xa6] = &Executor::if_acmpne;
    tabela_funcoes[0xa7] = &Executor::func_goto;
    tabela_funcoes[0xa8] = &Executor::jsr;
    tabela_funcoes[0xa9] = &Executor::ret;
    tabela_funcoes[0xaa] = &Executor::tableswitch;
    tabela_funcoes[0xab] = &Executor::lookupswitch;
    tabela_funcoes[0xac] = &Executor::ireturn;
    tabela_funcoes[0xad] = &Executor::lreturn;
    tabela_funcoes[0xae] = &Executor::freturn;
    tabela_funcoes[0xaf] = &Executor::dreturn;
    tabela_funcoes[0xb0] = &Executor::areturn;
    tabela_funcoes[0xb1] = &Executor::func_return;
    tabela_funcoes[0xb2] = &Executor::getstatic;
    tabela_funcoes[0xb3] = &Executor::putstatic;
    tabela_funcoes[0xb4] = &Executor::getfield;
    tabela_funcoes[0xb5] = &Executor::putfield;
    tabela_funcoes[0xb6] = &Executor::invokevirtual;
    tabela_funcoes[0xb7] = &Executor::invokespecial;
    tabela_funcoes[0xb8] = &Executor::invokestatic;
    tabela_funcoes[0xb9] = &Executor::invokeinterface;
    tabela_funcoes[0xbb] = &Executor::func_new;
    tabela_funcoes[0xbc] = &Executor::newarray;
    tabela_funcoes[0xbd] = &Executor::anewarray;
    tabela_funcoes[0xbe] = &Executor::arraylength;
    tabela_funcoes[0xbf] = &Executor::athrow;
    tabela_funcoes[0xc0] = &Executor::checkcast;
    tabela_funcoes[0xc1] = &Executor::instanceof;
    tabela_funcoes[0xc2] = &Executor::monitorenter;
    tabela_funcoes[0xc3] = &Executor::monitorexit;
    tabela_funcoes[0xc4] = &Executor::wide;
    tabela_funcoes[0xc5] = &Executor::multianewarray;
    tabela_funcoes[0xc6] = &Executor::ifnull;
    tabela_funcoes[0xc7] = &Executor::ifnonnull;
    tabela_funcoes[0xc8] = &Executor::goto_w;
    tabela_funcoes[0xc9] = &Executor::jsr_w;
}
