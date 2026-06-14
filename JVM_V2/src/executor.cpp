/*
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
void Executor::popula_multiarranjo(Arranjo* array, TipoValor value_type, stack<int> count)
{
    int curr_count = count.top();
    count.pop();
    TipoValor array_type = (count.size() > 1) ? REFERENCIA : value_type;
    if (count.size() == 0) {
        for (int i = 0; i < curr_count; i++) {
            Valor sub_array_value = faz_valor_long(0);
            array->push_value(sub_array_value);
        }
    } else {
        for (int i = 0; i < curr_count; i++) {
            Arranjo* subarray = new Arranjo(array_type);
            popula_multiarranjo(subarray, value_type, count);
            Valor sub_array_value = faz_valor_referencia(subarray);
            array->push_value(sub_array_value);
        }
    }
}

/* ----------------------------------------------------------------------- */
/* Opcodes                                                                 */
/* ----------------------------------------------------------------------- */
void Executor::nop()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    top_frame->pc++;
}

void Executor::aconst_null()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_referencia(NULL);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iconst_m1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_int(-1);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iconst_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_int(0);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iconst_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_int(1);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iconst_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_int(2);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iconst_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_int(3);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iconst_4()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_int(4);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iconst_5()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_int(5);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::lconst_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor padding = faz_valor_padding();
    Valor value = faz_valor_long(0);
    top_frame->push_operand_stack(padding);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::lconst_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor padding = faz_valor_padding();
    Valor value = faz_valor_long(1);
    top_frame->push_operand_stack(padding);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::fconst_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_float(0);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::fconst_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_float(1);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::fconst_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = faz_valor_float(2);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::dconst_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor padding = faz_valor_padding();
    Valor value = faz_valor_double(0);
    top_frame->push_operand_stack(padding);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::dconst_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor padding = faz_valor_padding();
    Valor value = faz_valor_double(1);
    top_frame->push_operand_stack(padding);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::bipush()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte = code[1];
    Valor value = faz_valor_int((int32_t)(int8_t)byte);
    value.tipo_print = BYTE;
    top_frame->push_operand_stack(value);
    top_frame->pc += 2;
}

void Executor::sipush()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t short_value = (byte1 << 8) | byte2;
    Valor value = faz_valor_int((int32_t)(int16_t)short_value);
    value.tipo_print = SHORT;
    top_frame->push_operand_stack(value);
    top_frame->pc += 3;
}

void Executor::ldc()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 index = code[1];
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    ConstantPoolInfo entry = constant_pool[index - 1];
    Valor value;
    if (entry.tag == ConstStr) {
        ConstantPoolInfo utf8_entry = constant_pool[entry.info.str_info.string_index - 1];
        assert(utf8_entry.tag == ConstUtf8);
        u1* bytes = utf8_entry.info.utf8_info.bytes;
        char utf8_str[utf8_entry.info.utf8_info.length + 1];
        int i;
        for (i = 0; i < utf8_entry.info.utf8_info.length; i++) {
            utf8_str[i] = bytes[i];
        }
        utf8_str[i] = '\0';
        value = faz_valor_referencia(new ObjetoString(utf8_str));
    } else if (entry.tag == ConstInt) {
        value = faz_valor_int((int32_t)entry.info.int_info.bytes);
    } else if (entry.tag == ConstFloat) {
        u4 floatBytes = entry.info.float_info.bytes;
        int s = ((floatBytes >> 31) == 0) ? 1 : -1;
        int e = ((floatBytes >> 23) & 0xff);
        int m = (e == 0) ? (floatBytes & 0x7fffff) << 1 : (floatBytes & 0x7fffff) | 0x800000;
        float number = s * m * pow(2, e - 150);
        value = faz_valor_float(number);
    } else {
        
        cerr << "ldc trying to access invalid constant pool element " << entry.tag << endl;
        exit(1);
    }
    top_frame->push_operand_stack(value);
    top_frame->pc += 2;
}

void Executor::ldc_w()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u2 index = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    ConstantPoolInfo entry = constant_pool[index - 1];
    Valor value;
    if (entry.tag == ConstStr) {
        ConstantPoolInfo utf8_entry = constant_pool[entry.info.str_info.string_index - 1];
        assert(utf8_entry.tag == ConstUtf8);
        u1* bytes = utf8_entry.info.utf8_info.bytes;
        char utf8_str[utf8_entry.info.utf8_info.length + 1];
        int i;
        for (i = 0; i < utf8_entry.info.utf8_info.length; i++) {
            utf8_str[i] = bytes[i];
        }
        utf8_str[i] = '\0';
        value = faz_valor_referencia(new ObjetoString(utf8_str));
    } else if (entry.tag == ConstInt) {
        value = faz_valor_int((int)entry.info.int_info.bytes);
    } else if (entry.tag == ConstFloat) {
        u4 floatBytes = entry.info.float_info.bytes;
        int s = ((floatBytes >> 31) == 0) ? 1 : -1;
        int e = ((floatBytes >> 23) & 0xff);
        int m = (e == 0) ? (floatBytes & 0x7fffff) << 1 : (floatBytes & 0x7fffff) | 0x800000;
        float number = s * m * pow(2, e - 150);
        value = faz_valor_float(number);
    } else {
        cerr << "ldc_w trying to access invalid constant pool element " << entry.tag << endl;
        exit(1);
    }
    top_frame->push_operand_stack(value);
    top_frame->pc += 3;
}

void Executor::ldc2_w()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u2 index = (byte1 << 8) | byte2;
    ConstantPoolInfo* class_file = *(top_frame->get_constant_pool());
    ConstantPoolInfo entry = class_file[index - 1];
    Valor value;
    if (entry.tag == ConstLong) {
        u4 highBytes = entry.info.long_info.high_bytes;
        u4 lowBytes = entry.info.long_info.low_bytes;
        int64_t longNumber = ((int64_t)highBytes << 32) + lowBytes;
        value = faz_valor_long(longNumber);
        Valor padding = faz_valor_padding();
        top_frame->push_operand_stack(padding);
    } else if (entry.tag == ConstDouble) {
        u4 highBytes = entry.info.double_info.high_bytes;
        u4 lowBytes = entry.info.double_info.low_bytes;
        int64_t longNumber = ((int64_t)highBytes << 32) + lowBytes;
        int32_t s = ((longNumber >> 63) == 0) ? 1 : -1;
        int32_t e = (int32_t)((longNumber >> 52) & 0x7ffL);
        int64_t m = (e == 0) ? (longNumber & 0xfffffffffffffL) << 1 : (longNumber & 0xfffffffffffffL) | 0x10000000000000L;
        double doubleNumber = s * m * pow(2, e - 1075);
        value = faz_valor_double(doubleNumber);
        Valor padding = faz_valor_padding();
        top_frame->push_operand_stack(padding);
    } else {
        cerr << "ldc2_w trying to access invalid constant pool element " << entry.tag << endl;
        exit(1);
    }
    top_frame->push_operand_stack(value);
    top_frame->pc += 3;
}

void Executor::iload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > index));
    Valor value = top_frame->get_local_variable_value(index);
    assert(value.tipo == INT);
    top_frame->push_operand_stack(value);
}

void Executor::lload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > (index + 1)));
    Valor value = top_frame->get_local_variable_value(index);
    assert(value.tipo == LONG);
    Valor padding = faz_valor_padding();
    top_frame->push_operand_stack(padding);
    top_frame->push_operand_stack(value);
}

void Executor::fload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > index));
    Valor value = top_frame->get_local_variable_value(index);
    assert(value.tipo == FLOAT);
    top_frame->push_operand_stack(value);
}

void Executor::dload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > (index + 1)));
    Valor value = top_frame->get_local_variable_value(index);
    assert(value.tipo == DOUBLE);
    Valor padding = faz_valor_padding();
    top_frame->push_operand_stack(padding);
    top_frame->push_operand_stack(value);
}

void Executor::aload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > index));
    Valor value = top_frame->get_local_variable_value(index);
    assert(value.tipo == REFERENCIA);
    top_frame->push_operand_stack(value);
}

void Executor::iload_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(0);
    assert(value.tipo == INT);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iload_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(1);
    assert(value.tipo == INT);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iload_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(2);
    assert(value.tipo == INT);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iload_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(3);
    assert(value.tipo == INT);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::lload_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value;
    value = top_frame->get_local_variable_value(1);
    assert(value.tipo == PADDING);
    top_frame->push_operand_stack(value);
    value = top_frame->get_local_variable_value(0);
    assert(value.tipo == LONG);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::lload_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value;
    value = top_frame->get_local_variable_value(2);
    assert(value.tipo == PADDING);
    top_frame->push_operand_stack(value);
    value = top_frame->get_local_variable_value(1);
    assert(value.tipo == LONG);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::lload_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value;
    value = top_frame->get_local_variable_value(3);
    assert(value.tipo == PADDING);
    top_frame->push_operand_stack(value);
    value = top_frame->get_local_variable_value(2);
    assert(value.tipo == LONG);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::lload_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value;
    value = top_frame->get_local_variable_value(4);
    assert(value.tipo == PADDING);
    top_frame->push_operand_stack(value);
    value = top_frame->get_local_variable_value(3);
    assert(value.tipo == LONG);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::fload_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(0);
    assert(value.tipo == FLOAT);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::fload_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(1);
    assert(value.tipo == FLOAT);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::fload_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(2);
    assert(value.tipo == FLOAT);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::fload_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(3);
    assert(value.tipo == FLOAT);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::dload_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value;
    value = top_frame->get_local_variable_value(1);
    assert(value.tipo == PADDING);
    top_frame->push_operand_stack(value);
    value = top_frame->get_local_variable_value(0);
    assert(value.tipo == DOUBLE);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::dload_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value;
    value = top_frame->get_local_variable_value(2);
    assert(value.tipo == PADDING);
    top_frame->push_operand_stack(value);
    value = top_frame->get_local_variable_value(1);
    assert(value.tipo == DOUBLE);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::dload_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value;
    value = top_frame->get_local_variable_value(3);
    assert(value.tipo == PADDING);
    top_frame->push_operand_stack(value);
    value = top_frame->get_local_variable_value(2);
    assert(value.tipo == DOUBLE);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::dload_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value;
    value = top_frame->get_local_variable_value(4);
    assert(value.tipo == PADDING);
    top_frame->push_operand_stack(value);
    value = top_frame->get_local_variable_value(3);
    assert(value.tipo == DOUBLE);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::aload_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(0);
    assert(value.tipo == REFERENCIA);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::aload_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(1);
    assert(value.tipo == REFERENCIA);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::aload_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(2);
    assert(value.tipo == REFERENCIA);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::aload_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->get_local_variable_value(3);
    assert(value.tipo == REFERENCIA);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::iaload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    top_frame->push_operand_stack(array->get_value(index.dados.valor_int));
    top_frame->pc++;
}

