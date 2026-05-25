/*
 * executor.cpp
 * Implementacao dos 202 handlers de bytecode + loop de despacho.
 *
 * Cada handler tem a forma void Executor::<mnemonico>() e e' montado
 * em tabela_instrucoes[opcode] pelo construtor. O loop em executar()
 * apenas faz `(this->*tabela_instrucoes[op])()`.
 */
#include "executor.hpp"
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>

using namespace std;

Executor::Executor()
    : is_wide(false)
{
    inicializar_tabela_instrucoes();
}

void Executor::executar(ClasseEstatica* classe_runtime)
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    vector<Value> arguments;
    Value commandLineArgs = {
        .type = ValueType::REFERENCE,
        .data = { .object = new VetorJVM(ValueType::REFERENCE) }
    };
    arguments.push_back(commandLineArgs);
    pilha.empilhar_quadro(new Quadro(classe_runtime, "main", "([Ljava/lang/String;)V", arguments));
    if (metodo_existe(classe_runtime, "<clinit>", "()V")) {
        pilha.empilhar_quadro(new Quadro(classe_runtime, "<clinit>", "()V", arguments));
    }
    while (pilha.tamanho() > 0) {
        Quadro* top_frame = pilha.topo();
        u1* code = top_frame->obter_codigo(top_frame->pc);

        (*this.*tabela_instrucoes[code[0]])();
    }
}

bool Executor::metodo_existe(ClasseEstatica* classe_runtime, const string& name, const string& descriptor)
{
    ArquivoClasse* class_file = classe_runtime->obter_arquivo();
    bool found = false;
    MethodInfo method;
    for (int i = 0; i < class_file->methods_count; i++) {
        method = class_file->methods[i];
        string method_name = formatar_constante(class_file->constant_pool, method.name_index);
        string method_desc = formatar_constante(class_file->constant_pool, method.descriptor_index);
        if (method_name == name && method_desc == descriptor) {
            found = true;
            break;
        }
    }
    return found;
}

void Executor::preencher_multiarray(VetorJVM* array, ValueType value_type, stack<int> count)
{
    int curr_count = count.top();
    count.pop();
    ValueType array_type = (count.size() > 1) ? ValueType::REFERENCE : value_type;
    if (count.size() == 0) {
        for (int i = 0; i < curr_count; i++) {
            Value sub_array_value = {
                .print_type = value_type,
                .type = value_type,
                .data = { .long_value = 0 }
            };
            array->anexar(sub_array_value);
        }
    } else {
        for (int i = 0; i < curr_count; i++) {
            VetorJVM* subarray = new VetorJVM(array_type);
            preencher_multiarray(subarray, value_type, count);
            Value sub_array_value = { 
                .print_type = ValueType::REFERENCE,
                .type = ValueType::REFERENCE, 
                .data = { .object = subarray } };
            array->anexar(sub_array_value);
        }
    }
}

void Executor::nop()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    top_frame->pc++;
}

void Executor::aconst_null()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = { 
        .print_type = ValueType::REFERENCE, 
        .type = ValueType::REFERENCE, 
        .data = { .object = NULL } };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iconst_m1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = -1 }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iconst_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = 0 }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iconst_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = 1 }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iconst_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = 2 }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iconst_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = 3 }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iconst_4()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = 4 }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iconst_5()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = 5 }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::lconst_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    Value value = {
        .print_type = ValueType::LONG,
        value.type = ValueType::LONG,
        value.data = { .long_value = 0 }
    };
    top_frame->empilhar_operando(padding);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::lconst_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    Value value = {
        .print_type = ValueType::LONG,
        value.type = ValueType::LONG,
        value.data = { .long_value = 1 }
    };
    top_frame->empilhar_operando(padding);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::fconst_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        value.print_type = ValueType::FLOAT,
        value.type = ValueType::FLOAT,
        value.data = { .float_value = 0 } 
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::fconst_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        value.print_type = ValueType::FLOAT,
        value.type = ValueType::FLOAT,
        value.data = { .float_value = 1 } 
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::fconst_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = {
        value.print_type = ValueType::FLOAT,
        value.type = ValueType::FLOAT,
        value.data = { .float_value = 2 } 
    };
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::dconst_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    Value value = { 
        value.print_type = ValueType::DOUBLE,
        value.type = ValueType::DOUBLE,
        value.data = { .double_value = 0 }
    };
    top_frame->empilhar_operando(padding);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::dconst_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    Value value = {
        .print_type = ValueType::DOUBLE,
        .type = ValueType::DOUBLE,
        .data =  { .double_value = 1 }
    };
    top_frame->empilhar_operando(padding);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::bipush()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte = code[1];
    Value value = {
        .print_type = ValueType::BYTE,
        .type = ValueType::INT,
        .data = { .int_value = (int32_t)(int8_t)byte }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc += 2;
}

void Executor::sipush()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t short_value = (byte1 << 8) | byte2;
    Value value = {
        .print_type = ValueType::SHORT,
        .type = ValueType::INT,
        .data = { .int_value = (int32_t)(int16_t)short_value }
    };
    top_frame->empilhar_operando(value);
    top_frame->pc += 3;
}

void Executor::ldc()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 index = code[1];
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    ConstantPoolInfo entry = constant_pool[index - 1];
    Value value;
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
        value = {
            .print_type = ValueType::REFERENCE,
            .type = ValueType::REFERENCE,
            .data = { .object = new ObjetoStr(utf8_str) }
        };
    } else if (entry.tag == ConstInt) {
        value = {
            .print_type = ValueType::INT,
            .type = ValueType::INT,
            .data = { .int_value = (int32_t)entry.info.int_info.bytes }
        };
    } else if (entry.tag == ConstFloat) {
        u4 floatBytes = entry.info.float_info.bytes;
        int s = ((floatBytes >> 31) == 0) ? 1 : -1;
        int e = ((floatBytes >> 23) & 0xff);
        int m = (e == 0) ? (floatBytes & 0x7fffff) << 1 : (floatBytes & 0x7fffff) | 0x800000;
        float number = s * m * pow(2, e - 150);
        value = { 
            .print_type = ValueType::FLOAT,
            .type = ValueType::FLOAT,
            .data = { .float_value = number }
        };
    } else {
        
        cerr << "ldc trying to access invalid constant pool element " << entry.tag << endl;
        exit(1);
    }
    top_frame->empilhar_operando(value);
    top_frame->pc += 2;
}

void Executor::ldc_w()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u2 index = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    ConstantPoolInfo entry = constant_pool[index - 1];
    Value value;
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
        value = {
            .print_type = ValueType::REFERENCE,
            .type = ValueType::REFERENCE,
            .data = { .object = new ObjetoStr(utf8_str) }
        };
    } else if (entry.tag == ConstInt) {
        value = {
            .print_type = ValueType::INT,
            .type = ValueType::INT,
            .data = { .int_value = (int)entry.info.int_info.bytes }
        };
    } else if (entry.tag == ConstFloat) {
        u4 floatBytes = entry.info.float_info.bytes;
        int s = ((floatBytes >> 31) == 0) ? 1 : -1;
        int e = ((floatBytes >> 23) & 0xff);
        int m = (e == 0) ? (floatBytes & 0x7fffff) << 1 : (floatBytes & 0x7fffff) | 0x800000;
        float number = s * m * pow(2, e - 150);
        value = {
            .print_type = ValueType::FLOAT,
            .type = ValueType::FLOAT,
            .data = { .float_value = number }
        };
    } else {
        cerr << "ldc_w trying to access invalid constant pool element " << entry.tag << endl;
        exit(1);
    }
    top_frame->empilhar_operando(value);
    top_frame->pc += 3;
}

void Executor::ldc2_w()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u2 index = (byte1 << 8) | byte2;
    ConstantPoolInfo* class_file = *(top_frame->obter_pool_constantes());
    ConstantPoolInfo entry = class_file[index - 1];
    Value value;
    if (entry.tag == ConstLong) {
        u4 highBytes = entry.info.long_info.high_bytes;
        u4 lowBytes = entry.info.long_info.low_bytes;
        int64_t longNumber = ((int64_t)highBytes << 32) + lowBytes;
        value = {
            .print_type = ValueType::LONG,
            .type = ValueType::LONG,
            .data = { .long_value = longNumber }
        };
        Value padding = {
            .print_type = ValueType::PADDING,
            .type = ValueType::PADDING
        };
        top_frame->empilhar_operando(padding);
    } else if (entry.tag == ConstDouble) {
        u4 highBytes = entry.info.double_info.high_bytes;
        u4 lowBytes = entry.info.double_info.low_bytes;
        int64_t longNumber = ((int64_t)highBytes << 32) + lowBytes;
        int32_t s = ((longNumber >> 63) == 0) ? 1 : -1;
        int32_t e = (int32_t)((longNumber >> 52) & 0x7ffL);
        int64_t m = (e == 0) ? (longNumber & 0xfffffffffffffL) << 1 : (longNumber & 0xfffffffffffffL) | 0x10000000000000L;
        double doubleNumber = s * m * pow(2, e - 1075);
        value = {
            .print_type = ValueType::DOUBLE,
            .type = ValueType::DOUBLE,
            .data = { .double_value = doubleNumber }
        };
        Value padding = {
            .print_type = ValueType::PADDING,
            .type = ValueType::PADDING
        };
        top_frame->empilhar_operando(padding);
    } else {
        cerr << "ldc2_w trying to access invalid constant pool element " << entry.tag << endl;
        exit(1);
    }
    top_frame->empilhar_operando(value);
    top_frame->pc += 3;
}

void Executor::iload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > index));
    Value value = top_frame->obter_local(index);
    assert(value.type == ValueType::INT);
    top_frame->empilhar_operando(value);
}

void Executor::lload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > (index + 1)));
    Value value = top_frame->obter_local(index);
    assert(value.type == ValueType::LONG);
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->empilhar_operando(padding);
    top_frame->empilhar_operando(value);
}

void Executor::fload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > index));
    Value value = top_frame->obter_local(index);
    assert(value.type == ValueType::FLOAT);
    top_frame->empilhar_operando(value);
}

void Executor::dload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > (index + 1)));
    Value value = top_frame->obter_local(index);
    assert(value.type == ValueType::DOUBLE);
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->empilhar_operando(padding);
    top_frame->empilhar_operando(value);
}

void Executor::aload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > index));
    Value value = top_frame->obter_local(index);
    assert(value.type == ValueType::REFERENCE);
    top_frame->empilhar_operando(value);
}

