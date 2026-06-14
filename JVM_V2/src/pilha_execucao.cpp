/*
 * pilha_execucao.cpp
 *
 * Implementacao de PilhaExecucao. Portado de JVM/src/Stack.cpp.
 *
 * Nota: a referencia tinha um limite FRAME_MAX_SIZE (50) com StackOverflowError,
 * mas ja' deixava o check comentado. Mantemos sem limite aqui para nao quebrar
 * recursao legitima dos exemplos; tamanho() permite reativar essa politica
 * depois, se necessario.
 */
#include "pilha_execucao.hpp"

// Destrutor: libera todos os frames restantes.
PilhaExecucao::~PilhaExecucao()
{
    while (!frames.empty()) {
        delete frames.top();
        frames.pop();
    }
}

// Empilha um novo frame.
void PilhaExecucao::empilhar_frame(Frame* frame)
{
    frames.push(frame);
}

// Retorna o frame do topo (ou NULL se vazia).
Frame* PilhaExecucao::frame_topo()
{
    if (frames.size() == 0) {
        return NULL;
    }
    return frames.top();
}

// Desempilha e destroi o frame do topo.
bool PilhaExecucao::desempilhar_frame()
{
    if (frames.size() == 0) {
        return false;
    }

    Frame* frame = frames.top();
    frames.pop();
    delete frame;
    return true;
}

// Quantidade de frames na pilha.
uint32_t PilhaExecucao::tamanho()
{
    return frames.size();
}
