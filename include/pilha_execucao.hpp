/*
 * pilha_execucao.hpp
 *
 * Pilha de execucao (pilha de chamadas) da JVM: um singleton que guarda os
 * Frames ativos. O frame do topo e' sempre o metodo em execucao; chamar um
 * metodo empilha um Frame, retornar dele desempilha.
 *
 * Portado de JVM/headers/Stack.hpp + JVM/src/Stack.cpp. Renomeado de Stack
 * para PilhaExecucao para nao colidir com std::stack (a referencia tinha o
 * vicio `Stack& Stack = ...`, que some com o rename).
 */
#ifndef PILHA_EXECUCAO_HPP
#define PILHA_EXECUCAO_HPP

#include "frame.hpp"
#include <cstdint>
#include <stack>

class PilhaExecucao {
public:
    // Unica instancia (singleton).
    static PilhaExecucao& instancia()
    {
        static PilhaExecucao unica;
        return unica;
    }

    // Libera os frames que sobrarem na destruicao.
    ~PilhaExecucao();

    // Empilha um novo frame (passa a ser o frame atual).
    void empilhar_frame(Frame* frame);

    // Retorna o frame do topo (frame atual), ou NULL se vazia.
    Frame* frame_topo();

    // Desempilha e destroi o frame do topo. Retorna false se a pilha estava vazia.
    bool desempilhar_frame();

    // Quantidade de frames atualmente na pilha.
    uint32_t tamanho();

    PilhaExecucao(const PilhaExecucao&)            = delete;
    PilhaExecucao& operator=(const PilhaExecucao&) = delete;

private:
    PilhaExecucao() = default;

    // Frames ativos (ponteiros; a pilha e' dona deles).
    std::stack<Frame*> frames;
};

#endif // PILHA_EXECUCAO_HPP