void Executor::laload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if ((signed)index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor padding = faz_valor_padding();
    top_frame->push_operand_stack(padding);
    top_frame->push_operand_stack(array->get_value(index.dados.valor_int));
    top_frame->pc++;
}

void Executor::faload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    top_frame->push_operand_stack(array->get_value(index.dados.valor_int));
    top_frame->pc++;
}

void Executor::daload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor padding = faz_valor_padding();
    top_frame->push_operand_stack(padding);
    top_frame->push_operand_stack(array->get_value(index.dados.valor_int));
    top_frame->pc++;
}

void Executor::aaload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    top_frame->push_operand_stack(array->get_value(index.dados.valor_int));
    top_frame->pc++;
}

void Executor::baload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor value = array->get_value(index.dados.valor_int);
    assert(value.tipo == BOOLEANO || value.tipo == BYTE);
    if (value.tipo == BOOLEANO) {
        value = faz_valor_int(value.dados.valor_booleano);
    value.tipo_print = BOOLEANO;
    } else {
        value = faz_valor_int((int32_t)value.dados.valor_byte);
    value.tipo_print = BYTE;
    }
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::caload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor char_value = array->get_value(index.dados.valor_int);
    char_value = faz_valor_int(char_value.dados.valor_char);
    char_value.tipo_print = CHAR;
    top_frame->push_operand_stack(char_value);
    top_frame->pc++;
}

void Executor::saload()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Valor short_value = array->get_value(index.dados.valor_int);
    short_value = faz_valor_int((int32_t)short_value.dados.valor_short);
    short_value.tipo_print = SHORT;
    top_frame->push_operand_stack(short_value);
    top_frame->pc++;
}

void Executor::istore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > index));
    top_frame->set_local_variable(value, index);
}

void Executor::lstore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == LONG);
    top_frame->pop_operand_stack();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > (index + 1)));
    top_frame->set_local_variable(value, index);
    Valor padding = faz_valor_padding();
    top_frame->set_local_variable(padding, index + 1);
}

void Executor::fstore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == FLOAT);
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > index));
    top_frame->set_local_variable(value, index);
}

void Executor::dstore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == DOUBLE);
    top_frame->pop_operand_stack();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > (index + 1)));
    top_frame->set_local_variable(value, index);
    Valor padding = faz_valor_padding();
    top_frame->set_local_variable(padding, index + 1);
}

void Executor::astore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == REFERENCIA);
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    int16_t index = (int16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
        top_frame->pc += 3;
        is_wide = false;
    } else {
        top_frame->pc += 2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > index));
    top_frame->set_local_variable(value, index);
}

void Executor::istore_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    top_frame->set_local_variable(value, 0);
    top_frame->pc++;
}

void Executor::istore_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    top_frame->set_local_variable(value, 1);
    top_frame->pc++;
}

void Executor::istore_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    top_frame->set_local_variable(value, 2);
    top_frame->pc++;
}

void Executor::istore_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    top_frame->set_local_variable(value, 3);
    top_frame->pc++;
}

void Executor::lstore_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == LONG);
    top_frame->set_local_variable(value, 0);
    value = top_frame->pop_operand_stack();
    assert(value.tipo == PADDING);
    top_frame->set_local_variable(value, 1);
    top_frame->pc++;
}

void Executor::lstore_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == LONG);
    top_frame->set_local_variable(value, 1);
    value = top_frame->pop_operand_stack();
    assert(value.tipo == PADDING);
    top_frame->set_local_variable(value, 2);
    top_frame->pc++;
}

void Executor::lstore_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == LONG);
    top_frame->set_local_variable(value, 2);
    value = top_frame->pop_operand_stack();
    assert(value.tipo == PADDING);
    top_frame->set_local_variable(value, 3);
    top_frame->pc++;
}

void Executor::lstore_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == LONG);
    top_frame->set_local_variable(value, 3);
    value = top_frame->pop_operand_stack();
    assert(value.tipo == PADDING);
    top_frame->set_local_variable(value, 4);
    top_frame->pc++;
}

void Executor::fstore_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == FLOAT);
    top_frame->set_local_variable(value, 0);
    top_frame->pc++;
}

void Executor::fstore_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == FLOAT);
    top_frame->set_local_variable(value, 1);
    top_frame->pc++;
}

void Executor::fstore_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == FLOAT);
    top_frame->set_local_variable(value, 2);
    top_frame->pc++;
}

void Executor::fstore_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == FLOAT);
    top_frame->set_local_variable(value, 3);
    top_frame->pc++;
}

void Executor::dstore_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == DOUBLE);
    top_frame->set_local_variable(value, 0);
    value = top_frame->pop_operand_stack();
    assert(value.tipo == PADDING);
    top_frame->set_local_variable(value, 1);
    top_frame->pc++;
}

void Executor::dstore_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == DOUBLE);
    top_frame->set_local_variable(value, 1);
    value = top_frame->pop_operand_stack();
    assert(value.tipo == PADDING);
    top_frame->set_local_variable(value, 2);
    top_frame->pc++;
}

void Executor::dstore_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == DOUBLE);
    top_frame->set_local_variable(value, 2);
    value = top_frame->pop_operand_stack();
    assert(value.tipo == PADDING);
    top_frame->set_local_variable(value, 3);
    top_frame->pc++;
}

void Executor::dstore_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == DOUBLE);
    top_frame->set_local_variable(value, 3);
    value = top_frame->pop_operand_stack();
    assert(value.tipo == PADDING);
    top_frame->set_local_variable(value, 4);
    top_frame->pc++;
}

void Executor::astore_0()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == REFERENCIA);
    top_frame->set_local_variable(value, 0);
    top_frame->pc++;
}

void Executor::astore_1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == REFERENCIA);
    top_frame->set_local_variable(value, 1);
    top_frame->pc++;
}

void Executor::astore_2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == REFERENCIA);
    top_frame->set_local_variable(value, 2);
    top_frame->pc++;
}

void Executor::astore_3()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == REFERENCIA);
    top_frame->set_local_variable(value, 3);
    top_frame->pc++;
}

void Executor::iastore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int >= (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    value.tipo_print = INT;
    assert(value.tipo == array->array_content_type());
    array->change_value(index.dados.valor_int, value);
    top_frame->pc++;
}

void Executor::lastore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == LONG);
    Valor padding = top_frame->pop_operand_stack();
    assert(padding.tipo == PADDING);
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int >= (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(value.tipo == array->array_content_type());
    array->change_value(index.dados.valor_int, value);
    top_frame->pc++;
}

void Executor::fastore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == FLOAT);
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int >= (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(value.tipo == array->array_content_type());
    array->change_value(index.dados.valor_int, value);
    top_frame->pc++;
}

void Executor::dastore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == DOUBLE);
    Valor padding = top_frame->pop_operand_stack();
    assert(padding.tipo == PADDING);
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int >= (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(value.tipo == array->array_content_type());
    array->change_value(index.dados.valor_int, value);
    top_frame->pc++;
}

void Executor::aastore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == REFERENCIA);
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int >= (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    array->change_value(index.dados.valor_int, value);
    top_frame->pc++;
}

void Executor::bastore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    assert(array->array_content_type() == BOOLEANO || array->array_content_type() == BYTE);
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    if (array->array_content_type() == BOOLEANO) {
        value = faz_valor_booleano((value.dados.valor_int != 0) ? true : false);
    } else {
        value = faz_valor_byte((int8_t)value.dados.valor_int);
    }
    array->change_value(index.dados.valor_int, value);
    top_frame->pc++;
}

void Executor::castore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    value = faz_valor_char((uint8_t)value.dados.valor_int);
    array->change_value(index.dados.valor_int, value);
    top_frame->pc++;
}

void Executor::sastore()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Arranjo* array;
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    Valor index = top_frame->pop_operand_stack();
    assert(index.tipo == INT);
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    assert((arrayref.dados.objeto)->tipo_objeto() == ARRANJO);
    array = (Arranjo*)arrayref.dados.objeto;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.dados.valor_int > (signed)array->get_size() || index.dados.valor_int < 0) {
        cerr << "ArranjoIndexOutOfBoundsException" << endl;
        exit(2);
    }
    value = faz_valor_short((int16_t)value.dados.valor_int);
    array->change_value(index.dados.valor_int, value);
    top_frame->pc++;
}

void Executor::pop()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo != LONG);
    assert(value.tipo != DOUBLE);
    top_frame->pc++;
}

void Executor::pop2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    top_frame->pc++;
}

void Executor::dup()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo != LONG);
    assert(value.tipo != DOUBLE);
    top_frame->push_operand_stack(value);
    top_frame->push_operand_stack(value);
    top_frame->pc++;
}