void Executor::iload_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(0);
    assert(value.type == ValueType::INT);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iload_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(1);
    assert(value.type == ValueType::INT);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iload_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(2);
    assert(value.type == ValueType::INT);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iload_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(3);
    assert(value.type == ValueType::INT);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::lload_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value;
    value = top_frame->obter_local(1);
    assert(value.type == ValueType::PADDING);
    top_frame->empilhar_operando(value);
    value = top_frame->obter_local(0);
    assert(value.type == ValueType::LONG);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::lload_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value;
    value = top_frame->obter_local(2);
    assert(value.type == ValueType::PADDING);
    top_frame->empilhar_operando(value);
    value = top_frame->obter_local(1);
    assert(value.type == ValueType::LONG);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::lload_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value;
    value = top_frame->obter_local(3);
    assert(value.type == ValueType::PADDING);
    top_frame->empilhar_operando(value);
    value = top_frame->obter_local(2);
    assert(value.type == ValueType::LONG);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::lload_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value;
    value = top_frame->obter_local(4);
    assert(value.type == ValueType::PADDING);
    top_frame->empilhar_operando(value);
    value = top_frame->obter_local(3);
    assert(value.type == ValueType::LONG);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::fload_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(0);
    assert(value.type == ValueType::FLOAT);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::fload_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(1);
    assert(value.type == ValueType::FLOAT);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::fload_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(2);
    assert(value.type == ValueType::FLOAT);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::fload_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(3);
    assert(value.type == ValueType::FLOAT);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::dload_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value;
    value = top_frame->obter_local(1);
    assert(value.type == ValueType::PADDING);
    top_frame->empilhar_operando(value);
    value = top_frame->obter_local(0);
    assert(value.type == ValueType::DOUBLE);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::dload_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value;
    value = top_frame->obter_local(2);
    assert(value.type == ValueType::PADDING);
    top_frame->empilhar_operando(value);
    value = top_frame->obter_local(1);
    assert(value.type == ValueType::DOUBLE);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::dload_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value;
    value = top_frame->obter_local(3);
    assert(value.type == ValueType::PADDING);
    top_frame->empilhar_operando(value);
    value = top_frame->obter_local(2);
    assert(value.type == ValueType::DOUBLE);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::dload_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value;
    value = top_frame->obter_local(4);
    assert(value.type == ValueType::PADDING);
    top_frame->empilhar_operando(value);
    value = top_frame->obter_local(3);
    assert(value.type == ValueType::DOUBLE);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::aload_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(0);
    assert(value.type == ValueType::REFERENCE);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::aload_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(1);
    assert(value.type == ValueType::REFERENCE);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::aload_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(2);
    assert(value.type == ValueType::REFERENCE);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::aload_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->obter_local(3);
    assert(value.type == ValueType::REFERENCE);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::iaload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    top_frame->empilhar_operando(array->obter(index.data.int_value));
    top_frame->pc++;
}

void Executor::laload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if ((signed)index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->empilhar_operando(padding);
    top_frame->empilhar_operando(array->obter(index.data.int_value));
    top_frame->pc++;
}

void Executor::faload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    top_frame->empilhar_operando(array->obter(index.data.int_value));
    top_frame->pc++;
}

void Executor::daload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->empilhar_operando(padding);
    top_frame->empilhar_operando(array->obter(index.data.int_value));
    top_frame->pc++;
}

void Executor::aaload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    top_frame->empilhar_operando(array->obter(index.data.int_value));
    top_frame->pc++;
}

void Executor::baload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Value value = array->obter(index.data.int_value);
    assert(value.type == ValueType::BOOLEAN || value.type == ValueType::BYTE);
    if (value.type == ValueType::BOOLEAN) {
        value = {
            .print_type = ValueType::BOOLEAN,
            .type = ValueType::INT,
            .data = { .int_value = value.data.boolean_value }
        };
    } else {
        value = {
            .print_type = ValueType::BYTE,
            .type = ValueType::INT,
            .data = { .int_value = (int32_t)value.data.byte_value }
        };
    }
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::caload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Value char_value = array->obter(index.data.int_value);
    char_value = {
        .print_type = ValueType::CHAR,
        .type = ValueType::INT,
        .data = { .int_value = char_value.data.char_value }
    };
    top_frame->empilhar_operando(char_value);
    top_frame->pc++;
}

void Executor::saload()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    Value short_value = array->obter(index.data.int_value);
    short_value = {
        .print_type = ValueType::SHORT,
        .type = ValueType::INT,
        .data = { .int_value = (int32_t)short_value.data.short_value }
    };
    top_frame->empilhar_operando(short_value);
    top_frame->pc++;
}

void Executor::istore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > index));
    top_frame->definir_local(value, index);
}

void Executor::lstore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::LONG);
    top_frame->desempilhar_operando();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > (index + 1)));
    top_frame->definir_local(value, index);
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->definir_local(padding, index + 1);
}

void Executor::fstore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::FLOAT);
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > index));
    top_frame->definir_local(value, index);
}

void Executor::dstore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::DOUBLE);
    top_frame->desempilhar_operando();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > (index + 1)));
    top_frame->definir_local(value, index);
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->definir_local(padding, index + 1);
}

void Executor::astore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::REFERENCE);
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    assert(((int16_t)(top_frame->max_locais()) > index));
    top_frame->definir_local(value, index);
}

void Executor::istore_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    top_frame->definir_local(value, 0);
    top_frame->pc++;
}

void Executor::istore_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    top_frame->definir_local(value, 1);
    top_frame->pc++;
}

void Executor::istore_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    top_frame->definir_local(value, 2);
    top_frame->pc++;
}

void Executor::istore_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    top_frame->definir_local(value, 3);
    top_frame->pc++;
}

void Executor::lstore_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::LONG);
    top_frame->definir_local(value, 0);
    value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::PADDING);
    top_frame->definir_local(value, 1);
    top_frame->pc++;
}

void Executor::lstore_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::LONG);
    top_frame->definir_local(value, 1);
    value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::PADDING);
    top_frame->definir_local(value, 2);
    top_frame->pc++;
}

void Executor::lstore_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::LONG);
    top_frame->definir_local(value, 2);
    value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::PADDING);
    top_frame->definir_local(value, 3);
    top_frame->pc++;
}

void Executor::lstore_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::LONG);
    top_frame->definir_local(value, 3);
    value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::PADDING);
    top_frame->definir_local(value, 4);
    top_frame->pc++;
}

void Executor::fstore_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::FLOAT);
    top_frame->definir_local(value, 0);
    top_frame->pc++;
}

void Executor::fstore_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::FLOAT);
    top_frame->definir_local(value, 1);
    top_frame->pc++;
}

void Executor::fstore_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::FLOAT);
    top_frame->definir_local(value, 2);
    top_frame->pc++;
}

void Executor::fstore_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::FLOAT);
    top_frame->definir_local(value, 3);
    top_frame->pc++;
}

void Executor::dstore_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::DOUBLE);
    top_frame->definir_local(value, 0);
    value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::PADDING);
    top_frame->definir_local(value, 1);
    top_frame->pc++;
}

void Executor::dstore_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::DOUBLE);
    top_frame->definir_local(value, 1);
    value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::PADDING);
    top_frame->definir_local(value, 2);
    top_frame->pc++;
}

void Executor::dstore_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::DOUBLE);
    top_frame->definir_local(value, 2);
    value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::PADDING);
    top_frame->definir_local(value, 3);
    top_frame->pc++;
}

void Executor::dstore_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::DOUBLE);
    top_frame->definir_local(value, 3);
    value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::PADDING);
    top_frame->definir_local(value, 4);
    top_frame->pc++;
}

void Executor::astore_0()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::REFERENCE);
    top_frame->definir_local(value, 0);
    top_frame->pc++;
}

void Executor::astore_1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::REFERENCE);
    top_frame->definir_local(value, 1);
    top_frame->pc++;
}

void Executor::astore_2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::REFERENCE);
    top_frame->definir_local(value, 2);
    top_frame->pc++;
}

void Executor::astore_3()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::REFERENCE);
    top_frame->definir_local(value, 3);
    top_frame->pc++;
}

void Executor::iastore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value >= (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    value.print_type = ValueType::INT;
    assert(value.type == array->tipo_conteudo());
    array->alterar(index.data.int_value, value);
    top_frame->pc++;
}

void Executor::lastore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::LONG);
    Value padding = top_frame->desempilhar_operando();
    assert(padding.type == ValueType::PADDING);
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value >= (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(value.type == array->tipo_conteudo());
    array->alterar(index.data.int_value, value);
    top_frame->pc++;
}

void Executor::fastore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::FLOAT);
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value >= (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(value.type == array->tipo_conteudo());
    array->alterar(index.data.int_value, value);
    top_frame->pc++;
}

void Executor::dastore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::DOUBLE);
    Value padding = top_frame->desempilhar_operando();
    assert(padding.type == ValueType::PADDING);
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value >= (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    assert(value.type == array->tipo_conteudo());
    array->alterar(index.data.int_value, value);
    top_frame->pc++;
}

void Executor::aastore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::REFERENCE);
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value >= (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    array->alterar(index.data.int_value, value);
    top_frame->pc++;
}

void Executor::bastore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    assert(array->tipo_conteudo() == ValueType::BOOLEAN || array->tipo_conteudo() == ValueType::BYTE);
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    if (array->tipo_conteudo() == ValueType::BOOLEAN) {
        value = {
            .print_type = ValueType::BOOLEAN,
            .type = ValueType::BOOLEAN,
            .data = { .boolean_value = (value.data.int_value != 0) ? true : false }
        };
    } else {
        value = {
            .print_type = ValueType::BYTE,
            .type = ValueType::BYTE,
            .data = { .byte_value = (int8_t)value.data.int_value }
        };
    }
    array->alterar(index.data.int_value, value);
    top_frame->pc++;
}

void Executor::castore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    value = {
        .print_type = ValueType::CHAR,
        .type = ValueType::CHAR,
        .data = { .char_value = (uint8_t)value.data.int_value }
    };
    array->alterar(index.data.int_value, value);
    top_frame->pc++;
}

