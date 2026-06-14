/*
 * executor.hpp
 *
 * Executor de bytecode da JVM (= Operations da referencia). Mantem uma tabela
 * de 202 ponteiros para funcao-membro (uma por opcode) e roda o laco principal
 * de despacho: a cada iteracao le o opcode apontado pelo pc do frame do topo e
 * chama a funcao correspondente. CADA funcao de opcode e' responsavel por
 * avancar o pc (a referencia nao centraliza isso).
 *
 * Passo 7 implementa apenas o ESQUELETO + algumas instrucoes triviais (sem
 * dependencia de pool/heap) para validar o mecanismo ponta a ponta. Os demais
 * opcodes apontam para instrucao_nao_implementada() ate o passo 8.
 *
 * Portado de JVM/headers/Operations.hpp (Operations -> Executor). Adaptacoes:
 * BasicTypes.hpp (inexistente) -> tipos_runtime.hpp; nomes em PT.
 */
#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "tipos_runtime.hpp"
#include "classe_estatica.hpp"
#include "frame.hpp"
#include "pilha_execucao.hpp"
#include <string>

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

    Executor(const Executor&)            = delete;
    Executor& operator=(const Executor&) = delete;

private:
    Executor();

    // Preenche tabela_funcoes: todos os slots apontam para
    // instrucao_nao_implementada e, em seguida, os opcodes ja' prontos sao
    // sobrescritos com suas funcoes.
    void init_instrucoes();

    // Tabela de despacho indexada pelo opcode (0x00..0xC9 -> 0..201).
    funcao_generica tabela_funcoes[202];

    // Estado do prefixo `wide` (consumido pelo proximo opcode). Passo 8.
    bool is_wide;

    // Fallback: reporta o opcode ainda nao implementado e encerra.
    void instrucao_nao_implementada();

    /* --- Subconjunto trivial implementado no passo 7 --- */
    void nop();          // 0x00
    void aconst_null();  // 0x01
    void iconst_m1();    // 0x02
    void iconst_0();     // 0x03
    void iconst_1();     // 0x04
    void iconst_2();     // 0x05
    void iconst_3();     // 0x06
    void iconst_4();     // 0x07
    void iconst_5();     // 0x08
    void bipush();       // 0x10
    void sipush();       // 0x11
    void ireturn();      // 0xAC
    void func_return();  // 0xB1 (return)
};

#endif // EXECUTOR_HPP