void Executor::dup_x1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo != LONG);
    assert(value_1.tipo != DOUBLE);
    Valor value_2 = top_frame->pop_operand_stack();
    assert(value_2.tipo != LONG);
    assert(value_2.tipo != DOUBLE);
    top_frame->push_operand_stack(value_1);
    top_frame->push_operand_stack(value_2);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::dup_x2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_3 = top_frame->pop_operand_stack();
    assert(value_1.tipo != LONG);
    assert(value_1.tipo != DOUBLE);
    assert(value_3.tipo != LONG);
    assert(value_3.tipo != DOUBLE);
    top_frame->push_operand_stack(value_1);
    top_frame->push_operand_stack(value_3);
    top_frame->push_operand_stack(value_2);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::dup2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    Valor value_2 = top_frame->pop_operand_stack();
    assert(value_2.tipo != LONG);
    assert(value_2.tipo != DOUBLE);
    top_frame->push_operand_stack(value_2);
    top_frame->push_operand_stack(value_1);
    top_frame->push_operand_stack(value_2);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::dup2_x1()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_3 = top_frame->pop_operand_stack();
    assert(value_2.tipo != LONG);
    assert(value_2.tipo != DOUBLE);
    assert(value_3.tipo != LONG);
    assert(value_3.tipo != DOUBLE);
    top_frame->push_operand_stack(value_2);
    top_frame->push_operand_stack(value_1);
    top_frame->push_operand_stack(value_3);
    top_frame->push_operand_stack(value_2);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::dup2_x2()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_3 = top_frame->pop_operand_stack();
    Valor value_4 = top_frame->pop_operand_stack();
    assert(value_2.tipo != LONG);
    assert(value_2.tipo != DOUBLE);
    assert(value_4.tipo != LONG);
    assert(value_4.tipo != DOUBLE);
    top_frame->push_operand_stack(value_2);
    top_frame->push_operand_stack(value_1);
    top_frame->push_operand_stack(value_4);
    top_frame->push_operand_stack(value_3);
    top_frame->push_operand_stack(value_2);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::swap()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    Valor value_2 = top_frame->pop_operand_stack();
    assert(value_1.tipo != LONG);
    assert(value_1.tipo != DOUBLE);
    assert(value_2.tipo != LONG);
    assert(value_2.tipo != DOUBLE);
    top_frame->push_operand_stack(value_1);
    top_frame->push_operand_stack(value_2);
    top_frame->pc++;
}

void Executor::iadd()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    value_1.dados.valor_int = value_1.dados.valor_int + (value_2.dados.valor_int);
    value_1.tipo_print = INT;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::ladd()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);
    value_1.dados.valor_long = value_1.dados.valor_long + (value_2.dados.valor_long);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::fadd()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == FLOAT);
    assert(value_1.tipo == FLOAT);
    value_1.dados.valor_float = value_1.dados.valor_float + (value_2.dados.valor_float);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::dadd()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == DOUBLE);
    assert(value_1.tipo == DOUBLE);
    value_1.dados.valor_double = value_1.dados.valor_double + (value_2.dados.valor_double);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::isub()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    value_1.tipo_print = INT;
    value_1.dados.valor_int = value_1.dados.valor_int - (value_2.dados.valor_int);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lsub()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);
    value_1.dados.valor_long = value_1.dados.valor_long - (value_2.dados.valor_long);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::fsub()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == FLOAT);
    assert(value_1.tipo == FLOAT);
    value_1.dados.valor_float = value_1.dados.valor_float - (value_2.dados.valor_float);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::dsub()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == DOUBLE);
    assert(value_1.tipo == DOUBLE);
    value_1.dados.valor_double = value_1.dados.valor_double - (value_2.dados.valor_double);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::imul()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    value_1.tipo_print = INT;
    value_1.dados.valor_int = value_1.dados.valor_int * (value_2.dados.valor_int);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lmul()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);
    value_1.dados.valor_long = value_1.dados.valor_long * (value_2.dados.valor_long);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::fmul()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == FLOAT);
    assert(value_1.tipo == FLOAT);
    value_1.dados.valor_float = value_1.dados.valor_float * (value_2.dados.valor_float);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::dmul()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == DOUBLE);
    assert(value_1.tipo == DOUBLE);
    value_1.dados.valor_double = value_1.dados.valor_double * (value_2.dados.valor_double);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::idiv()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    if (value_2.dados.valor_int == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.tipo_print = INT;
    value_1.dados.valor_int = value_1.dados.valor_int / (value_2.dados.valor_int);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::ldiv()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);
    if (value_2.dados.valor_long == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.dados.valor_long = value_1.dados.valor_long / (value_2.dados.valor_long);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::fdiv()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == FLOAT);
    assert(value_1.tipo == FLOAT);
    if (value_2.dados.valor_float == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.dados.valor_float = value_1.dados.valor_float / (value_2.dados.valor_float);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::ddiv()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == DOUBLE);
    assert(value_1.tipo == DOUBLE);
    if (value_2.dados.valor_double == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.dados.valor_double = value_1.dados.valor_double / (value_2.dados.valor_double);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::irem()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    if (value_2.dados.valor_int == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.tipo_print = INT;
    value_1.dados.valor_int = value_1.dados.valor_int - (value_1.dados.valor_int / value_2.dados.valor_int) * value_2.dados.valor_int;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lrem()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);
    if (value_2.dados.valor_long == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    value_1.dados.valor_long = value_1.dados.valor_long - (value_1.dados.valor_long / value_2.dados.valor_long) * value_2.dados.valor_long;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::frem()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == FLOAT);
    assert(value_1.tipo == FLOAT);
    if (value_2.dados.valor_float == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    value_1.dados.valor_float = value_1.dados.valor_float
        - ((uint32_t)(value_1.dados.valor_float / value_2.dados.valor_float)) * value_2.dados.valor_float;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::drem()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_2.tipo == DOUBLE);
    assert(value_1.tipo == DOUBLE);
    if (value_2.dados.valor_double == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    value_1.dados.valor_double = value_1.dados.valor_double
        - ((uint64_t)(value_1.dados.valor_double / value_2.dados.valor_double)) * value_2.dados.valor_double;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::ineg()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == INT);
    value_1.tipo_print = INT;
    value_1.dados.valor_int = -value_1.dados.valor_int;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lneg()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();

    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == LONG);
    value_1.dados.valor_long = -value_1.dados.valor_long;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::fneg()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == FLOAT);
    value_1.dados.valor_float = -value_1.dados.valor_float;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::dneg()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();

    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == DOUBLE);
    value_1.dados.valor_double = -value_1.dados.valor_double;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::ishl()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);

    value_2.dados.valor_int = 0x1f & value_2.dados.valor_int;
    value_1.dados.valor_int = value_1.dados.valor_int << value_2.dados.valor_int;
    value_1.tipo_print = INT;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lshl()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_2.tipo == INT);
    assert(value_1.tipo == LONG);
    value_2.dados.valor_long = 0x3f & value_2.dados.valor_long;
    value_1.dados.valor_long = (value_1.dados.valor_long) << value_2.dados.valor_int;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::ishr()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);

    value_2.dados.valor_int = 0x1f & value_2.dados.valor_int;
    value_1.dados.valor_int = value_1.dados.valor_int >> value_2.dados.valor_int;
    value_1.tipo_print = INT;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lshr()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_2.tipo == INT);
    assert(value_1.tipo == LONG);

    value_2.dados.valor_long = 0x3f & value_2.dados.valor_long;
    value_1.dados.valor_long = value_1.dados.valor_long >> value_2.dados.valor_long;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::iushr()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    value_2.dados.valor_int = 0x1f & value_2.dados.valor_int;
    value_1.dados.valor_int = value_1.dados.valor_int >> value_2.dados.valor_int;
    if (value_1.dados.valor_int < 0) {
        value_1.dados.valor_int = value_1.dados.valor_int + (2 << ~(value_2.dados.valor_int));
    }
    value_1.tipo_print = INT;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lushr()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == LONG);
    value_2.dados.valor_int = 0x3f & value_2.dados.valor_int;
    value_1.dados.valor_long = value_1.dados.valor_long >> value_2.dados.valor_int;
    if (value_1.dados.valor_long < 0) {
        value_1.dados.valor_long = value_1.dados.valor_long + ((int64_t)2 << ~(value_2.dados.valor_int));
    }
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::iand()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    value_1.tipo_print = INT;
    value_1.dados.valor_int = value_1.dados.valor_int & value_2.dados.valor_int;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::land()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);

    value_1.dados.valor_long = value_1.dados.valor_long & value_2.dados.valor_long;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::ior()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    value_1.tipo_print = INT;
    value_1.dados.valor_int = value_1.dados.valor_int | value_2.dados.valor_int;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lor()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);
    value_1.dados.valor_long = value_1.dados.valor_long | value_2.dados.valor_long;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::ixor()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_2.tipo == INT);
    assert(value_1.tipo == INT);
    value_1.tipo_print = INT;
    value_1.dados.valor_int = value_1.dados.valor_int ^ value_2.dados.valor_int;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lxor()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);
    value_1.dados.valor_long = value_1.dados.valor_long ^ value_2.dados.valor_long;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::iinc()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u2 index = 0;
    if (is_wide) {
        index = (code[1] << 8) | code[2];
    } else {
        index += code[1];
    }
    Valor localVariable = top_frame->get_local_variable_value(index);
    assert(localVariable.tipo == INT);
    int32_t inc;
    if (is_wide) {
        uint16_t constant = (code[3] << 8) | code[4];
        inc = (int32_t)(int16_t)constant;
    } else {
        inc = (int32_t)(int8_t)code[2];
    }
    localVariable.dados.valor_int += inc;
    top_frame->set_local_variable(localVariable, index);
    top_frame->pc += is_wide ? 5 : 3;
    is_wide = false;
}

