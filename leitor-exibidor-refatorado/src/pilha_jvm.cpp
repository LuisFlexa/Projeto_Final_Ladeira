#include "pilha_jvm.hpp"

void PilhaJVM::empilhar_quadro(Quadro* q)
{
    quadros.push(q);
}

Quadro* PilhaJVM::topo()
{
    return quadros.empty() ? nullptr : quadros.top();
}

bool PilhaJVM::desempilhar_quadro()
{
    if (quadros.empty()) {
        return false;
    }
    Quadro* q = quadros.top();
    quadros.pop();
    delete q;
    return true;
}

uint32_t PilhaJVM::tamanho()
{
    return static_cast<uint32_t>(quadros.size());
}