void Executor::sastore()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    VetorJVM* array;
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    Value index = top_frame->desempilhar_operando();
    assert(index.type == ValueType::INT);
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    assert((arrayref.data.object)->tipo_objeto() == ObjectType::ARRAY);
    array = (VetorJVM*)arrayref.data.object;
    if (array == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    if (index.data.int_value > (signed)array->tamanho() || index.data.int_value < 0) {
        cerr << "ArrayIndexOutOfBoundsException" << endl;
        exit(2);
    }
    value = {
        .print_type = ValueType::SHORT,
        .type = ValueType::SHORT,
        .data = { .short_value = (int16_t)value.data.int_value }
    };
    array->alterar(index.data.int_value, value);
    top_frame->pc++;
}

void Executor::pop()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type != ValueType::LONG);
    assert(value.type != ValueType::DOUBLE);
    top_frame->pc++;
}

void Executor::pop2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    top_frame->pc++;
}

void Executor::dup()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type != ValueType::LONG);
    assert(value.type != ValueType::DOUBLE);
    top_frame->empilhar_operando(value);
    top_frame->empilhar_operando(value);
    top_frame->pc++;
}

void Executor::dup_x1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type != ValueType::LONG);
    assert(value_1.type != ValueType::DOUBLE);
    Value value_2 = top_frame->desempilhar_operando();
    assert(value_2.type != ValueType::LONG);
    assert(value_2.type != ValueType::DOUBLE);
    top_frame->empilhar_operando(value_1);
    top_frame->empilhar_operando(value_2);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::dup_x2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_3 = top_frame->desempilhar_operando();
    assert(value_1.type != ValueType::LONG);
    assert(value_1.type != ValueType::DOUBLE);
    assert(value_3.type != ValueType::LONG);
    assert(value_3.type != ValueType::DOUBLE);
    top_frame->empilhar_operando(value_1);
    top_frame->empilhar_operando(value_3);
    top_frame->empilhar_operando(value_2);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::dup2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    Value value_2 = top_frame->desempilhar_operando();
    assert(value_2.type != ValueType::LONG);
    assert(value_2.type != ValueType::DOUBLE);
    top_frame->empilhar_operando(value_2);
    top_frame->empilhar_operando(value_1);
    top_frame->empilhar_operando(value_2);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::dup2_x1()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_3 = top_frame->desempilhar_operando();
    assert(value_2.type != ValueType::LONG);
    assert(value_2.type != ValueType::DOUBLE);
    assert(value_3.type != ValueType::LONG);
    assert(value_3.type != ValueType::DOUBLE);
    top_frame->empilhar_operando(value_2);
    top_frame->empilhar_operando(value_1);
    top_frame->empilhar_operando(value_3);
    top_frame->empilhar_operando(value_2);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::dup2_x2()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_3 = top_frame->desempilhar_operando();
    Value value_4 = top_frame->desempilhar_operando();
    assert(value_2.type != ValueType::LONG);
    assert(value_2.type != ValueType::DOUBLE);
    assert(value_4.type != ValueType::LONG);
    assert(value_4.type != ValueType::DOUBLE);
    top_frame->empilhar_operando(value_2);
    top_frame->empilhar_operando(value_1);
    top_frame->empilhar_operando(value_4);
    top_frame->empilhar_operando(value_3);
    top_frame->empilhar_operando(value_2);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::swap()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    Value value_2 = top_frame->desempilhar_operando();
    assert(value_1.type != ValueType::LONG);
    assert(value_1.type != ValueType::DOUBLE);
    assert(value_2.type != ValueType::LONG);
    assert(value_2.type != ValueType::DOUBLE);
    top_frame->empilhar_operando(value_1);
    top_frame->empilhar_operando(value_2);
    top_frame->pc++;
}

void Executor::iadd()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    value_1.data.int_value = value_1.data.int_value + (value_2.data.int_value);
    value_1.print_type = ValueType::INT;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::ladd()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);
    value_1.data.long_value = value_1.data.long_value + (value_2.data.long_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::fadd()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::FLOAT);
    assert(value_1.type == ValueType::FLOAT);
    value_1.data.float_value = value_1.data.float_value + (value_2.data.float_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::dadd()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::DOUBLE);
    assert(value_1.type == ValueType::DOUBLE);
    value_1.data.double_value = value_1.data.double_value + (value_2.data.double_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::isub()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    value_1.print_type = ValueType::INT;
    value_1.data.int_value = value_1.data.int_value - (value_2.data.int_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lsub()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);
    value_1.data.long_value = value_1.data.long_value - (value_2.data.long_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::fsub()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::FLOAT);
    assert(value_1.type == ValueType::FLOAT);
    value_1.data.float_value = value_1.data.float_value - (value_2.data.float_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::dsub()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::DOUBLE);
    assert(value_1.type == ValueType::DOUBLE);
    value_1.data.double_value = value_1.data.double_value - (value_2.data.double_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::imul()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    value_1.print_type = ValueType::INT;
    value_1.data.int_value = value_1.data.int_value * (value_2.data.int_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lmul()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);
    value_1.data.long_value = value_1.data.long_value * (value_2.data.long_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::fmul()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::FLOAT);
    assert(value_1.type == ValueType::FLOAT);
    value_1.data.float_value = value_1.data.float_value * (value_2.data.float_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::dmul()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::DOUBLE);
    assert(value_1.type == ValueType::DOUBLE);
    value_1.data.double_value = value_1.data.double_value * (value_2.data.double_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::idiv()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    if (value_2.data.int_value == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.print_type = ValueType::INT;
    value_1.data.int_value = value_1.data.int_value / (value_2.data.int_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::ldiv()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);
    if (value_2.data.long_value == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.data.long_value = value_1.data.long_value / (value_2.data.long_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::fdiv()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::FLOAT);
    assert(value_1.type == ValueType::FLOAT);
    if (value_2.data.float_value == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.data.float_value = value_1.data.float_value / (value_2.data.float_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::ddiv()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::DOUBLE);
    assert(value_1.type == ValueType::DOUBLE);
    if (value_2.data.double_value == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.data.double_value = value_1.data.double_value / (value_2.data.double_value);
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::irem()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    if (value_2.data.int_value == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }
    value_1.print_type = ValueType::INT;
    value_1.data.int_value = value_1.data.int_value - (value_1.data.int_value / value_2.data.int_value) * value_2.data.int_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lrem()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);
    if (value_2.data.long_value == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    value_1.data.long_value = value_1.data.long_value - (value_1.data.long_value / value_2.data.long_value) * value_2.data.long_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::frem()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::FLOAT);
    assert(value_1.type == ValueType::FLOAT);
    if (value_2.data.float_value == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    value_1.data.float_value = value_1.data.float_value
        - ((uint32_t)(value_1.data.float_value / value_2.data.float_value)) * value_2.data.float_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::drem()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_2.type == ValueType::DOUBLE);
    assert(value_1.type == ValueType::DOUBLE);
    if (value_2.data.double_value == 0) {
        cerr << "ArithmeticException" << endl;
        exit(2);
    }

    value_1.data.double_value = value_1.data.double_value
        - ((uint64_t)(value_1.data.double_value / value_2.data.double_value)) * value_2.data.double_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::ineg()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::INT);
    value_1.print_type = ValueType::INT;
    value_1.data.int_value = -value_1.data.int_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lneg()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();

    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::LONG);
    value_1.data.long_value = -value_1.data.long_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::fneg()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::FLOAT);
    value_1.data.float_value = -value_1.data.float_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::dneg()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();

    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::DOUBLE);
    value_1.data.double_value = -value_1.data.double_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::ishl()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);

    value_2.data.int_value = 0x1f & value_2.data.int_value;
    value_1.data.int_value = value_1.data.int_value << value_2.data.int_value;
    value_1.print_type = ValueType::INT;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lshl()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::LONG);
    value_2.data.long_value = 0x3f & value_2.data.long_value;
    value_1.data.long_value = (value_1.data.long_value) << value_2.data.int_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::ishr()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);

    value_2.data.int_value = 0x1f & value_2.data.int_value;
    value_1.data.int_value = value_1.data.int_value >> value_2.data.int_value;
    value_1.print_type = ValueType::INT;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lshr()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::LONG);

    value_2.data.long_value = 0x3f & value_2.data.long_value;
    value_1.data.long_value = value_1.data.long_value >> value_2.data.long_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::iushr()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    value_2.data.int_value = 0x1f & value_2.data.int_value;
    value_1.data.int_value = value_1.data.int_value >> value_2.data.int_value;
    if (value_1.data.int_value < 0) {
        value_1.data.int_value = value_1.data.int_value + (2 << ~(value_2.data.int_value));
    }
    value_1.print_type = ValueType::INT;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lushr()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::LONG);
    value_2.data.int_value = 0x3f & value_2.data.int_value;
    value_1.data.long_value = value_1.data.long_value >> value_2.data.int_value;
    if (value_1.data.long_value < 0) {
        value_1.data.long_value = value_1.data.long_value + ((int64_t)2 << ~(value_2.data.int_value));
    }
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::iand()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    value_1.print_type = ValueType::INT;
    value_1.data.int_value = value_1.data.int_value & value_2.data.int_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::land()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);

    value_1.data.long_value = value_1.data.long_value & value_2.data.long_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::ior()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    value_1.print_type = ValueType::INT;
    value_1.data.int_value = value_1.data.int_value | value_2.data.int_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lor()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);
    value_1.data.long_value = value_1.data.long_value | value_2.data.long_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::ixor()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_2.type == ValueType::INT);
    assert(value_1.type == ValueType::INT);
    value_1.print_type = ValueType::INT;
    value_1.data.int_value = value_1.data.int_value ^ value_2.data.int_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lxor()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);
    value_1.data.long_value = value_1.data.long_value ^ value_2.data.long_value;
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::iinc()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u2 index = 0;
    if (is_wide) {
        index = (code[1] << 8) | code[2];
    } else {
        index += code[1];
    }
    Value localVariable = top_frame->obter_local(index);
    assert(localVariable.type == ValueType::INT);
    int32_t inc;
    if (is_wide) {
        uint16_t constant = (code[3] << 8) | code[4];
        inc = (int32_t)(int16_t)constant;
    } else {
        inc = (int32_t)(int8_t)code[2];
    }
    localVariable.data.int_value += inc;
    top_frame->definir_local(localVariable, index);
    top_frame->pc += is_wide ? 5 : 3;
    is_wide = false;
}