void Executor::i2l()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == INT);
    Valor padding = faz_valor_padding();
    top_frame->push_operand_stack(padding);
    value_1 = faz_valor_long((int64_t)value_1.dados.valor_int);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::i2f()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == INT);
    value_1 = faz_valor_float((float)value_1.dados.valor_int);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::i2d()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == INT);
    Valor padding = faz_valor_padding();
    top_frame->push_operand_stack(padding);
    value_1 = faz_valor_double((double)value_1.dados.valor_int);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::l2i()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    assert(value_1.tipo == LONG);
    value_1 = faz_valor_int((int32_t)value_1.dados.valor_int);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::l2f()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    assert(value_1.tipo == LONG);
    value_1 = faz_valor_float((float)value_1.dados.valor_long);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::l2d()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_1.tipo == LONG);
    value_1 = faz_valor_double((double)value_1.dados.valor_long);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::f2i()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == FLOAT);
    value_1 = faz_valor_int((int32_t)value_1.dados.valor_float);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::f2l()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == FLOAT);
    Valor padding = faz_valor_padding();
    top_frame->push_operand_stack(padding);
    value_1 = faz_valor_long((int64_t)value_1.dados.valor_float);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::f2d()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == FLOAT);
    Valor padding = faz_valor_padding();
    top_frame->push_operand_stack(padding);
    value_1 = faz_valor_double((double)value_1.dados.valor_float);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::d2i()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    assert(value_1.tipo == DOUBLE);
    value_1 = faz_valor_int((int32_t)value_1.dados.valor_double);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::d2l()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();

    assert(value_1.tipo == DOUBLE);
    value_1 = faz_valor_long((int64_t)value_1.dados.valor_double);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::d2f()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    assert(value_1.tipo == DOUBLE);
    value_1 = faz_valor_float((float)value_1.dados.valor_double);
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::i2b()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == INT);
    value_1 = faz_valor_int((int32_t)(int8_t)value_1.dados.valor_int);
    value_1.tipo_print = BYTE;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::i2c()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == INT);
    value_1 = faz_valor_char((uint8_t)value_1.dados.valor_int);
    value_1.tipo = INT;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::i2s()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_1 = top_frame->pop_operand_stack();
    assert(value_1.tipo == INT);
    value_1 = faz_valor_short((int16_t)value_1.dados.valor_int);
    value_1.tipo = INT;
    top_frame->push_operand_stack(value_1);
    top_frame->pc++;
}

void Executor::lcmp()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor result = faz_valor_int(0);
    assert(value_2.tipo == LONG);
    assert(value_1.tipo == LONG);
    if (value_1.dados.valor_long > value_2.dados.valor_long) {
        result.dados.valor_int = 1;
    } else if (value_1.dados.valor_long == value_2.dados.valor_long) {
        result.dados.valor_int = 0;
    } else {
        result.dados.valor_int = -1;
    }
    top_frame->push_operand_stack(result);
    top_frame->pc++;
}

void Executor::fcmpl()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    Valor result = faz_valor_int(0);
    assert(value_2.tipo == FLOAT);
    assert(value_1.tipo == FLOAT);
    if (isnan(value_1.dados.valor_float) || isnan(value_2.dados.valor_float)) {
        result.dados.valor_int = -1;
    } else if (value_1.dados.valor_float > value_2.dados.valor_float) {
        result.dados.valor_int = 1;
    } else if (value_1.dados.valor_float == value_2.dados.valor_float) {
        result.dados.valor_int = 0;
    } else {
        result.dados.valor_int = -1;
    }
    top_frame->push_operand_stack(result);
    top_frame->pc++;
}

void Executor::fcmpg()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    Valor result = faz_valor_int(0);
    assert(value_2.tipo == FLOAT);
    assert(value_1.tipo == FLOAT);
    if (isnan(value_1.dados.valor_float) || isnan(value_2.dados.valor_float)) {
        result.dados.valor_int = 1;
    } else if (value_1.dados.valor_float > value_2.dados.valor_float) {
        result.dados.valor_int = 1;
    } else if (value_1.dados.valor_float == value_2.dados.valor_float) {
        result.dados.valor_int = 0;
    } else {
        result.dados.valor_int = -1;
    }
    top_frame->push_operand_stack(result);
    top_frame->pc++;
}

void Executor::dcmpl()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor result = faz_valor_int(0);
    assert(value_2.tipo == DOUBLE);
    assert(value_1.tipo == DOUBLE);
    if (isnan(value_1.dados.valor_double) || isnan(value_2.dados.valor_double)) {
        result.dados.valor_int = -1;
    } else if (value_1.dados.valor_double > value_2.dados.valor_double) {
        result.dados.valor_int = 1;
    } else if (value_1.dados.valor_double == value_2.dados.valor_double) {
        result.dados.valor_int = 0;
    } else {
        result.dados.valor_int = -1;
    }
    top_frame->push_operand_stack(result);
    top_frame->pc++;
}

void Executor::dcmpg()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value_2 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor value_1 = top_frame->pop_operand_stack();
    top_frame->pop_operand_stack();
    Valor result = faz_valor_int(0);
    assert(value_2.tipo == DOUBLE);
    assert(value_1.tipo == DOUBLE);
    if (isnan(value_1.dados.valor_double) || isnan(value_2.dados.valor_double)) {
        result.dados.valor_int = 1;
    } else if (value_1.dados.valor_double > value_2.dados.valor_double) {
        result.dados.valor_int = 1;
    } else if (value_1.dados.valor_double == value_2.dados.valor_double) {
        result.dados.valor_int = 0;
    } else {
        result.dados.valor_int = -1;
    }
    top_frame->push_operand_stack(result);
    top_frame->pc++;
}

void Executor::ifeq()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    if (value.dados.valor_int == 0) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::ifne()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    if (value.dados.valor_int != 0) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::iflt()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    if (value.dados.valor_int < 0) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::ifge()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    if (value.dados.valor_int >= 0) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::ifgt()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    if (value.dados.valor_int > 0) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::ifle()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value = top_frame->pop_operand_stack();
    assert(value.tipo == INT);
    if (value.dados.valor_int <= 0) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::if_icmpeq()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value2 = top_frame->pop_operand_stack();
    Valor value1 = top_frame->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int == value2.dados.valor_int) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::if_icmpne()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value2 = top_frame->pop_operand_stack();
    Valor value1 = top_frame->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int != value2.dados.valor_int) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::if_icmplt()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value2 = top_frame->pop_operand_stack();
    Valor value1 = top_frame->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int < value2.dados.valor_int) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::if_icmpge()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value2 = top_frame->pop_operand_stack();
    Valor value1 = top_frame->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int >= value2.dados.valor_int) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::if_icmpgt()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value2 = top_frame->pop_operand_stack();
    Valor value1 = top_frame->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int > value2.dados.valor_int) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::if_icmple()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value2 = top_frame->pop_operand_stack();
    Valor value1 = top_frame->pop_operand_stack();
    assert(value1.tipo == INT);
    assert(value2.tipo == INT);
    if (value1.dados.valor_int <= value2.dados.valor_int) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::if_acmpeq()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value2 = top_frame->pop_operand_stack();
    Valor value1 = top_frame->pop_operand_stack();
    assert(value1.tipo == REFERENCIA);
    assert(value2.tipo == REFERENCIA);
    if (value1.dados.objeto == value2.dados.objeto) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::if_acmpne()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor value2 = top_frame->pop_operand_stack();
    Valor value1 = top_frame->pop_operand_stack();
    assert(value1.tipo == REFERENCIA);
    assert(value2.tipo == REFERENCIA);
    if (value1.dados.objeto != value2.dados.objeto) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branchOffset = (byte1 << 8) | byte2;
        top_frame->pc += branchOffset;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::func_goto()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    int16_t branchOffset = (byte1 << 8) | byte2;
    top_frame->pc += branchOffset;
}

void Executor::jsr()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    int16_t branchOffset = (byte1 << 8) | byte2;
    Valor returnAddr = faz_valor_endereco_retorno(top_frame->pc + 3);
    top_frame->push_operand_stack(returnAddr);
    top_frame->pc += branchOffset;
}

void Executor::ret()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    uint16_t index = (uint16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
    }
    assert(((int16_t)(top_frame->get_local_variables_vector_size()) > index));
    Valor value = top_frame->get_local_variable_value(index);
    assert(value.tipo == ENDERECO_RETORNO);
    top_frame->set_local_variable(value, index);
    top_frame->pc = value.dados.endereco_retorno;
    is_wide = false;
}

void Executor::tableswitch()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 padding = 4 - (top_frame->pc + 1) % 4;
    padding = (padding == 4) ? 0 : padding;
    u1 defaultbyte1 = code[padding + 1];
    u1 defaultbyte2 = code[padding + 2];
    u1 defaultbyte3 = code[padding + 3];
    u1 defaultbyte4 = code[padding + 4];
    int32_t defaultBytes = (defaultbyte1 << 24) | (defaultbyte2 << 16) | (defaultbyte3 << 8) | defaultbyte4;
    u1 lowbyte1 = code[padding + 5];
    u1 lowbyte2 = code[padding + 6];
    u1 lowbyte3 = code[padding + 7];
    u1 lowbyte4 = code[padding + 8];
    uint32_t lowbytes = (lowbyte1 << 24) | (lowbyte2 << 16) | (lowbyte3 << 8) | lowbyte4;
    u1 highbyte1 = code[padding + 9];
    u1 highbyte2 = code[padding + 10];
    u1 highbyte3 = code[padding + 11];
    u1 highbyte4 = code[padding + 12];
    uint32_t highbytes = (highbyte1 << 24) | (highbyte2 << 16) | (highbyte3 << 8) | highbyte4;
    Valor keyValue = top_frame->pop_operand_stack();
    assert(keyValue.tipo == INT);
    int32_t key = keyValue.dados.valor_int;
    uint32_t i;
    uint32_t baseIndex = padding + 13;
    int32_t offsets = highbytes - lowbytes + 1;
    bool matched = false;
    for (i = 0; i < (unsigned)offsets; i++) {
        if ((unsigned)key == (unsigned)lowbytes) {
            int32_t offset = (code[baseIndex] << 24) | (code[baseIndex + 1] << 16) | (code[baseIndex + 2] << 8) | code[baseIndex + 3];
            top_frame->pc += offset;
            matched = true;
            break;
        }
        lowbytes++;
        baseIndex += 4;
    }
    if (!matched) {
        top_frame->pc += defaultBytes;
    }
}

