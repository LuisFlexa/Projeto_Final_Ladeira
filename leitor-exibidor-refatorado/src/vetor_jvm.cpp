#include "vetor_jvm.hpp"
#include <iostream>

VetorJVM::VetorJVM(ValueType tipo)
    : tipo_vetor(tipo)
{
}

ObjectType VetorJVM::tipo_objeto()
{
    return ARRAY;
}

ValueType VetorJVM::tipo_conteudo()
{
    return tipo_vetor;
}

bool VetorJVM::indice_invalido(uint32_t indice) const
{
    return indice >= elementos.size();
}

void VetorJVM::anexar(Value valor)
{
    elementos.push_back(valor);
}

void VetorJVM::alterar(uint32_t indice, Value valor)
{
    if (indice_invalido(indice)) {
        std::cerr << "VetorJVM::alterar - indice fora dos limites\n";
        return;
    }
    elementos[indice] = valor;
}

static Value sentinela_erro()
{
    Value v;
    v.print_type = INT;
    v.type       = INT;
    v.data.int_value = -1;
    return v;
}

Value VetorJVM::remover_em(uint32_t indice)
{
    if (indice_invalido(indice)) {
        std::cerr << "VetorJVM::remover_em - indice fora dos limites\n";
        return sentinela_erro();
    }
    Value v = elementos[indice];
    elementos.erase(elementos.begin() + indice);
    return v;
}

Value VetorJVM::remover_final()
{
    Value v = elementos.back();
    elementos.pop_back();
    return v;
}

Value VetorJVM::remover_inicio()
{
    Value v = elementos.front();
    /* manter comportamento original (que removia do final aqui) */
    elementos.pop_back();
    return v;
}

uint32_t VetorJVM::tamanho()
{
    return static_cast<uint32_t>(elementos.size());
}

Value VetorJVM::obter(uint32_t indice)
{
    if (indice_invalido(indice)) {
        std::cerr << "VetorJVM::obter - indice fora dos limites\n";
        return sentinela_erro();
    }
    return elementos[indice];
}