void Executor::i2l()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::INT);
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->empilhar_operando(padding);
    value_1 = {
        .print_type = ValueType::LONG,
        .type = ValueType::LONG,
        .data = { .long_value = (int64_t)value_1.data.int_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::i2f()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::INT);
    value_1 = {
        .print_type = ValueType::FLOAT,
        .type = ValueType::FLOAT,
        .data = { .float_value = (float)value_1.data.int_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::i2d()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::INT);
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->empilhar_operando(padding);
    value_1 = {
        .print_type = ValueType::DOUBLE,
        .type = ValueType::DOUBLE,
        .data = { .double_value = (double)value_1.data.int_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::l2i()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::LONG);
    value_1 = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = (int32_t)value_1.data.int_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::l2f()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::LONG);
    value_1 = {
        .print_type = ValueType::FLOAT,
        .type = ValueType::FLOAT,
        .data = { .float_value = (float)value_1.data.long_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::l2d()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_1.type == ValueType::LONG);
    value_1 = {
        .print_type = ValueType::DOUBLE,
        .type = ValueType::DOUBLE,
        .data = { .double_value = (double)value_1.data.long_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::f2i()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::FLOAT);
    value_1 = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = (int32_t)value_1.data.float_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::f2l()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::FLOAT);
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->empilhar_operando(padding);
    value_1 = {
        .print_type = ValueType::LONG,
        .type = ValueType::LONG,
        .data = { .long_value = (int64_t)value_1.data.float_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::f2d()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::FLOAT);
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    top_frame->empilhar_operando(padding);
    value_1 = {
        .print_type = ValueType::DOUBLE,
        .type = ValueType::DOUBLE,
        .data = { .double_value = (double)value_1.data.float_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::d2i()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::DOUBLE);
    value_1 = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = (int32_t)value_1.data.double_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::d2l()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();

    assert(value_1.type == ValueType::DOUBLE);
    value_1 = {
        .print_type = ValueType::LONG,
        .type = ValueType::LONG,
        .data = { .long_value = (int64_t)value_1.data.double_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::d2f()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::DOUBLE);
    value_1 = {
        .print_type = ValueType::FLOAT,
        .type = ValueType::FLOAT,
        .data = { .float_value = (float)value_1.data.double_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::i2b()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::INT);
    value_1 = {
        .print_type = ValueType::BYTE,
        .type = ValueType::INT,
        .data = { .int_value = (int32_t)(int8_t)value_1.data.int_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::i2c()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::INT);
    value_1 = {
        .print_type = ValueType::CHAR,
        .type = ValueType::INT,
        .data = { .char_value = (uint8_t)value_1.data.int_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::i2s()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_1 = top_frame->desempilhar_operando();
    assert(value_1.type == ValueType::INT);
    value_1 = {
        .print_type = ValueType::SHORT,
        .type = ValueType::INT,
        .data = { .short_value = (int16_t)value_1.data.int_value }
    };
    top_frame->empilhar_operando(value_1);
    top_frame->pc++;
}

void Executor::lcmp()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value result = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
    };
    assert(value_2.type == ValueType::LONG);
    assert(value_1.type == ValueType::LONG);
    if (value_1.data.long_value > value_2.data.long_value) {
        result.data.int_value = 1;
    } else if (value_1.data.long_value == value_2.data.long_value) {
        result.data.int_value = 0;
    } else {
        result.data.int_value = -1;
    }
    top_frame->empilhar_operando(result);
    top_frame->pc++;
}

void Executor::fcmpl()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    Value result = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
    };
    assert(value_2.type == ValueType::FLOAT);
    assert(value_1.type == ValueType::FLOAT);
    if (isnan(value_1.data.float_value) || isnan(value_2.data.float_value)) {
        result.data.int_value = -1;
    } else if (value_1.data.float_value > value_2.data.float_value) {
        result.data.int_value = 1;
    } else if (value_1.data.float_value == value_2.data.float_value) {
        result.data.int_value = 0;
    } else {
        result.data.int_value = -1;
    }
    top_frame->empilhar_operando(result);
    top_frame->pc++;
}

void Executor::fcmpg()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    Value result = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
    };
    assert(value_2.type == ValueType::FLOAT);
    assert(value_1.type == ValueType::FLOAT);
    if (isnan(value_1.data.float_value) || isnan(value_2.data.float_value)) {
        result.data.int_value = 1;
    } else if (value_1.data.float_value > value_2.data.float_value) {
        result.data.int_value = 1;
    } else if (value_1.data.float_value == value_2.data.float_value) {
        result.data.int_value = 0;
    } else {
        result.data.int_value = -1;
    }
    top_frame->empilhar_operando(result);
    top_frame->pc++;
}

void Executor::dcmpl()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value result = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
    };
    assert(value_2.type == ValueType::DOUBLE);
    assert(value_1.type == ValueType::DOUBLE);
    if (isnan(value_1.data.double_value) || isnan(value_2.data.double_value)) {
        result.data.int_value = -1;
    } else if (value_1.data.double_value > value_2.data.double_value) {
        result.data.int_value = 1;
    } else if (value_1.data.double_value == value_2.data.double_value) {
        result.data.int_value = 0;
    } else {
        result.data.int_value = -1;
    }
    top_frame->empilhar_operando(result);
    top_frame->pc++;
}

void Executor::dcmpg()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value_2 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value value_1 = top_frame->desempilhar_operando();
    top_frame->desempilhar_operando();
    Value result = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
    };
    assert(value_2.type == ValueType::DOUBLE);
    assert(value_1.type == ValueType::DOUBLE);
    if (isnan(value_1.data.double_value) || isnan(value_2.data.double_value)) {
        result.data.int_value = 1;
    } else if (value_1.data.double_value > value_2.data.double_value) {
        result.data.int_value = 1;
    } else if (value_1.data.double_value == value_2.data.double_value) {
        result.data.int_value = 0;
    } else {
        result.data.int_value = -1;
    }
    top_frame->empilhar_operando(result);
    top_frame->pc++;
}

void Executor::ifeq()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    if (value.data.int_value == 0) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    if (value.data.int_value != 0) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    if (value.data.int_value < 0) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    if (value.data.int_value >= 0) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    if (value.data.int_value > 0) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value = top_frame->desempilhar_operando();
    assert(value.type == ValueType::INT);
    if (value.data.int_value <= 0) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value2 = top_frame->desempilhar_operando();
    Value value1 = top_frame->desempilhar_operando();
    assert(value1.type == ValueType::INT);
    assert(value2.type == ValueType::INT);
    if (value1.data.int_value == value2.data.int_value) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value2 = top_frame->desempilhar_operando();
    Value value1 = top_frame->desempilhar_operando();
    assert(value1.type == ValueType::INT);
    assert(value2.type == ValueType::INT);
    if (value1.data.int_value != value2.data.int_value) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value2 = top_frame->desempilhar_operando();
    Value value1 = top_frame->desempilhar_operando();
    assert(value1.type == ValueType::INT);
    assert(value2.type == ValueType::INT);
    if (value1.data.int_value < value2.data.int_value) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value2 = top_frame->desempilhar_operando();
    Value value1 = top_frame->desempilhar_operando();
    assert(value1.type == ValueType::INT);
    assert(value2.type == ValueType::INT);
    if (value1.data.int_value >= value2.data.int_value) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value2 = top_frame->desempilhar_operando();
    Value value1 = top_frame->desempilhar_operando();
    assert(value1.type == ValueType::INT);
    assert(value2.type == ValueType::INT);
    if (value1.data.int_value > value2.data.int_value) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value2 = top_frame->desempilhar_operando();
    Value value1 = top_frame->desempilhar_operando();
    assert(value1.type == ValueType::INT);
    assert(value2.type == ValueType::INT);
    if (value1.data.int_value <= value2.data.int_value) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value2 = top_frame->desempilhar_operando();
    Value value1 = top_frame->desempilhar_operando();
    assert(value1.type == ValueType::REFERENCE);
    assert(value2.type == ValueType::REFERENCE);
    if (value1.data.object == value2.data.object) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value value2 = top_frame->desempilhar_operando();
    Value value1 = top_frame->desempilhar_operando();
    assert(value1.type == ValueType::REFERENCE);
    assert(value2.type == ValueType::REFERENCE);
    if (value1.data.object != value2.data.object) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    int16_t branchOffset = (byte1 << 8) | byte2;
    top_frame->pc += branchOffset;
}

void Executor::jsr()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    int16_t branchOffset = (byte1 << 8) | byte2;
    Value returnAddr = {
        .print_type = ValueType::RETURN_ADDR,
        .type = ValueType::RETURN_ADDR,
        .data = { .return_address = top_frame->pc + 3 }
    };
    top_frame->empilhar_operando(returnAddr);
    top_frame->pc += branchOffset;
}

void Executor::ret()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    uint16_t index = (uint16_t)byte1;
    if (is_wide) {
        u1 byte2 = code[2];
        index = (byte1 << 8) | byte2;
    }
    assert(((int16_t)(top_frame->max_locais()) > index));
    Value value = top_frame->obter_local(index);
    assert(value.type == ValueType::RETURN_ADDR);
    top_frame->definir_local(value, index);
    top_frame->pc = value.data.return_address;
    is_wide = false;
}

void Executor::tableswitch()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    Value keyValue = top_frame->desempilhar_operando();
    assert(keyValue.type == ValueType::INT);
    int32_t key = keyValue.data.int_value;
    uint32_t i;
    uint32_t baseIndex = padding + 13;
    int32_t offsets = highbytes - lowbytes + 1;
    bool matched = false;
    for (i = 0; i < (unsigned)offsets; i++) {
        if (key == (int32_t)lowbytes) {
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    Value keyValue = top_frame->desempilhar_operando();
    assert(keyValue.type == ValueType::INT);
    int32_t key = keyValue.data.int_value;
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value returnValue = top_frame->desempilhar_operando();
    assert(returnValue.type == ValueType::INT);
    pilha.desempilhar_quadro();
    Quadro* newTopFrame = pilha.topo();
    newTopFrame->empilhar_operando(returnValue);
}

void Executor::lreturn()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value returnValue = top_frame->desempilhar_operando();
    assert(returnValue.type == ValueType::LONG);
    assert(top_frame->desempilhar_operando().type == ValueType::PADDING);
    pilha.desempilhar_quadro();
    Quadro* newTopFrame = pilha.topo();
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    newTopFrame->empilhar_operando(padding);
    newTopFrame->empilhar_operando(returnValue);
}