void Executor::lookupswitch()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 padding = 4 - (top_frame->pc + 1) % 4;
    padding = (padding == 4) ? 0 : padding;
    u1 defaultbyte1 = code[padding + 1];
    u1 defaultbyte2 = code[padding + 2];
    u1 defaultbyte3 = code[padding + 3];
    u1 defaultbyte4 = code[padding + 4];
    int32_t defaultBytes = (defaultbyte1 << 24) | (defaultbyte2 << 16) | (defaultbyte3 << 8) | defaultbyte4;
    u1 npairs1 = code[padding + 5];
    u1 npairs2 = code[padding + 6];
    u1 npairs3 = code[padding + 7];
    u1 npairs4 = code[padding + 8];
    uint32_t npairs = (npairs1 << 24) | (npairs2 << 16) | (npairs3 << 8) | npairs4;
    Valor keyValue = top_frame->pop_operand_stack();
    assert(keyValue.tipo == INT);
    int32_t key = keyValue.dados.valor_int;
    uint32_t i;
    uint32_t baseIndex = padding + 9;
    bool matched = false;
    for (i = 0; i < npairs; i++) {
        int32_t match = (code[baseIndex] << 24) | (code[baseIndex + 1] << 16) | (code[baseIndex + 2] << 8) | code[baseIndex + 3];
        if (key == match) {
            int32_t offset = (code[baseIndex + 4] << 24) | (code[baseIndex + 5] << 16) | (code[baseIndex + 6] << 8) | code[baseIndex + 7];
            top_frame->pc += offset;
            matched = true;
            break;
        }
        baseIndex += 8;
    }
    if (!matched) {
        top_frame->pc += defaultBytes;
    }
}

void Executor::ireturn()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor returnValue = top_frame->pop_operand_stack();
    assert(returnValue.tipo == INT);
    stack_frame.desempilhar_frame();
    Frame* newTopFrame = stack_frame.frame_topo();
    newTopFrame->push_operand_stack(returnValue);
}

void Executor::lreturn()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor returnValue = top_frame->pop_operand_stack();
    assert(returnValue.tipo == LONG);
    assert(top_frame->pop_operand_stack().tipo == PADDING);
    stack_frame.desempilhar_frame();
    Frame* newTopFrame = stack_frame.frame_topo();
    Valor padding = faz_valor_padding();
    newTopFrame->push_operand_stack(padding);
    newTopFrame->push_operand_stack(returnValue);
}

void Executor::freturn()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor returnValue = top_frame->pop_operand_stack();
    assert(returnValue.tipo == FLOAT);
    stack_frame.desempilhar_frame();
    Frame* newTopFrame = stack_frame.frame_topo();
    newTopFrame->push_operand_stack(returnValue);
}

void Executor::dreturn()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor returnValue = top_frame->pop_operand_stack();
    assert(returnValue.tipo == DOUBLE);
    assert(top_frame->pop_operand_stack().tipo == PADDING);
    stack_frame.desempilhar_frame();
    Frame* newTopFrame = stack_frame.frame_topo();
    Valor padding = faz_valor_padding();
    newTopFrame->push_operand_stack(padding);
    newTopFrame->push_operand_stack(returnValue);
}

void Executor::areturn()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor returnValue = top_frame->pop_operand_stack();
    assert(returnValue.tipo == REFERENCIA);
    stack_frame.desempilhar_frame();
    Frame* newTopFrame = stack_frame.frame_topo();
    newTopFrame->push_operand_stack(returnValue);
}

void Executor::func_return()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    stack_frame.desempilhar_frame();
}

void Executor::getstatic()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t fieldIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo fieldCP = constant_pool[fieldIndex - 1];
    assert(fieldCP.tag == ConstFieldRef);
    ConstFieldRefInfo fieldRef = fieldCP.info.field_ref_info;
    string className = formatar_constante(constant_pool, fieldRef.class_index);
    ConstantPoolInfo nameAndTypeCP = constant_pool[fieldRef.name_and_type_index - 1];
    assert(nameAndTypeCP.tag == ConstNameType);
    ConstNameTypeInfo fieldNameAndType = nameAndTypeCP.info.name_type_info;
    string fieldName = formatar_constante(constant_pool, fieldNameAndType.name_index);
    string fieldDescriptor = formatar_constante(constant_pool, fieldNameAndType.descriptor_index);

    if (className == "java/lang/System" && fieldDescriptor == "Ljava/io/PrintStream;") {
        top_frame->pc += 3;
        return;
    }

    AreaMetodos& methodArea = AreaMetodos::instancia();
    ClasseEstatica* class_runtime = methodArea.carregar_classe(className);
    while (class_runtime != NULL) {
        if (class_runtime->check_field(fieldName) == false) {
            if (class_runtime->get_arquivo_classe()->super_class == 0) {
                class_runtime = NULL;
            } else {
                string superClassName = formatar_constante(class_runtime->get_arquivo_classe()->constant_pool,
                    class_runtime->get_arquivo_classe()->super_class);
                class_runtime = methodArea.carregar_classe(superClassName);
            }
        } else {
            break;
        }
    }
    if (class_runtime == NULL) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }

    if (stack_frame.frame_topo() != top_frame)
        return;
    Valor staticValue = class_runtime->get_value(fieldName);
    switch (staticValue.tipo) {
    case BOOLEANO:
        staticValue.tipo = INT;
        staticValue.tipo_print = BOOLEANO;
        break;
    case BYTE:
        staticValue.tipo = INT;
        staticValue.tipo_print = BYTE;
        break;
    case SHORT:
        staticValue.tipo = INT;
        staticValue.tipo_print = SHORT;
        break;
    case INT:
        staticValue.tipo = INT;
        staticValue.tipo_print = INT;
        break;
    default:
        break;
    }
    if (staticValue.tipo == DOUBLE || staticValue.tipo == LONG) {
        Valor paddingValue = faz_valor_padding();
        top_frame->push_operand_stack(paddingValue);
    }
    top_frame->push_operand_stack(staticValue);
    top_frame->pc += 3;
}

void Executor::putstatic()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t fieldIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo fieldCP = constant_pool[fieldIndex - 1];
    assert(fieldCP.tag == ConstFieldRef);
    ConstFieldRefInfo fieldRef = fieldCP.info.field_ref_info;
    string className = formatar_constante(constant_pool, fieldRef.class_index);
    ConstantPoolInfo nameAndTypeCP = constant_pool[fieldRef.name_and_type_index - 1];
    assert(nameAndTypeCP.tag == ConstNameType);
    ConstNameTypeInfo fieldNameAndType = nameAndTypeCP.info.name_type_info;
    string fieldName = formatar_constante(constant_pool, fieldNameAndType.name_index);
    string fieldDescriptor = formatar_constante(constant_pool, fieldNameAndType.descriptor_index);
    AreaMetodos& methodArea = AreaMetodos::instancia();
    ClasseEstatica* class_runtime = methodArea.carregar_classe(className);
    while (class_runtime != NULL) {
        if (class_runtime->check_field(fieldName) == false) {
            if (class_runtime->get_arquivo_classe()->super_class == 0) {
                class_runtime = NULL;
            } else {
                string superClassName = formatar_constante(class_runtime->get_arquivo_classe()->constant_pool,
                    class_runtime->get_arquivo_classe()->super_class);
                class_runtime = methodArea.carregar_classe(superClassName);
            }
        } else {
            break;
        }
    }
    if (class_runtime == NULL) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }

    if (stack_frame.frame_topo() != top_frame)
        return;
    Valor topValue = top_frame->pop_operand_stack();
    if (topValue.tipo == DOUBLE || topValue.tipo == LONG) {
        top_frame->pop_operand_stack();
    } else {
        switch (fieldDescriptor[0]) {
        case 'B':
            topValue.tipo = BYTE;
            topValue.tipo_print = BYTE;
            break;
        case 'C':
            topValue.tipo = CHAR;
            topValue.tipo = CHAR;
            break;
        case 'S':
            topValue.tipo = SHORT;
            topValue.tipo = SHORT;
            break;
        case 'Z':
            topValue.tipo = BOOLEANO;
            topValue.tipo = BOOLEANO;
            break;
        }
    }
    class_runtime->insert_value(topValue, fieldName);
    top_frame->pc += 3;
}

