/*
 * pilha_jvm.hpp
 * Pilha de invocacao (call stack) da nossa JVM.
 *
 * Singleton: o executor consulta sempre PilhaJVM::instancia() para
 * empilhar um novo Quadro quando um metodo e' invocado e
 * desempilha-lo quando o metodo retorna.
 *
 * Nao impomos limite explicito de profundidade. Se quiser tratar
 * StackOverflowError, e' so' comparar tamanho() com FRAME_MAX_SIZE.
 */
#ifndef PILHA_JVM_HPP
#define PILHA_JVM_HPP

#include "quadro.hpp"
#include <stack>

#define FRAME_MAX_SIZE 50

class PilhaJVM {
public:
    static PilhaJVM& instancia()
    {
        static PilhaJVM unica;
        return unica;
    }

    void     empilhar_quadro(Quadro* q);
    Quadro*  topo();
    bool     desempilhar_quadro();
    uint32_t tamanho();

    PilhaJVM(const PilhaJVM&)            = delete;
    PilhaJVM& operator=(const PilhaJVM&) = delete;

private:
    PilhaJVM()  = default;
    ~PilhaJVM() = default;

    std::stack<Quadro*> quadros;
};

#endif