void Executor::freturn()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value returnValue = top_frame->desempilhar_operando();
    assert(returnValue.type == ValueType::FLOAT);
    pilha.desempilhar_quadro();
    Quadro* newTopFrame = pilha.topo();
    newTopFrame->empilhar_operando(returnValue);
}

void Executor::dreturn()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value returnValue = top_frame->desempilhar_operando();
    assert(returnValue.type == ValueType::DOUBLE);
    assert(top_frame->desempilhar_operando().type == ValueType::PADDING);
    pilha.desempilhar_quadro();
    Quadro* newTopFrame = pilha.topo();
    Value padding = {
        .print_type = ValueType::PADDING,
        .type = ValueType::PADDING
    };
    newTopFrame->empilhar_operando(padding);
    newTopFrame->empilhar_operando(returnValue);
}

void Executor::areturn()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value returnValue = top_frame->desempilhar_operando();
    assert(returnValue.type == ValueType::REFERENCE);
    pilha.desempilhar_quadro();
    Quadro* newTopFrame = pilha.topo();
    newTopFrame->empilhar_operando(returnValue);
}

void Executor::func_return()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    pilha.desempilhar_quadro();
}

void Executor::getstatic()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    ClasseEstatica* classe_runtime = methodArea.carregar_classe(className);
    while (classe_runtime != NULL) {
        if (classe_runtime->tem_campo(fieldName) == false) {
            if (classe_runtime->obter_arquivo()->super_class == 0) {
                classe_runtime = NULL;
            } else {
                string superClassName = formatar_constante(classe_runtime->obter_arquivo()->constant_pool,
                    classe_runtime->obter_arquivo()->super_class);
                classe_runtime = methodArea.carregar_classe(superClassName);
            }
        } else {
            break;
        }
    }
    if (classe_runtime == NULL) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }

    if (pilha.topo() != top_frame)
        return;
    Value staticValue = classe_runtime->obter_campo(fieldName);
    switch (staticValue.type) {
    case ValueType::BOOLEAN:
        staticValue.type = ValueType::INT;
        staticValue.print_type = ValueType::BOOLEAN;
        break;
    case ValueType::BYTE:
        staticValue.type = ValueType::INT;
        staticValue.print_type = ValueType::BYTE;
        break;
    case ValueType::SHORT:
        staticValue.type = ValueType::INT;
        staticValue.print_type = ValueType::SHORT;
        break;
    case ValueType::INT:
        staticValue.type = ValueType::INT;
        staticValue.print_type = ValueType::INT;
        break;
    default:
        break;
    }
    if (staticValue.type == ValueType::DOUBLE || staticValue.type == ValueType::LONG) {
        Value paddingValue = {
            .print_type = ValueType::PADDING,
            .type = ValueType::PADDING
        };
        top_frame->empilhar_operando(paddingValue);
    }
    top_frame->empilhar_operando(staticValue);
    top_frame->pc += 3;
}

void Executor::putstatic()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    ClasseEstatica* classe_runtime = methodArea.carregar_classe(className);
    while (classe_runtime != NULL) {
        if (classe_runtime->tem_campo(fieldName) == false) {
            if (classe_runtime->obter_arquivo()->super_class == 0) {
                classe_runtime = NULL;
            } else {
                string superClassName = formatar_constante(classe_runtime->obter_arquivo()->constant_pool,
                    classe_runtime->obter_arquivo()->super_class);
                classe_runtime = methodArea.carregar_classe(superClassName);
            }
        } else {
            break;
        }
    }
    if (classe_runtime == NULL) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }

    if (pilha.topo() != top_frame)
        return;
    Value topValue = top_frame->desempilhar_operando();
    if (topValue.type == ValueType::DOUBLE || topValue.type == ValueType::LONG) {
        top_frame->desempilhar_operando();
    } else {
        switch (fieldDescriptor[0]) {
        case 'B':
            topValue.type = ValueType::BYTE;
            topValue.print_type = ValueType::BYTE;
            break;
        case 'C':
            topValue.type = ValueType::CHAR;
            topValue.print_type = ValueType::CHAR;
            break;
        case 'S':
            topValue.type = ValueType::SHORT;
            topValue.print_type = ValueType::SHORT;
            break;
        case 'Z':
            topValue.type = ValueType::BOOLEAN;
            topValue.print_type = ValueType::BOOLEAN;
            break;
        }
    }
    classe_runtime->definir_campo(topValue, fieldName);
    top_frame->pc += 3;
}

void Executor::getfield()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    Value objectValue = top_frame->desempilhar_operando();
    assert(objectValue.type == ValueType::REFERENCE);
    ObjetoBase* object = objectValue.data.object;
    assert(object->tipo_objeto() == ObjectType::CLASS_INSTANCE);
    InstanciaClasse* classInstance = (InstanciaClasse*)object;
    if (!classInstance->tem_campo(fieldName)) {
        cerr << "NoSuchFieldError" << endl;
        exit(1);
    }
    Value fieldValue = classInstance->obter_campo(fieldName);
    switch (fieldValue.type) {
    case ValueType::BOOLEAN:
        fieldValue.type = ValueType::INT;
        fieldValue.print_type = ValueType::BOOLEAN;
        break;
    case ValueType::BYTE:
        fieldValue.type = ValueType::INT;
        fieldValue.print_type = ValueType::BYTE;
        break;
    case ValueType::SHORT:
        fieldValue.type = ValueType::INT;
        fieldValue.print_type = ValueType::SHORT;
        break;
    case ValueType::INT:
        fieldValue.type = ValueType::INT;
        fieldValue.print_type = ValueType::INT;
        break;
    default:
        break;
    }
    if (fieldValue.type == ValueType::DOUBLE || fieldValue.type == ValueType::LONG) {
        Value paddingValue = {
            .print_type = ValueType::PADDING,
            .type = ValueType::PADDING
        };
        top_frame->empilhar_operando(paddingValue);
    }
    top_frame->empilhar_operando(fieldValue);
    top_frame->pc += 3;
}

void Executor::putfield()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    Value valueToBeInserted = top_frame->desempilhar_operando();
    if (valueToBeInserted.type == ValueType::DOUBLE || valueToBeInserted.type == ValueType::LONG) {
        top_frame->desempilhar_operando();
    } else {
        switch (fieldDescriptor[0]) {
        case 'B':
            valueToBeInserted.type = ValueType::BYTE;
            valueToBeInserted.print_type = ValueType::BYTE;
            break;
        case 'C':
            valueToBeInserted.type = ValueType::CHAR;
            valueToBeInserted.print_type = ValueType::CHAR;
            break;
        case 'S':
            valueToBeInserted.type = ValueType::SHORT;
            valueToBeInserted.print_type = ValueType::SHORT;
            break;
        case 'Z':
            valueToBeInserted.type = ValueType::BOOLEAN;
            valueToBeInserted.print_type = ValueType::BOOLEAN;
            break;
        }
    }
    Value objectValue = top_frame->desempilhar_operando();
    assert(objectValue.type == ValueType::REFERENCE);
    ObjetoBase* object = objectValue.data.object;
    assert(object->tipo_objeto() == ObjectType::CLASS_INSTANCE);
    InstanciaClasse* classInstance = (InstanciaClasse*)object;
    classInstance->definir_campo(valueToBeInserted, fieldName);
    top_frame->pc += 3;
}

void Executor::invokevirtual()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    stack<Value> operandStackBackup = top_frame->copiar_pilha_operandos();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
                Value printValue = top_frame->desempilhar_operando();
                if (printValue.type == ValueType::INT) {
                    switch (printValue.print_type) {
                    case ValueType::BOOLEAN:
                        cout << (printValue.data.boolean_value == 0 ? "false" : "true");
                        break;
                    case ValueType::BYTE:
                        cout << (int)printValue.data.byte_value;
                        break;
                    case ValueType::CHAR:
                        cout << printValue.data.char_value;
                        break;
                    case ValueType::SHORT:
                        cout << printValue.data.short_value;
                        break;
                    default:
                        cout << printValue.data.int_value;
                        break;
                    }
                } else {
                    switch (printValue.type) {
                    case ValueType::DOUBLE:
                        top_frame->desempilhar_operando();
                        cout << printValue.data.double_value;
                        break;
                    case ValueType::FLOAT:
                        cout << printValue.data.float_value;
                        break;
                    case ValueType::LONG:
                        top_frame->desempilhar_operando();
                        cout << printValue.data.long_value;
                        break;
                    case ValueType::REFERENCE:
                        assert(printValue.data.object->tipo_objeto() == ObjectType::STRING_INSTANCE);
                        cout << ((ObjetoStr*)printValue.data.object)->obter_str().c_str();
                        break;
                    case ValueType::BOOLEAN:
                        cout << (printValue.data.boolean_value == 0 ? "false" : "true");
                        break;
                    case ValueType::BYTE:
                        cout << (int)printValue.data.byte_value;
                        break;
                    case ValueType::CHAR:
                        cout << printValue.data.char_value;
                        break;
                    case ValueType::SHORT:
                        cout << printValue.data.short_value;
                        break;
                    default:
                        cerr << "Invalid print type:" << printValue.type << endl;
                        exit(1);
                        break;
                    }
                }
            }
            if (method_name == "println")
                cout << "\n";
        } else if (className == "java/lang/String" && method_name == "equals") {
            Value strValue1 = top_frame->desempilhar_operando();
            Value strValue2 = top_frame->desempilhar_operando();
            assert(strValue1.type == ValueType::REFERENCE);
            assert(strValue2.type == ValueType::REFERENCE);
            assert(strValue1.data.object->tipo_objeto() == ObjectType::STRING_INSTANCE);
            assert(strValue2.data.object->tipo_objeto() == ObjectType::STRING_INSTANCE);
            ObjetoStr* str1 = (ObjetoStr*)strValue1.data.object;
            ObjetoStr* str2 = (ObjetoStr*)strValue2.data.object;
            Value result = {
                .print_type = ValueType::INT,
                .type = ValueType::INT,
            };
            if (str1->obter_str() == str2->obter_str()) {
                result.data.int_value = 1;
            } else {
                result.data.int_value = 0;
            }
            top_frame->empilhar_operando(result);
        } else if (className == "java/lang/String" && method_name == "length") {
            Value strValue = top_frame->desempilhar_operando();
            assert(strValue.type == ValueType::REFERENCE);
            assert(strValue.data.object->tipo_objeto() == ObjectType::STRING_INSTANCE);
            ObjetoStr* str = (ObjetoStr*)strValue.data.object;
            Value result = {
                .print_type = ValueType::INT,
                .type = ValueType::INT,
                .data = { .int_value = (int32_t)(str->obter_str()).size() }
            };
            top_frame->empilhar_operando(result);
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
        vector<Value> args;
        for (int i = 0; i < nargs; i++) {
            Value value = top_frame->desempilhar_operando();
            if (value.type == ValueType::PADDING) {
                args.insert(args.begin() + 1, value);
            } else {
                args.insert(args.begin(), value);
            }
        }
        Value objectValue = top_frame->desempilhar_operando();
        assert(objectValue.type == ValueType::REFERENCE);
        args.insert(args.begin(), objectValue);
        ObjetoBase* object = objectValue.data.object;
        assert(object->tipo_objeto() == ObjectType::CLASS_INSTANCE);
        InstanciaClasse* instance = (InstanciaClasse*)object;
        AreaMetodos& methodArea = AreaMetodos::instancia();
        ClasseEstatica* classe_runtime = methodArea.carregar_classe(className);
        Quadro* newFrame = new Quadro(instance, classe_runtime, method_name, methodDescriptor, args);

        if (pilha.topo() != top_frame) {
            top_frame->carregar_pilha_operandos(operandStackBackup);
            delete newFrame;
            return;
        }
        pilha.empilhar_quadro(newFrame);
    }
    top_frame->pc += 3;
}