void Executor::getfield()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t fieldIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo fieldCP = constant_pool[fieldIndex - 1];
    assert(fieldCP.tag == ConstFieldRef);
    ConstFieldRefInfo fieldRef = fieldCP.info.field_ref_info;
    string className = formatar_constante(constant_pool, fieldRef.class_index);
    ConstantPoolInfo nameAndTypeCP = constant_pool[fieldRef.name_and_type_index - 1];
    assert(nameAndTypeCP.tag == ConstNameType);
    ConstNameTypeInfo fieldNameAndType = nameAndTypeCP.info.name_type_info;
    string fieldName = formatar_constante(constant_pool, fieldNameAndType.name_index);
    string fieldDescriptor = formatar_constante(constant_pool, fieldNameAndType.descriptor_index);
    Valor objectValue = top_frame->pop_operand_stack();
    assert(objectValue.tipo == REFERENCIA);
    Objeto* object = objectValue.dados.objeto;
    assert(object->tipo_objeto() == INSTANCIA_CLASSE);
    ClasseInstancia* classInstance = (ClasseInstancia*)object;
    if (!classInstance->field_exists(fieldName)) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }
    Valor fieldValue = classInstance->get_value_from_field(fieldName);
    switch (fieldValue.tipo) {
    case BOOLEANO:
        fieldValue.tipo = INT;
        fieldValue.tipo_print = BOOLEANO;
        break;
    case BYTE:
        fieldValue.tipo = INT;
        fieldValue.tipo_print = BYTE;
        break;
    case SHORT:
        fieldValue.tipo = INT;
        fieldValue.tipo_print = SHORT;
        break;
    case INT:
        fieldValue.tipo = INT;
        fieldValue.tipo_print = INT;
        break;
    default:
        break;
    }
    if (fieldValue.tipo == DOUBLE || fieldValue.tipo == LONG) {
        Valor paddingValue = faz_valor_padding();
        top_frame->push_operand_stack(paddingValue);
    }
    top_frame->push_operand_stack(fieldValue);
    top_frame->pc += 3;
}

void Executor::putfield()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t fieldIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo fieldCP = constant_pool[fieldIndex - 1];
    assert(fieldCP.tag == ConstFieldRef);
    ConstFieldRefInfo fieldRef = fieldCP.info.field_ref_info;
    string className = formatar_constante(constant_pool, fieldRef.class_index);
    ConstantPoolInfo nameAndTypeCP = constant_pool[fieldRef.name_and_type_index - 1];
    assert(nameAndTypeCP.tag == ConstNameType);
    ConstNameTypeInfo fieldNameAndType = nameAndTypeCP.info.name_type_info;
    string fieldName = formatar_constante(constant_pool, fieldNameAndType.name_index);
    string fieldDescriptor = formatar_constante(constant_pool, fieldNameAndType.descriptor_index);
    Valor valueToBeInserted = top_frame->pop_operand_stack();
    if (valueToBeInserted.tipo == DOUBLE || valueToBeInserted.tipo == LONG) {
        top_frame->pop_operand_stack();
    } else {
        switch (fieldDescriptor[0]) {
        case 'B':
            valueToBeInserted.tipo = BYTE;
            valueToBeInserted.tipo_print = BYTE;
            break;
        case 'C':
            valueToBeInserted.tipo = CHAR;
            valueToBeInserted.tipo_print = CHAR;
            break;
        case 'S':
            valueToBeInserted.tipo = SHORT;
            valueToBeInserted.tipo_print = SHORT;
            break;
        case 'Z':
            valueToBeInserted.tipo = BOOLEANO;
            valueToBeInserted.tipo_print = BOOLEANO;
            break;
        }
    }
    Valor objectValue = top_frame->pop_operand_stack();
    assert(objectValue.tipo == REFERENCIA);
    Objeto* object = objectValue.dados.objeto;
    assert(object->tipo_objeto() == INSTANCIA_CLASSE);
    ClasseInstancia* classInstance = (ClasseInstancia*)object;
    classInstance->insert_value_into_field(valueToBeInserted, fieldName);
    top_frame->pc += 3;
}

void Executor::invokevirtual()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    stack<Valor> operandStackBackup = top_frame->copy_operand_stack();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t methodIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo methodCP = constant_pool[methodIndex - 1];
    assert(methodCP.tag == ConstMethodRef);
    ConstMethodRefInfo methodInfo = methodCP.info.method_ref_info;
    string className = formatar_constante(constant_pool, methodInfo.class_index);
    ConstantPoolInfo nameAndTypeCP = constant_pool[methodInfo.name_and_type_index - 1];
    assert(nameAndTypeCP.tag == ConstNameType);
    ConstNameTypeInfo methodNameAndType = nameAndTypeCP.info.name_type_info;
    string method_name = formatar_constante(constant_pool, methodNameAndType.name_index);
    string methodDescriptor = formatar_constante(constant_pool, methodNameAndType.descriptor_index);
    if (className.find("java/") != string::npos) {

        if (className == "java/io/PrintStream" && (method_name == "print" || method_name == "println")) {
            if (methodDescriptor != "()V") {
                Valor printValue = top_frame->pop_operand_stack();
                if (printValue.tipo == INT) {
                    switch (printValue.tipo_print) {
                    case BOOLEANO:
                        cout << (printValue.dados.valor_booleano == 0 ? "false" : "true");
                        break;
                    case BYTE:
                        cout << (int)printValue.dados.valor_byte;
                        break;
                    case CHAR:
                        cout << printValue.dados.valor_char;
                        break;
                    case SHORT:
                        cout << printValue.dados.valor_short;
                        break;
                    default:
                        cout << printValue.dados.valor_int;
                        break;
                    }
                } else {
                    switch (printValue.tipo) {
                    case DOUBLE:
                        top_frame->pop_operand_stack();
                        cout << printValue.dados.valor_double;
                        break;
                    case FLOAT:
                        cout << printValue.dados.valor_float;
                        break;
                    case LONG:
                        top_frame->pop_operand_stack();
                        cout << printValue.dados.valor_long;
                        break;
                    case REFERENCIA:
                        assert(printValue.dados.objeto->tipo_objeto() == INSTANCIA_STRING);
                        cout << ((ObjetoString*)printValue.dados.objeto)->get_str().c_str();
                        break;
                    case BOOLEANO:
                        cout << (printValue.dados.valor_booleano == 0 ? "false" : "true");
                        break;
                    case BYTE:
                        cout << (int)printValue.dados.valor_byte;
                        break;
                    case CHAR:
                        cout << printValue.dados.valor_char;
                        break;
                    case SHORT:
                        cout << printValue.dados.valor_short;
                        break;
                    default:
                        cerr << "Invalid print type:" << printValue.tipo << endl;
                        exit(1);
                        break;
                    }
                }
            }
            if (method_name == "println")
                cout << "\n";
        } else if (className == "java/lang/String" && method_name == "equals") {
            Valor strValue1 = top_frame->pop_operand_stack();
            Valor strValue2 = top_frame->pop_operand_stack();
            assert(strValue1.tipo == REFERENCIA);
            assert(strValue2.tipo == REFERENCIA);
            assert(strValue1.dados.objeto->tipo_objeto() == INSTANCIA_STRING);
            assert(strValue2.dados.objeto->tipo_objeto() == INSTANCIA_STRING);
            ObjetoString* str1 = (ObjetoString*)strValue1.dados.objeto;
            ObjetoString* str2 = (ObjetoString*)strValue2.dados.objeto;
            Valor result = faz_valor_int(0);
            if (str1->get_str() == str2->get_str()) {
                result.dados.valor_int = 1;
            } else {
                result.dados.valor_int = 0;
            }
            top_frame->push_operand_stack(result);
        } else if (className == "java/lang/String" && method_name == "length") {
            Valor strValue = top_frame->pop_operand_stack();
            assert(strValue.tipo == REFERENCIA);
            assert(strValue.dados.objeto->tipo_objeto() == INSTANCIA_STRING);
            ObjetoString* str = (ObjetoString*)strValue.dados.objeto;
            Valor result = faz_valor_int((int32_t)(str->get_str()).size());
            top_frame->push_operand_stack(result);
        } else {
            cerr << "Call to invalid instance method: " << method_name << endl;
            exit(1);
        }
    } else {
        uint16_t nargs = 0;
        uint16_t i = 1;
        while (methodDescriptor[i] != ')') {
            char baseType = methodDescriptor[i];
            if (baseType == 'D' || baseType == 'J') {
                nargs += 2;
            } else if (baseType == 'L') {
                nargs++;
                while (methodDescriptor[++i] != ';')
                    ;
            } else if (baseType == '[') {
                nargs++;
                while (methodDescriptor[++i] == '[')
                    ;
                if (methodDescriptor[i] == 'L')
                    while (methodDescriptor[++i] != ';')
                        ;
            } else {
                nargs++;
            }
            i++;
        }
        vector<Valor> args;
        for (int i = 0; i < nargs; i++) {
            Valor value = top_frame->pop_operand_stack();
            if (value.tipo == PADDING) {
                args.insert(args.begin() + 1, value);
            } else {
                args.insert(args.begin(), value);
            }
        }
        Valor objectValue = top_frame->pop_operand_stack();
        assert(objectValue.tipo == REFERENCIA);
        args.insert(args.begin(), objectValue);
        Objeto* object = objectValue.dados.objeto;
        assert(object->tipo_objeto() == INSTANCIA_CLASSE);
        ClasseInstancia* instance = (ClasseInstancia*)object;
        AreaMetodos& methodArea = AreaMetodos::instancia();
        ClasseEstatica* class_runtime = methodArea.carregar_classe(className);
        Frame* newFrame = new Frame(instance, class_runtime, method_name, methodDescriptor, args);

        if (stack_frame.frame_topo() != top_frame) {
            top_frame->load_operand_stack(operandStackBackup);
            delete newFrame;
            return;
        }
        stack_frame.empilhar_frame(newFrame);
    }
    top_frame->pc += 3;
}

