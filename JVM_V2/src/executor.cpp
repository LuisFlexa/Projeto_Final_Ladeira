/*
 * executor.cpp
 *
 * Implementacao do Executor. Portado de JVM/src/Operations.cpp (linhas ~1-330,
 * apenas o esqueleto + opcodes triviais). Os designated initializers da
 * referencia (Value v = {.type=..., .data={...}}) sao substituidos pelos
 * helpers faz_valor_* de tipos_runtime.hpp.
 */
#include "executor.hpp"
#include "exibidor_classe.hpp"  // formatar_constante, tabela_mnemonicos
#include "objeto_arranjo.hpp"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

using std::string;
using std::vector;

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

    // <clinit> e' ()V: roda antes do main, mas NAO recebe os argumentos do main
    // (corrige a referencia, que repassava `arguments` ao frame do <clinit>).
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

/* ------------------------------------------------------------------------- */
/* Tabela de despacho                                                        */
/* ------------------------------------------------------------------------- */
void Executor::init_instrucoes()
{
    // Por padrao todo opcode cai no fallback de "nao implementado".
    for (int i = 0; i < 202; i++) {
        tabela_funcoes[i] = &Executor::instrucao_nao_implementada;
    }

    // Opcodes ja' prontos no passo 7.
    tabela_funcoes[0x00] = &Executor::nop;
    tabela_funcoes[0x01] = &Executor::aconst_null;
    tabela_funcoes[0x02] = &Executor::iconst_m1;
    tabela_funcoes[0x03] = &Executor::iconst_0;
    tabela_funcoes[0x04] = &Executor::iconst_1;
    tabela_funcoes[0x05] = &Executor::iconst_2;
    tabela_funcoes[0x06] = &Executor::iconst_3;
    tabela_funcoes[0x07] = &Executor::iconst_4;
    tabela_funcoes[0x08] = &Executor::iconst_5;
    tabela_funcoes[0x10] = &Executor::bipush;
    tabela_funcoes[0x11] = &Executor::sipush;
    tabela_funcoes[0xAC] = &Executor::ireturn;
    tabela_funcoes[0xB1] = &Executor::func_return;
}

void Executor::instrucao_nao_implementada()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    u1* code = topo->get_code(topo->pc);
    u1 opcode = code[0];

    const string& mnem = (opcode < 202) ? tabela_mnemonicos[opcode] : string("???");
    std::cerr << "Opcode nao implementado: 0x" << std::hex << (int)opcode
              << std::dec << " (" << mnem << ") — sera' implementado no passo 8."
              << std::endl;
    exit(2);
}

/* ------------------------------------------------------------------------- */
/* Opcodes triviais (sem dependencia de pool/heap)                           */
/* ------------------------------------------------------------------------- */
void Executor::nop()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->pc++;
}

void Executor::aconst_null()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->push_operand_stack(faz_valor_referencia(NULL));
    topo->pc++;
}

void Executor::iconst_m1()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->push_operand_stack(faz_valor_int(-1));
    topo->pc++;
}

void Executor::iconst_0()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->push_operand_stack(faz_valor_int(0));
    topo->pc++;
}

void Executor::iconst_1()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->push_operand_stack(faz_valor_int(1));
    topo->pc++;
}

void Executor::iconst_2()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->push_operand_stack(faz_valor_int(2));
    topo->pc++;
}

void Executor::iconst_3()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->push_operand_stack(faz_valor_int(3));
    topo->pc++;
}

void Executor::iconst_4()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->push_operand_stack(faz_valor_int(4));
    topo->pc++;
}

void Executor::iconst_5()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    topo->push_operand_stack(faz_valor_int(5));
    topo->pc++;
}

void Executor::bipush()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    u1* code = topo->get_code(topo->pc);
    int8_t byte = (int8_t)code[1];

    // O valor vai como INT na pilha, mas o tipo logico de exibicao e' BYTE.
    Valor valor = faz_valor_int((int32_t)byte);
    valor.tipo_print = BYTE;
    topo->push_operand_stack(valor);
    topo->pc += 2;
}

void Executor::sipush()
{
    Frame* topo = PilhaExecucao::instancia().frame_topo();
    u1* code = topo->get_code(topo->pc);
    uint16_t bruto = (uint16_t)((code[1] << 8) | code[2]);

    // O valor vai como INT na pilha, mas o tipo logico de exibicao e' SHORT.
    Valor valor = faz_valor_int((int32_t)(int16_t)bruto);
    valor.tipo_print = SHORT;
    topo->push_operand_stack(valor);
    topo->pc += 3;
}

void Executor::ireturn()
{
    PilhaExecucao& pilha = PilhaExecucao::instancia();
    Frame* topo = pilha.frame_topo();

    Valor retorno = topo->pop_operand_stack();
    assert(retorno.tipo == INT);

    pilha.desempilhar_frame();
    Frame* novo_topo = pilha.frame_topo();
    novo_topo->push_operand_stack(retorno);
}

void Executor::func_return()
{
    PilhaExecucao::instancia().desempilhar_frame();
}