void Executor::invokespecial()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    stack<Value> operandStackBackup = top_frame->copiar_pilha_operandos();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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

    if ((className == "java/lang/Object" || className == "java/lang/String") && method_name == "<init>") {
        if (className == "java/lang/String") {
            top_frame->desempilhar_operando();
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
        vector<Value> args;
        for (int i = 0; i < nargs; i++) {
            Value value = top_frame->desempilhar_operando();
            if (value.type == ValueType::PADDING) {
                args.insert(args.begin() + 1, value);
            } else {
                args.insert(args.begin(), value);
            }
        }
        Value objectValue = top_frame->desempilhar_operando();
        assert(objectValue.type == ValueType::REFERENCE);
        args.insert(args.begin(), objectValue);
        ObjetoBase* object = objectValue.data.object;
        assert(object->tipo_objeto() == ObjectType::CLASS_INSTANCE);
        InstanciaClasse* instance = (InstanciaClasse*)object;
        AreaMetodos& methodArea = AreaMetodos::instancia();
        ClasseEstatica* classe_runtime = methodArea.carregar_classe(className);
        Quadro* newFrame = new Quadro(instance, classe_runtime, method_name, methodDescriptor, args);

        if (pilha.topo() != top_frame) {
            top_frame->carregar_pilha_operandos(operandStackBackup);
            delete newFrame;
            return;
        }
        pilha.empilhar_quadro(newFrame);
    }
    top_frame->pc += 3;
}

void Executor::invokestatic()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    stack<Value> operandStackBackup = top_frame->copiar_pilha_operandos();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
    if (className == "java/lang/Object" && method_name == "registerNatives") {
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
        vector<Value> args;
        for (int i = 0; i < nargs; i++) {
            Value value = top_frame->desempilhar_operando();
            if (value.type == ValueType::PADDING) {
                args.insert(args.begin() + 1, value);
            } else {
                args.insert(args.begin(), value);
            }
        }
        AreaMetodos& methodArea = AreaMetodos::instancia();
        ClasseEstatica* classe_runtime = methodArea.carregar_classe(className);
        Quadro* newFrame = new Quadro(classe_runtime, method_name, methodDescriptor, args);

        if (pilha.topo() != top_frame) {
            top_frame->carregar_pilha_operandos(operandStackBackup);
            delete newFrame;
            return;
        }
        pilha.empilhar_quadro(newFrame);
    }
    top_frame->pc += 3;
}

void Executor::invokeinterface()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    stack<Value> operandStackBackup = top_frame->copiar_pilha_operandos();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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
        vector<Value> args;
        for (int i = 0; i < nargs; i++) {
            Value value = top_frame->desempilhar_operando();
            if (value.type == ValueType::PADDING) {
                args.insert(args.begin() + 1, value);
            } else {
                args.insert(args.begin(), value);
            }
        }
        Value objectValue = top_frame->desempilhar_operando();
        assert(objectValue.type == ValueType::REFERENCE);
        args.insert(args.begin(), objectValue);
        ObjetoBase* object = objectValue.data.object;
        assert(object->tipo_objeto() == ObjectType::CLASS_INSTANCE);
        InstanciaClasse* instance = (InstanciaClasse*)object;
        AreaMetodos& methodArea = AreaMetodos::instancia();
        methodArea.carregar_classe(className);
        Quadro* newFrame = new Quadro(instance, instance->obter_runtime(), method_name, methodDescriptor, args);

        if (pilha.topo() != top_frame) {
            top_frame->carregar_pilha_operandos(operandStackBackup);
            delete newFrame;
            return;
        }
        pilha.empilhar_quadro(newFrame);
    }
    top_frame->pc += 5;
}

void Executor::func_new()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    uint16_t classIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo classCP = constant_pool[classIndex - 1];
    assert(classCP.tag == ConstClass);
    ConstClassInfo classInfo = classCP.info.class_info;
    string className = formatar_constante(constant_pool, classInfo.name_index);
    ObjetoBase* object;
    if (className == "java/lang/String") {
        object = new ObjetoStr();
    } else {
        AreaMetodos& methodArea = AreaMetodos::instancia();
        ClasseEstatica* classe_runtime = methodArea.carregar_classe(className);
        object = new InstanciaClasse(classe_runtime);
    }

    Value objectref = {
        .print_type = ValueType::REFERENCE,
        .type = ValueType::REFERENCE,
        .data = { .object = object }
    };
    top_frame->empilhar_operando(objectref);
    top_frame->pc += 3;
}

void Executor::newarray()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value count = top_frame->desempilhar_operando();
    assert(count.type == ValueType::INT);
    if (count.data.int_value < 0) {
        cerr << "NegativeArraySizeException" << endl;
        exit(1);
    }
    VetorJVM* array = nullptr;
    Value value;
    value.data.long_value = 0;
    u1* code = top_frame->obter_codigo(top_frame->pc);
    switch (code[1]) {
    case 4:
        array = new VetorJVM(ValueType::BOOLEAN);
        value = {
            .print_type = ValueType::BOOLEAN,
            .type = ValueType::BOOLEAN
        };
        for (int i = 0; i < count.data.int_value; i++) {
            array->anexar(value);
        }
        break;
    case 5:
        array = new VetorJVM(ValueType::CHAR);
        value = {
            .print_type = ValueType::CHAR,
            .type = ValueType::CHAR
        };
        for (int i = 0; i < count.data.int_value; i++) {
            array->anexar(value);
        }
        break;
    case 6:
        array = new VetorJVM(ValueType::FLOAT);
        value = {
            .print_type = ValueType::FLOAT,
            .type = ValueType::FLOAT
        };
        for (int i = 0; i < count.data.int_value; i++) {
            array->anexar(value);
        }
        break;
    case 7:
        array = new VetorJVM(ValueType::DOUBLE);
        value = {
            .print_type = ValueType::DOUBLE,
            .type = ValueType::DOUBLE
        };
        for (int i = 0; i < count.data.int_value; i++) {
            array->anexar(value);
        }
        break;
    case 8:
        array = new VetorJVM(ValueType::BYTE);
        value = {
            .print_type = ValueType::BYTE,
            .type = ValueType::BYTE
        };
        for (int i = 0; i < count.data.int_value; i++) {
            array->anexar(value);
        }
        break;
    case 9:
        array = new VetorJVM(ValueType::SHORT);
        value = {
            .print_type = ValueType::SHORT,
            .type = ValueType::SHORT
        };
        for (int i = 0; i < count.data.int_value; i++) {
            array->anexar(value);
        }
        break;
    case 10:
        array = new VetorJVM(ValueType::INT);
        value = {
            .print_type = ValueType::INT,
            .type = ValueType::INT
        };
        for (int i = 0; i < count.data.int_value; i++) {
            array->anexar(value);
        }
        break;
    case 11:
        array = new VetorJVM(ValueType::LONG);
        value = {
            .print_type = ValueType::LONG,
            .type = ValueType::LONG
        };
        for (int i = 0; i < count.data.int_value; i++) {
            array->anexar(value);
        }
        break;
    }
    Value arrayref = {
        .print_type = ValueType::REFERENCE,
        .type = ValueType::REFERENCE,
        .data = { .object = array }
    };
    top_frame->empilhar_operando(arrayref);
    top_frame->pc += 2;
}

void Executor::anewarray()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value count = top_frame->desempilhar_operando();
    assert(count.type == ValueType::INT);
    if (count.data.int_value < 0) {
        cerr << "NegativeArraySizeException" << endl;
        exit(1);
    }
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
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

    Value objectref = {
        .print_type = ValueType::REFERENCE,
        .type = ValueType::REFERENCE,
        .data = { .object = new VetorJVM(ValueType::REFERENCE) }
    };

    Value nullValue = {
        .print_type = ValueType::REFERENCE,
        .type = ValueType::REFERENCE,
        .data = { .object = NULL }
    };
    for (int i = 0; i < count.data.int_value; i++) {
        ((VetorJVM*)objectref.data.object)->anexar(nullValue);
    }
    top_frame->empilhar_operando(objectref);
    top_frame->pc += 3;
}

void Executor::arraylength()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value arrayref = top_frame->desempilhar_operando();
    assert(arrayref.type == ValueType::REFERENCE);
    if (arrayref.data.object == NULL) {
        cerr << "NullPointerException" << endl;
        exit(1);
    }
    Value length = {
        .print_type = ValueType::INT,
        .type = ValueType::INT,
        .data = { .int_value = (int)((VetorJVM*)arrayref.data.object)->tamanho() }
    };
    top_frame->empilhar_operando(length);
    top_frame->pc++;
}

void Executor::athrow()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    top_frame->pc++;
}