void Executor::invokespecial()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    stack<Valor> operandStackBackup = top_frame->copy_operand_stack();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t methodIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo methodCP = constant_pool[methodIndex - 1];
    assert(methodCP.tag == ConstMethodRef);
    ConstMethodRefInfo methodInfo = methodCP.info.method_ref_info;
    string className = formatar_constante(constant_pool, methodInfo.class_index);
    ConstantPoolInfo nameAndTypeCP = constant_pool[methodInfo.name_and_type_index - 1];
    assert(nameAndTypeCP.tag == ConstNameType);
    ConstNameTypeInfo methodNameAndType = nameAndTypeCP.info.name_type_info;
    string method_name = formatar_constante(constant_pool, methodNameAndType.name_index);
    string methodDescriptor = formatar_constante(constant_pool, methodNameAndType.descriptor_index);

    if ((className == "java/lang/Objeto" || className == "java/lang/String") && method_name == "<init>") {
        if (className == "java/lang/String") {
            top_frame->pop_operand_stack();
        }
        top_frame->pc += 3;
        return;
    }

    if (className.find("java/") != string::npos) {
        cerr << "Call to invalid special method: " << method_name << endl;
        exit(1);
    } else {
        uint16_t nargs = 0;
        uint16_t i = 1;
        while (methodDescriptor[i] != ')') {
            char baseType = methodDescriptor[i];
            if (baseType == 'D' || baseType == 'J') {
                nargs += 2;
            } else if (baseType == 'L') {
                nargs++;
                while (methodDescriptor[++i] != ';')
                    ;
            } else if (baseType == '[') {
                nargs++;
                while (methodDescriptor[++i] == '[')
                    ;
                if (methodDescriptor[i] == 'L')
                    while (methodDescriptor[++i] != ';')
                        ;
            } else {
                nargs++;
            }
            i++;
        }
        vector<Valor> args;
        for (int i = 0; i < nargs; i++) {
            Valor value = top_frame->pop_operand_stack();
            if (value.tipo == PADDING) {
                args.insert(args.begin() + 1, value);
            } else {
                args.insert(args.begin(), value);
            }
        }
        Valor objectValue = top_frame->pop_operand_stack();
        assert(objectValue.tipo == REFERENCIA);
        args.insert(args.begin(), objectValue);
        Objeto* object = objectValue.dados.objeto;
        assert(object->tipo_objeto() == INSTANCIA_CLASSE);
        ClasseInstancia* instance = (ClasseInstancia*)object;
        AreaMetodos& methodArea = AreaMetodos::instancia();
        ClasseEstatica* class_runtime = methodArea.carregar_classe(className);
        Frame* newFrame = new Frame(instance, class_runtime, method_name, methodDescriptor, args);

        if (stack_frame.frame_topo() != top_frame) {
            top_frame->load_operand_stack(operandStackBackup);
            delete newFrame;
            return;
        }
        stack_frame.empilhar_frame(newFrame);
    }
    top_frame->pc += 3;
}

void Executor::invokestatic()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    stack<Valor> operandStackBackup = top_frame->copy_operand_stack();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t methodIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo methodCP = constant_pool[methodIndex - 1];
    assert(methodCP.tag == ConstMethodRef);
    ConstMethodRefInfo methodInfo = methodCP.info.method_ref_info;
    string className = formatar_constante(constant_pool, methodInfo.class_index);
    ConstantPoolInfo nameAndTypeCP = constant_pool[methodInfo.name_and_type_index - 1];
    assert(nameAndTypeCP.tag == ConstNameType);
    ConstNameTypeInfo methodNameAndType = nameAndTypeCP.info.name_type_info;
    string method_name = formatar_constante(constant_pool, methodNameAndType.name_index);
    string methodDescriptor = formatar_constante(constant_pool, methodNameAndType.descriptor_index);
    if (className == "java/lang/Objeto" && method_name == "registerNatives") {
        top_frame->pc += 3;
        return;
    }
    if (className.find("java/") != string::npos) {
        cerr << "Call to invalid static method: " << method_name << endl;
        exit(1);
    } else {
        uint16_t nargs = 0;
        uint16_t i = 1;
        while (methodDescriptor[i] != ')') {
            char baseType = methodDescriptor[i];
            if (baseType == 'D' || baseType == 'J') {
                nargs += 2;
            } else if (baseType == 'L') {
                nargs++;
                while (methodDescriptor[++i] != ';')
                    ;
            } else if (baseType == '[') {
                nargs++;
                while (methodDescriptor[++i] == '[')
                    ;
                if (methodDescriptor[i] == 'L')
                    while (methodDescriptor[++i] != ';')
                        ;
            } else {
                nargs++;
            }
            i++;
        }
        vector<Valor> args;
        for (int i = 0; i < nargs; i++) {
            Valor value = top_frame->pop_operand_stack();
            if (value.tipo == PADDING) {
                args.insert(args.begin() + 1, value);
            } else {
                args.insert(args.begin(), value);
            }
        }
        AreaMetodos& methodArea = AreaMetodos::instancia();
        ClasseEstatica* class_runtime = methodArea.carregar_classe(className);
        Frame* newFrame = new Frame(class_runtime, method_name, methodDescriptor, args);

        if (stack_frame.frame_topo() != top_frame) {
            top_frame->load_operand_stack(operandStackBackup);
            delete newFrame;
            return;
        }
        stack_frame.empilhar_frame(newFrame);
    }
    top_frame->pc += 3;
}

void Executor::invokeinterface()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    stack<Valor> operandStackBackup = top_frame->copy_operand_stack();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t methodIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo methodCP = constant_pool[methodIndex - 1];
    assert(methodCP.tag == ConstMethodRef || methodCP.tag == ConstInterfaceMethodRef);
    ConstMethodRefInfo methodInfo = methodCP.info.method_ref_info;
    string className = formatar_constante(constant_pool, methodInfo.class_index);
    ConstantPoolInfo nameAndTypeCP = constant_pool[methodInfo.name_and_type_index - 1];
    assert(nameAndTypeCP.tag == ConstNameType);
    ConstNameTypeInfo methodNameAndType = nameAndTypeCP.info.name_type_info;
    string method_name = formatar_constante(constant_pool, methodNameAndType.name_index);
    string methodDescriptor = formatar_constante(constant_pool, methodNameAndType.descriptor_index);
    if (className.find("java/") != string::npos) {
        cerr << "Call to invalid interface method: " << method_name << endl;
        exit(1);
    } else {
        uint16_t nargs = 0;
        uint16_t i = 1;
        while (methodDescriptor[i] != ')') {
            char baseType = methodDescriptor[i];
            if (baseType == 'D' || baseType == 'J') {
                nargs += 2;
            } else if (baseType == 'L') {
                nargs++;
                while (methodDescriptor[++i] != ';')
                    ;
            } else if (baseType == '[') {
                nargs++;
                while (methodDescriptor[++i] == '[')
                    ;
                if (methodDescriptor[i] == 'L')
                    while (methodDescriptor[++i] != ';')
                        ;
            } else {
                nargs++;
            }
            i++;
        }
        vector<Valor> args;
        for (int i = 0; i < nargs; i++) {
            Valor value = top_frame->pop_operand_stack();
            if (value.tipo == PADDING) {
                args.insert(args.begin() + 1, value);
            } else {
                args.insert(args.begin(), value);
            }
        }
        Valor objectValue = top_frame->pop_operand_stack();
        assert(objectValue.tipo == REFERENCIA);
        args.insert(args.begin(), objectValue);
        Objeto* object = objectValue.dados.objeto;
        assert(object->tipo_objeto() == INSTANCIA_CLASSE);
        ClasseInstancia* instance = (ClasseInstancia*)object;
        AreaMetodos& methodArea = AreaMetodos::instancia();
        methodArea.carregar_classe(className);
        Frame* newFrame = new Frame(instance, instance->get_classe_runtime(), method_name, methodDescriptor, args);

        if (stack_frame.frame_topo() != top_frame) {
            top_frame->load_operand_stack(operandStackBackup);
            delete newFrame;
            return;
        }
        stack_frame.empilhar_frame(newFrame);
    }
    top_frame->pc += 5;
}

void Executor::func_new()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t classIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo classCP = constant_pool[classIndex - 1];
    assert(classCP.tag == ConstClass);
    ConstClassInfo classInfo = classCP.info.class_info;
    string className = formatar_constante(constant_pool, classInfo.name_index);
    Objeto* object;
    if (className == "java/lang/String") {
        object = new ObjetoString();
    } else {
        AreaMetodos& methodArea = AreaMetodos::instancia();
        ClasseEstatica* class_runtime = methodArea.carregar_classe(className);
        object = new ClasseInstancia(class_runtime);
    }

    Valor objectref = faz_valor_referencia(object);
    top_frame->push_operand_stack(objectref);
    top_frame->pc += 3;
}

void Executor::newarray()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor count = top_frame->pop_operand_stack();
    assert(count.tipo == INT);
    if (count.dados.valor_int < 0) {
        cerr << "NegativeArranjoSizeException" << endl;
        exit(1);
    }
    Arranjo* array = nullptr;
    Valor value;
    value.dados.valor_long = 0;
    u1* code = top_frame->get_code(top_frame->pc);
    switch (code[1]) {
    case 4:
        array = new Arranjo(BOOLEANO);
        value = faz_valor_booleano(false);
        for (int i = 0; i < count.dados.valor_int; i++) {
            array->push_value(value);
        }
        break;
    case 5:
        array = new Arranjo(CHAR);
        value = faz_valor_char(0);
        for (int i = 0; i < count.dados.valor_int; i++) {
            array->push_value(value);
        }
        break;
    case 6:
        array = new Arranjo(FLOAT);
        value = faz_valor_float(0);
        for (int i = 0; i < count.dados.valor_int; i++) {
            array->push_value(value);
        }
        break;
    case 7:
        array = new Arranjo(DOUBLE);
        value = faz_valor_double(0);
        for (int i = 0; i < count.dados.valor_int; i++) {
            array->push_value(value);
        }
        break;
    case 8:
        array = new Arranjo(BYTE);
        value = faz_valor_byte(0);
        for (int i = 0; i < count.dados.valor_int; i++) {
            array->push_value(value);
        }
        break;
    case 9:
        array = new Arranjo(SHORT);
        value = faz_valor_short(0);
        for (int i = 0; i < count.dados.valor_int; i++) {
            array->push_value(value);
        }
        break;
    case 10:
        array = new Arranjo(INT);
        value = faz_valor_int(0);
        for (int i = 0; i < count.dados.valor_int; i++) {
            array->push_value(value);
        }
        break;
    case 11:
        array = new Arranjo(LONG);
        value = faz_valor_long(0);
        for (int i = 0; i < count.dados.valor_int; i++) {
            array->push_value(value);
        }
        break;
    }
    Valor arrayref = faz_valor_referencia(array);
    top_frame->push_operand_stack(arrayref);
    top_frame->pc += 2;
}

