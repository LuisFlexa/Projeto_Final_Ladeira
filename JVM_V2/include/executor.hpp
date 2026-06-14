/*
 * executor.hpp
 *
 * Executor de bytecode da JVM (= Operations da referencia). Mantem uma tabela
 * de 202 ponteiros para funcao-membro (uma por opcode) e roda o laco principal
 * de despacho: a cada iteracao le o opcode apontado pelo pc do frame do topo e
 * chama a funcao correspondente. CADA funcao de opcode e' responsavel por
 * avancar o pc (a referencia nao centraliza isso).
 *
 * Passo 8 implementa o conjunto completo de instrucoes (todos os opcodes da
 * tabela 0x00..0xC9 que a referencia trata). Os poucos slots nao preenchidos
 * (ex.: 0xBA invokedynamic, 0xCA..0xFF) caem em instrucao_nao_implementada().
 *
 * Portado de JVM/headers/Operations.hpp (Operations -> Executor). Adaptacoes:
 * BasicTypes.hpp (inexistente) -> tipos_runtime.hpp; nomes em PT; designated
 * initializers (Value v = {.type=...}) -> helpers faz_valor_* de
 * tipos_runtime.hpp.
 */
#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "tipos_runtime.hpp"
#include "classe_estatica.hpp"
#include "frame.hpp"
#include "pilha_execucao.hpp"
#include <stack>
#include <string>

class Arranjo;

class Executor;

// Ponteiro para funcao-membro do Executor: a assinatura de todo opcode.
typedef void (Executor::*funcao_generica)();

class Executor {
public:
    // Unica instancia (singleton).
    static Executor& instancia()
    {
        static Executor unico;
        return unico;
    }

    ~Executor();

    // Empilha o frame de main (e o de <clinit>, se existir) e roda o laco de
    // despacho ate a pilha de execucao esvaziar.
    void executar_metodos(ClasseEstatica* classe_runtime);

    // Verifica se a classe declara um metodo com esse nome+descritor.
    bool verifica_metodo(ClasseEstatica* classe_runtime,
                         const std::string& nome, const std::string& descritor);

    // Preenche um arranjo multidimensional recursivamente (multianewarray).
    void popula_multiarranjo(Arranjo* arranjo, TipoValor tipo_valor,
                             std::stack<int> contagem);

    Executor(const Executor&)            = delete;
    Executor& operator=(const Executor&) = delete;

private:
    Executor();

    // Preenche tabela_funcoes: todos os slots apontam para
    // instrucao_nao_implementada e, em seguida, os opcodes implementados sao
    // sobrescritos com suas funcoes.
    void init_instrucoes();

    // Tabela de despacho indexada pelo opcode (0x00..0xC9 -> 0..201).
    funcao_generica tabela_funcoes[202];

    // Estado do prefixo `wide` (consumido pelo proximo opcode).
    bool is_wide;

    // Fallback: reporta o opcode ainda nao implementado e encerra.
    void instrucao_nao_implementada();

    // Implementacoes das instrucoes da JVM (uma por opcode).
    void nop();
    void aconst_null();
    void iconst_m1();
    void iconst_0();
    void iconst_1();
    void iconst_2();
    void iconst_3();
    void iconst_4();
    void iconst_5();
    void lconst_0();
    void lconst_1();
    void fconst_0();
    void fconst_1();
    void fconst_2();
    void dconst_0();
    void dconst_1();
    void bipush();
    void sipush();
    void ldc();
    void ldc_w();
    void ldc2_w();
    void iload();
    void lload();
    void fload();
    void dload();
    void aload();
    void iload_0();
    void iload_1();
    void iload_2();
    void iload_3();
    void lload_0();
    void lload_1();
    void lload_2();
    void lload_3();
    void fload_0();
    void fload_1();
    void fload_2();
    void fload_3();
    void dload_0();
    void dload_1();
    void dload_2();
    void dload_3();
    void aload_0();
    void aload_1();
    void aload_2();
    void aload_3();
    void iaload();
    void laload();
    void faload();
    void daload();
    void aaload();
    void baload();
    void caload();
    void saload();
    void istore();
    void lstore();
    void fstore();
    void dstore();
    void astore();
    void istore_0();
    void istore_1();
    void istore_2();
    void istore_3();
    void lstore_0();
    void lstore_1();
    void lstore_2();
    void lstore_3();
    void fstore_0();
    void fstore_1();
    void fstore_2();
    void fstore_3();
    void dstore_0();
    void dstore_1();
    void dstore_2();
    void dstore_3();
    void astore_0();
    void astore_1();
    void astore_2();
    void astore_3();
    void iastore();
    void lastore();
    void fastore();
    void dastore();
    void aastore();
    void bastore();
    void castore();
    void sastore();
    void pop();
    void pop2();
    void dup();
    void dup_x1();
    void dup_x2();
    void dup2();
    void dup2_x1();
    void dup2_x2();
    void swap();
    void iadd();
    void ladd();
    void fadd();
    void dadd();
    void isub();
    void lsub();
    void fsub();
    void dsub();
    void imul();
    void lmul();
    void fmul();
    void dmul();
    void idiv();
    void ldiv();
    void fdiv();
    void ddiv();
    void irem();
    void lrem();
    void frem();
    void drem();
    void ineg();
    void lneg();
    void fneg();
    void dneg();
    void ishl();
    void lshl();
    void ishr();
    void lshr();
    void iushr();
    void lushr();
    void iand();
    void land();
    void ior();
    void lor();
    void ixor();
    void lxor();
    void iinc();
    void i2l();
    void i2f();
    void i2d();
    void l2i();
    void l2f();
    void l2d();
    void f2i();
    void f2l();
    void f2d();
    void d2i();
    void d2l();
    void d2f();
    void i2b();
    void i2c();
    void i2s();
    void lcmp();
    void fcmpl();
    void fcmpg();
    void dcmpl();
    void dcmpg();
    void ifeq();
    void ifne();
    void iflt();
    void ifge();
    void ifgt();
    void ifle();
    void if_icmpeq();
    void if_icmpne();
    void if_icmplt();
    void if_icmpge();
    void if_icmpgt();
    void if_icmple();
    void if_acmpeq();
    void if_acmpne();
    void func_goto();
    void jsr();
    void ret();
    void tableswitch();
    void lookupswitch();
    void ireturn();
    void lreturn();
    void freturn();
    void dreturn();
    void areturn();
    void func_return();
    void getstatic();
    void putstatic();
    void getfield();
    void putfield();
    void invokevirtual();
    void invokespecial();
    void invokestatic();
    void invokeinterface();
    void func_new();
    void newarray();
    void anewarray();
    void arraylength();
    void athrow();
    void checkcast();
    void instanceof();
    void monitorenter();
    void monitorexit();
    void wide();
    void multianewarray();
    void ifnull();
    void ifnonnull();
    void goto_w();
    void jsr_w();
};

#endif // EXECUTOR_HPP