void Executor::checkcast()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    AreaMetodos& methodArea = AreaMetodos::instancia();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u2 cpIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    ConstantPoolInfo cpElement = constant_pool[cpIndex - 1];
    assert(cpElement.tag == ConstClass);
    string className = formatar_constante(constant_pool, cpIndex);
    Value objectrefValue = top_frame->desempilhar_operando();
    assert(objectrefValue.type == ValueType::REFERENCE);
    Value resultValue = {
        .print_type = ValueType::INT,
        .type = ValueType::INT
    };
    if (objectrefValue.data.object == NULL) {
        cerr << "ClassCastException" << endl;
        exit(1);
    } else {
        ObjetoBase* obj = objectrefValue.data.object;
        if (obj->tipo_objeto() == ObjectType::CLASS_INSTANCE) {
            InstanciaClasse* classInstance = (InstanciaClasse*)obj;
            ClasseEstatica* classe_runtime = classInstance->obter_runtime();
            bool found = false;
            while (!found) {
                ArquivoClasse* class_file = classe_runtime->obter_arquivo();
                string currClassName = formatar_constante(class_file->constant_pool, class_file->this_class);
                if (currClassName == className) {
                    found = true;
                } else {
                    if (class_file->super_class == 0) {
                        break;
                    } else {
                        string superClassName = formatar_constante(class_file->constant_pool, class_file->this_class);
                        classe_runtime = methodArea.carregar_classe(superClassName);
                    }
                }
            }
            resultValue.data.int_value = found ? 1 : 0;
        } else if (obj->tipo_objeto() == ObjectType::STRING_INSTANCE) {
            resultValue.data.int_value = (className == "java/lang/String" || className == "java/lang/Object") ? 1 : 0;
        } else {
            if (className == "java/lang/Object") {
                resultValue.data.int_value = 1;
            } else {
                resultValue.data.int_value = 0;
            }
        }
    }
    top_frame->empilhar_operando(resultValue);
    top_frame->pc += 3;
}

void Executor::instanceof()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    AreaMetodos& methodArea = AreaMetodos::instancia();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u2 cpIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    ConstantPoolInfo cpElement = constant_pool[cpIndex - 1];
    assert(cpElement.tag == ConstClass);
    string className = formatar_constante(constant_pool, cpIndex);
    Value objectrefValue = top_frame->desempilhar_operando();
    assert(objectrefValue.type == ValueType::REFERENCE);
    Value resultValue = {
        .print_type = ValueType::INT,
        .type = ValueType::INT
    };
    if (objectrefValue.data.object == NULL) {
        resultValue.data.int_value = 0;
    } else {
        ObjetoBase* obj = objectrefValue.data.object;
        if (obj->tipo_objeto() == ObjectType::CLASS_INSTANCE) {
            InstanciaClasse* classInstance = (InstanciaClasse*)obj;
            ClasseEstatica* classe_runtime = classInstance->obter_runtime();
            bool found = false;
            while (!found) {
                ArquivoClasse* class_file = classe_runtime->obter_arquivo();
                string currClassName = formatar_constante(class_file->constant_pool, class_file->this_class);
                if (currClassName == className) {
                    found = true;
                } else {
                    if (class_file->super_class == 0) {
                        break;
                    } else {
                        string superClassName = formatar_constante(class_file->constant_pool, class_file->this_class);
                        classe_runtime = methodArea.carregar_classe(superClassName);
                    }
                }
            }
            resultValue.data.int_value = found ? 1 : 0;
        } else if (obj->tipo_objeto() == ObjectType::STRING_INSTANCE) {
            resultValue.data.int_value = (className == "java/lang/String" || className == "java/lang/Object") ? 1 : 0;
        } else {
            if (className == "java/lang/Object") {
                resultValue.data.int_value = 1;
            } else {
                resultValue.data.int_value = 0;
            }
        }
    }
    top_frame->empilhar_operando(resultValue);
    top_frame->pc += 3;
}

void Executor::monitorenter()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    top_frame->pc++;
}

void Executor::monitorexit()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    top_frame->pc++;
}

void Executor::wide()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    is_wide = true;
    top_frame->pc++;
}

void Executor::multianewarray()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    ConstantPoolInfo* constant_pool = *(top_frame->obter_pool_constantes());
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u1 dimensions = code[3];
    assert(dimensions >= 1);
    uint16_t classIndex = (byte1 << 8) | byte2;
    ConstantPoolInfo classCP = constant_pool[classIndex - 1];
    assert(classCP.tag == ConstClass);
    ConstClassInfo classInfo = classCP.info.class_info;
    string className = formatar_constante(constant_pool, classInfo.name_index);

    ValueType value_type;
    int i = 0;
    while (className[i] == '[')
        i++;
    string multiArrayType = className.substr(i + 1, className.size() - i - 2);
    switch (className[i]) {
    case 'L':
        if (multiArrayType != "java/lang/String") {
            AreaMetodos& methodArea = AreaMetodos::instancia();
            methodArea.carregar_classe(multiArrayType);
        }
        value_type = ValueType::REFERENCE;
        break;
    case 'B':
        value_type = ValueType::BYTE;
        break;
    case 'C':
        value_type = ValueType::CHAR;
        break;
    case 'D':
        value_type = ValueType::DOUBLE;
        break;
    case 'F':
        value_type = ValueType::FLOAT;
        break;
    case 'I':
        value_type = ValueType::INT;
        break;
    case 'J':
        value_type = ValueType::LONG;
        break;
    case 'S':
        value_type = ValueType::SHORT;
        break;
    case 'Z':
        value_type = ValueType::BOOLEAN;
        break;
    default:
        cerr << "Invalid descriptor in multianewarray" << endl;
        exit(1);
    }
    stack<int> count;
    for (int i = 0; i < dimensions; i++) {
        Value dimLength = top_frame->desempilhar_operando();
        assert(dimLength.type == ValueType::INT);
        count.push(dimLength.data.int_value);
    }
    VetorJVM* array = new VetorJVM((dimensions > 1) ? ValueType::REFERENCE : value_type);
    preencher_multiarray(array, value_type, count);
    Value arrayValue = {
        .print_type = ValueType::REFERENCE,
        .type = ValueType::REFERENCE,
        .data = { .object = array }
    };
    top_frame->empilhar_operando(arrayValue);
    top_frame->pc += 4;
}

void Executor::ifnull()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value referenceValue = top_frame->desempilhar_operando();
    assert(referenceValue.type == ValueType::REFERENCE);
    if (referenceValue.data.object == NULL) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    Value referenceValue = top_frame->desempilhar_operando();
    assert(referenceValue.type == ValueType::REFERENCE);
    if (referenceValue.data.object != NULL) {
        u1* code = top_frame->obter_codigo(top_frame->pc);
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
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u1 byte3 = code[3];
    u1 byte4 = code[4];
    int32_t branchOffset = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
    top_frame->pc += branchOffset;
}