void Executor::anewarray()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor count = top_frame->pop_operand_stack();
    assert(count.tipo == INT);
    if (count.dados.valor_int < 0) {
        cerr << "NegativeArranjoSizeException" << endl;
        exit(1);
    }
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t classIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo classCP = constant_pool[classIndex - 1];
    assert(classCP.tag == ConstClass);
    ConstClassInfo classInfo = classCP.info.class_info;
    string className = formatar_constante(constant_pool, classInfo.name_index);
    if (className != "java/lang/String") {
        int i = 0;
        while (className[i] == '[')
            i++;
        if (className[i] == 'L') {
            AreaMetodos& methodArea = AreaMetodos::instancia();
            methodArea.carregar_classe(className.substr(i + 1, className.size() - i - 2));
        }
    }

    Valor objectref = faz_valor_referencia(new Arranjo(REFERENCIA));

    Valor nullValue = faz_valor_referencia(NULL);
    for (int i = 0; i < count.dados.valor_int; i++) {
        ((Arranjo*)objectref.dados.objeto)->push_value(nullValue);
    }
    top_frame->push_operand_stack(objectref);
    top_frame->pc += 3;
}

void Executor::arraylength()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor arrayref = top_frame->pop_operand_stack();
    assert(arrayref.tipo == REFERENCIA);
    if (arrayref.dados.objeto == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    Valor length = faz_valor_int((int)((Arranjo*)arrayref.dados.objeto)->get_size());
    top_frame->push_operand_stack(length);
    top_frame->pc++;
}

void Executor::athrow()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    top_frame->pc++;
}

void Executor::checkcast()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    AreaMetodos& methodArea = AreaMetodos::instancia();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u2 cpIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    ConstantPoolInfo cpElement = constant_pool[cpIndex - 1];
    assert(cpElement.tag == ConstClass);
    string className = formatar_constante(constant_pool, cpIndex);
    Valor objectrefValue = top_frame->pop_operand_stack();
    assert(objectrefValue.tipo == REFERENCIA);
    Valor resultValue = faz_valor_int(0);
    if (objectrefValue.dados.objeto == NULL) {
        cerr << "ClassCastException" << endl;
        exit(1);
    } else {
        Objeto* obj = objectrefValue.dados.objeto;
        if (obj->tipo_objeto() == INSTANCIA_CLASSE) {
            ClasseInstancia* classInstance = (ClasseInstancia*)obj;
            ClasseEstatica* class_runtime = classInstance->get_classe_runtime();
            bool found = false;
            while (!found) {
                ArquivoClasse* class_file = class_runtime->get_arquivo_classe();
                string currClassName = formatar_constante(class_file->constant_pool, class_file->this_class);
                if (currClassName == className) {
                    found = true;
                } else {
                    if (class_file->super_class == 0) {
                        break;
                    } else {
                        string superClassName = formatar_constante(class_file->constant_pool, class_file->this_class);
                        class_runtime = methodArea.carregar_classe(superClassName);
                    }
                }
            }
            resultValue.dados.valor_int = found ? 1 : 0;
        } else if (obj->tipo_objeto() == INSTANCIA_STRING) {
            resultValue.dados.valor_int = (className == "java/lang/String" || className == "java/lang/Objeto") ? 1 : 0;
        } else {
            if (className == "java/lang/Objeto") {
                resultValue.dados.valor_int = 1;
            } else {
                resultValue.dados.valor_int = 0;
            }
        }
    }
    top_frame->push_operand_stack(resultValue);
    top_frame->pc += 3;
}

void Executor::instanceof()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    AreaMetodos& methodArea = AreaMetodos::instancia();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u2 cpIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    ConstantPoolInfo cpElement = constant_pool[cpIndex - 1];
    assert(cpElement.tag == ConstClass);
    string className = formatar_constante(constant_pool, cpIndex);
    Valor objectrefValue = top_frame->pop_operand_stack();
    assert(objectrefValue.tipo == REFERENCIA);
    Valor resultValue = faz_valor_int(0);
    if (objectrefValue.dados.objeto == NULL) {
        resultValue.dados.valor_int = 0;
    } else {
        Objeto* obj = objectrefValue.dados.objeto;
        if (obj->tipo_objeto() == INSTANCIA_CLASSE) {
            ClasseInstancia* classInstance = (ClasseInstancia*)obj;
            ClasseEstatica* class_runtime = classInstance->get_classe_runtime();
            bool found = false;
            while (!found) {
                ArquivoClasse* class_file = class_runtime->get_arquivo_classe();
                string currClassName = formatar_constante(class_file->constant_pool, class_file->this_class);
                if (currClassName == className) {
                    found = true;
                } else {
                    if (class_file->super_class == 0) {
                        break;
                    } else {
                        string superClassName = formatar_constante(class_file->constant_pool, class_file->this_class);
                        class_runtime = methodArea.carregar_classe(superClassName);
                    }
                }
            }
            resultValue.dados.valor_int = found ? 1 : 0;
        } else if (obj->tipo_objeto() == INSTANCIA_STRING) {
            resultValue.dados.valor_int = (className == "java/lang/String" || className == "java/lang/Objeto") ? 1 : 0;
        } else {
            if (className == "java/lang/Objeto") {
                resultValue.dados.valor_int = 1;
            } else {
                resultValue.dados.valor_int = 0;
            }
        }
    }
    top_frame->push_operand_stack(resultValue);
    top_frame->pc += 3;
}

void Executor::monitorenter()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    top_frame->pc++;
}

void Executor::monitorexit()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    top_frame->pc++;
}

void Executor::wide()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    is_wide = true;
    top_frame->pc++;
}

void Executor::multianewarray()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    ConstantPoolInfo* constant_pool = *(top_frame->get_constant_pool());
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u1 dimensions = code[3];
    assert(dimensions >= 1);
    uint16_t classIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo classCP = constant_pool[classIndex - 1];
    assert(classCP.tag == ConstClass);
    ConstClassInfo classInfo = classCP.info.class_info;
    string className = formatar_constante(constant_pool, classInfo.name_index);

    TipoValor value_type;
    int i = 0;
    while (className[i] == '[')
        i++;
    string multiArranjoType = className.substr(i + 1, className.size() - i - 2);
    switch (className[i]) {
    case 'L':
        if (multiArranjoType != "java/lang/String") {
            AreaMetodos& methodArea = AreaMetodos::instancia();
            methodArea.carregar_classe(multiArranjoType);
        }
        value_type = REFERENCIA;
        break;
    case 'B':
        value_type = BYTE;
        break;
    case 'C':
        value_type = CHAR;
        break;
    case 'D':
        value_type = DOUBLE;
        break;
    case 'F':
        value_type = FLOAT;
        break;
    case 'I':
        value_type = INT;
        break;
    case 'J':
        value_type = LONG;
        break;
    case 'S':
        value_type = SHORT;
        break;
    case 'Z':
        value_type = BOOLEANO;
        break;
    default:
        cerr << "Invalid descriptor in multianewarray" << endl;
        exit(1);
    }
    stack<int> count;
    for (int i = 0; i < dimensions; i++) {
        Valor dimLength = top_frame->pop_operand_stack();
        assert(dimLength.tipo == INT);
        count.push(dimLength.dados.valor_int);
    }
    Arranjo* array = new Arranjo((dimensions > 1) ? REFERENCIA : value_type);
    popula_multiarranjo(array, value_type, count);
    Valor arrayValue = faz_valor_referencia(array);
    top_frame->push_operand_stack(arrayValue);
    top_frame->pc += 4;
}

void Executor::ifnull()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor referenceValue = top_frame->pop_operand_stack();
    assert(referenceValue.tipo == REFERENCIA);
    if (referenceValue.dados.objeto == NULL) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branch = (byte1 << 8) | byte2;
        top_frame->pc += branch;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::ifnonnull()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    Valor referenceValue = top_frame->pop_operand_stack();
    assert(referenceValue.tipo == REFERENCIA);
    if (referenceValue.dados.objeto != NULL) {
        u1* code = top_frame->get_code(top_frame->pc);
        u1 byte1 = code[1];
        u1 byte2 = code[2];
        int16_t branch = (byte1 << 8) | byte2;
        top_frame->pc += branch;
    } else {
        top_frame->pc += 3;
    }
}

void Executor::goto_w()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u1 byte3 = code[3];
    u1 byte4 = code[4];
    int32_t branchOffset = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
    top_frame->pc += branchOffset;
}

void Executor::jsr_w()
{
    PilhaExecucao& stack_frame = PilhaExecucao::instancia();
    Frame* top_frame = stack_frame.frame_topo();
    u1* code = top_frame->get_code(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u1 byte3 = code[3];
    u1 byte4 = code[4];
    int32_t branchOffset = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
    Valor returnAddr = faz_valor_endereco_retorno(top_frame->pc + 5);
    top_frame->push_operand_stack(returnAddr);
    top_frame->pc += branchOffset;
    assert((int32_t)top_frame->pc < (int32_t)top_frame->get_code_size());
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