void Executor::jsr_w()
{
    PilhaJVM& pilha = PilhaJVM::instancia();
    Quadro* top_frame = pilha.topo();
    u1* code = top_frame->obter_codigo(top_frame->pc);
    u1 byte1 = code[1];
    u1 byte2 = code[2];
    u1 byte3 = code[3];
    u1 byte4 = code[4];
    int32_t branchOffset = (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
    Value returnAddr = { 
        .print_type = ValueType::RETURN_ADDR, 
        .type = ValueType::RETURN_ADDR, 
        .data = { .return_address = top_frame->pc + 5 }};
    top_frame->empilhar_operando(returnAddr);
    top_frame->pc += branchOffset;
    assert((int32_t)top_frame->pc < (int32_t)top_frame->tamanho_codigo());
}

void Executor::inicializar_tabela_instrucoes()
{
    tabela_instrucoes[0x00] = &Executor::nop;
    tabela_instrucoes[0x01] = &Executor::aconst_null;
    tabela_instrucoes[0x02] = &Executor::iconst_m1;
    tabela_instrucoes[0x03] = &Executor::iconst_0;
    tabela_instrucoes[0x04] = &Executor::iconst_1;
    tabela_instrucoes[0x05] = &Executor::iconst_2;
    tabela_instrucoes[0x06] = &Executor::iconst_3;
    tabela_instrucoes[0x07] = &Executor::iconst_4;
    tabela_instrucoes[0x08] = &Executor::iconst_5;
    tabela_instrucoes[0x09] = &Executor::lconst_0;
    tabela_instrucoes[0x0a] = &Executor::lconst_1;
    tabela_instrucoes[0x0b] = &Executor::fconst_0;
    tabela_instrucoes[0x0c] = &Executor::fconst_1;
    tabela_instrucoes[0x0d] = &Executor::fconst_2;
    tabela_instrucoes[0x0e] = &Executor::dconst_0;
    tabela_instrucoes[0x0f] = &Executor::dconst_1;
    tabela_instrucoes[0x10] = &Executor::bipush;
    tabela_instrucoes[0x11] = &Executor::sipush;
    tabela_instrucoes[0x12] = &Executor::ldc;
    tabela_instrucoes[0x13] = &Executor::ldc_w;
    tabela_instrucoes[0x14] = &Executor::ldc2_w;
    tabela_instrucoes[0x15] = &Executor::iload;
    tabela_instrucoes[0x16] = &Executor::lload;
    tabela_instrucoes[0x17] = &Executor::fload;
    tabela_instrucoes[0x18] = &Executor::dload;
    tabela_instrucoes[0x19] = &Executor::aload;
    tabela_instrucoes[0x1a] = &Executor::iload_0;
    tabela_instrucoes[0x1b] = &Executor::iload_1;
    tabela_instrucoes[0x1c] = &Executor::iload_2;
    tabela_instrucoes[0x1d] = &Executor::iload_3;
    tabela_instrucoes[0x1e] = &Executor::lload_0;
    tabela_instrucoes[0x1f] = &Executor::lload_1;
    tabela_instrucoes[0x20] = &Executor::lload_2;
    tabela_instrucoes[0x21] = &Executor::lload_3;
    tabela_instrucoes[0x22] = &Executor::fload_0;
    tabela_instrucoes[0x23] = &Executor::fload_1;
    tabela_instrucoes[0x24] = &Executor::fload_2;
    tabela_instrucoes[0x25] = &Executor::fload_3;
    tabela_instrucoes[0x26] = &Executor::dload_0;
    tabela_instrucoes[0x27] = &Executor::dload_1;
    tabela_instrucoes[0x28] = &Executor::dload_2;
    tabela_instrucoes[0x29] = &Executor::dload_3;
    tabela_instrucoes[0x2a] = &Executor::aload_0;
    tabela_instrucoes[0x2b] = &Executor::aload_1;
    tabela_instrucoes[0x2c] = &Executor::aload_2;
    tabela_instrucoes[0x2d] = &Executor::aload_3;
    tabela_instrucoes[0x2e] = &Executor::iaload;
    tabela_instrucoes[0x2f] = &Executor::laload;
    tabela_instrucoes[0x30] = &Executor::faload;
    tabela_instrucoes[0x31] = &Executor::daload;
    tabela_instrucoes[0x32] = &Executor::aaload;
    tabela_instrucoes[0x33] = &Executor::baload;
    tabela_instrucoes[0x34] = &Executor::caload;
    tabela_instrucoes[0x35] = &Executor::saload;
    tabela_instrucoes[0x36] = &Executor::istore;
    tabela_instrucoes[0x37] = &Executor::lstore;
    tabela_instrucoes[0x38] = &Executor::fstore;
    tabela_instrucoes[0x39] = &Executor::dstore;
    tabela_instrucoes[0x3a] = &Executor::astore;
    tabela_instrucoes[0x3b] = &Executor::istore_0;
    tabela_instrucoes[0x3c] = &Executor::istore_1;
    tabela_instrucoes[0x3d] = &Executor::istore_2;
    tabela_instrucoes[0x3e] = &Executor::istore_3;
    tabela_instrucoes[0x3f] = &Executor::lstore_0;
    tabela_instrucoes[0x40] = &Executor::lstore_1;
    tabela_instrucoes[0x41] = &Executor::lstore_2;
    tabela_instrucoes[0x42] = &Executor::lstore_3;
    tabela_instrucoes[0x43] = &Executor::fstore_0;
    tabela_instrucoes[0x44] = &Executor::fstore_1;
    tabela_instrucoes[0x45] = &Executor::fstore_2;
    tabela_instrucoes[0x46] = &Executor::fstore_3;
    tabela_instrucoes[0x47] = &Executor::dstore_0;
    tabela_instrucoes[0x48] = &Executor::dstore_1;
    tabela_instrucoes[0x49] = &Executor::dstore_2;
    tabela_instrucoes[0x4a] = &Executor::dstore_3;
    tabela_instrucoes[0x4b] = &Executor::astore_0;
    tabela_instrucoes[0x4c] = &Executor::astore_1;
    tabela_instrucoes[0x4d] = &Executor::astore_2;
    tabela_instrucoes[0x4e] = &Executor::astore_3;
    tabela_instrucoes[0x4f] = &Executor::iastore;
    tabela_instrucoes[0x50] = &Executor::lastore;
    tabela_instrucoes[0x51] = &Executor::fastore;
    tabela_instrucoes[0x52] = &Executor::dastore;
    tabela_instrucoes[0x53] = &Executor::aastore;
    tabela_instrucoes[0x54] = &Executor::bastore;
    tabela_instrucoes[0x55] = &Executor::castore;
    tabela_instrucoes[0x56] = &Executor::sastore;
    tabela_instrucoes[0x57] = &Executor::pop;
    tabela_instrucoes[0x58] = &Executor::pop2;
    tabela_instrucoes[0x59] = &Executor::dup;
    tabela_instrucoes[0x5a] = &Executor::dup2_x1;
    tabela_instrucoes[0x5b] = &Executor::dup2_x2;
    tabela_instrucoes[0x5c] = &Executor::dup2;
    tabela_instrucoes[0x5d] = &Executor::dup2_x1;
    tabela_instrucoes[0x5e] = &Executor::dup2_x2;
    tabela_instrucoes[0x5f] = &Executor::swap;
    tabela_instrucoes[0x60] = &Executor::iadd;
    tabela_instrucoes[0x61] = &Executor::ladd;
    tabela_instrucoes[0x62] = &Executor::fadd;
    tabela_instrucoes[0x63] = &Executor::dadd;
    tabela_instrucoes[0x64] = &Executor::isub;
    tabela_instrucoes[0x65] = &Executor::lsub;
    tabela_instrucoes[0x66] = &Executor::fsub;
    tabela_instrucoes[0x67] = &Executor::dsub;
    tabela_instrucoes[0x68] = &Executor::imul;
    tabela_instrucoes[0x69] = &Executor::lmul;
    tabela_instrucoes[0x6a] = &Executor::fmul;
    tabela_instrucoes[0x6b] = &Executor::dmul;
    tabela_instrucoes[0x6c] = &Executor::idiv;
    tabela_instrucoes[0x6d] = &Executor::ldiv;
    tabela_instrucoes[0x6e] = &Executor::fdiv;
    tabela_instrucoes[0x6f] = &Executor::ddiv;
    tabela_instrucoes[0x70] = &Executor::irem;
    tabela_instrucoes[0x71] = &Executor::lrem;
    tabela_instrucoes[0x72] = &Executor::frem;
    tabela_instrucoes[0x73] = &Executor::drem;
    tabela_instrucoes[0x74] = &Executor::ineg;
    tabela_instrucoes[0x75] = &Executor::lneg;
    tabela_instrucoes[0x76] = &Executor::fneg;
    tabela_instrucoes[0x77] = &Executor::dneg;
    tabela_instrucoes[0x78] = &Executor::ishl;
    tabela_instrucoes[0x79] = &Executor::lshl;
    tabela_instrucoes[0x7a] = &Executor::ishr;
    tabela_instrucoes[0x7b] = &Executor::lshr;
    tabela_instrucoes[0x7c] = &Executor::iushr;
    tabela_instrucoes[0x7d] = &Executor::lushr;
    tabela_instrucoes[0x7e] = &Executor::iand;
    tabela_instrucoes[0x7f] = &Executor::land;
    tabela_instrucoes[0x80] = &Executor::ior;
    tabela_instrucoes[0x81] = &Executor::lor;
    tabela_instrucoes[0x82] = &Executor::ixor;
    tabela_instrucoes[0x83] = &Executor::lxor;
    tabela_instrucoes[0x84] = &Executor::iinc;
    tabela_instrucoes[0x85] = &Executor::i2l;
    tabela_instrucoes[0x86] = &Executor::i2f;
    tabela_instrucoes[0x87] = &Executor::i2d;
    tabela_instrucoes[0x88] = &Executor::l2i;
    tabela_instrucoes[0x89] = &Executor::l2f;
    tabela_instrucoes[0x8a] = &Executor::l2d;
    tabela_instrucoes[0x8b] = &Executor::f2i;
    tabela_instrucoes[0x8c] = &Executor::f2l;
    tabela_instrucoes[0x8d] = &Executor::f2d;
    tabela_instrucoes[0x8e] = &Executor::d2i;
    tabela_instrucoes[0x8f] = &Executor::d2l;
    tabela_instrucoes[0x90] = &Executor::d2f;
    tabela_instrucoes[0x91] = &Executor::i2b;
    tabela_instrucoes[0x92] = &Executor::i2c;
    tabela_instrucoes[0x93] = &Executor::i2s;
    tabela_instrucoes[0x94] = &Executor::lcmp;
    tabela_instrucoes[0x95] = &Executor::fcmpl;
    tabela_instrucoes[0x96] = &Executor::fcmpg;
    tabela_instrucoes[0x97] = &Executor::dcmpl;
    tabela_instrucoes[0x98] = &Executor::dcmpg;
    tabela_instrucoes[0x99] = &Executor::ifeq;
    tabela_instrucoes[0x9a] = &Executor::ifne;
    tabela_instrucoes[0x9b] = &Executor::iflt;
    tabela_instrucoes[0x9c] = &Executor::ifge;
    tabela_instrucoes[0x9d] = &Executor::ifgt;
    tabela_instrucoes[0x9e] = &Executor::ifle;
    tabela_instrucoes[0x9f] = &Executor::if_icmpeq;
    tabela_instrucoes[0xa0] = &Executor::if_icmpne;
    tabela_instrucoes[0xa1] = &Executor::if_icmplt;
    tabela_instrucoes[0xa2] = &Executor::if_icmpge;
    tabela_instrucoes[0xa3] = &Executor::if_icmpgt;
    tabela_instrucoes[0xa4] = &Executor::if_icmple;
    tabela_instrucoes[0xa5] = &Executor::if_acmpeq;
    tabela_instrucoes[0xa6] = &Executor::if_acmpne;
    tabela_instrucoes[0xa7] = &Executor::func_goto;
    tabela_instrucoes[0xa8] = &Executor::jsr;
    tabela_instrucoes[0xa9] = &Executor::ret;
    tabela_instrucoes[0xaa] = &Executor::tableswitch;
    tabela_instrucoes[0xab] = &Executor::lookupswitch;
    tabela_instrucoes[0xac] = &Executor::ireturn;
    tabela_instrucoes[0xad] = &Executor::lreturn;
    tabela_instrucoes[0xae] = &Executor::freturn;
    tabela_instrucoes[0xaf] = &Executor::dreturn;
    tabela_instrucoes[0xb0] = &Executor::areturn;
    tabela_instrucoes[0xb1] = &Executor::func_return;
    tabela_instrucoes[0xb2] = &Executor::getstatic;
    tabela_instrucoes[0xb3] = &Executor::putstatic;
    tabela_instrucoes[0xb4] = &Executor::getfield;
    tabela_instrucoes[0xb5] = &Executor::putfield;
    tabela_instrucoes[0xb6] = &Executor::invokevirtual;
    tabela_instrucoes[0xb7] = &Executor::invokespecial;
    tabela_instrucoes[0xb8] = &Executor::invokestatic;
    tabela_instrucoes[0xb9] = &Executor::invokeinterface;
    tabela_instrucoes[0xbb] = &Executor::func_new;
    tabela_instrucoes[0xbc] = &Executor::newarray;
    tabela_instrucoes[0xbd] = &Executor::anewarray;
    tabela_instrucoes[0xbe] = &Executor::arraylength;
    tabela_instrucoes[0xbf] = &Executor::athrow;
    tabela_instrucoes[0xc0] = &Executor::checkcast;
    tabela_instrucoes[0xc1] = &Executor::instanceof;
    tabela_instrucoes[0xc2] = &Executor::monitorenter;
    tabela_instrucoes[0xc3] = &Executor::monitorexit;
    tabela_instrucoes[0xc4] = &Executor::wide;
    tabela_instrucoes[0xc5] = &Executor::multianewarray;
    tabela_instrucoes[0xc6] = &Executor::ifnull;
    tabela_instrucoes[0xc7] = &Executor::ifnonnull;
    tabela_instrucoes[0xc8] = &Executor::goto_w;
    tabela_instrucoes[0xc9] = &Executor::jsr_w;
}
