/*
 * objeto_arranjo.cpp
 *
 * Implementacao de Arranjo. Portado de JVM/src/Array.cpp. Os retornos de erro
 * usam o helper faz_valor_int(-1) no lugar dos designated initializers da
 * referencia (que nao sao C++14 padrao).
 */
#include "objeto_arranjo.hpp"
#include <iostream>

Arranjo::Arranjo(TipoValor tipo)
    : array_type{tipo}
{
}

Arranjo::~Arranjo()
{
}

TipoObjeto Arranjo::tipo_objeto()
{
    return ARRANJO;
}

TipoValor Arranjo::array_content_type()
{
    return array_type;
}

void Arranjo::push_value(Valor value)
{
    array_elements.push_back(value);
}

Valor Arranjo::remove_at(uint32_t index)
{
    if (index >= array_elements.size()) {
        std::cerr << "Arranjo::remove_at - indice invalido\n";
        return faz_valor_int(-1);
    }
    Valor temp = array_elements[index];
    array_elements.erase(array_elements.begin() + index);
    return temp;
}

Valor Arranjo::remove_last()
{
    Valor temp = array_elements.back();
    array_elements.pop_back();
    return temp;
}

Valor Arranjo::remove_first()
{
    Valor temp = array_elements.front();
    array_elements.erase(array_elements.begin());
    return temp;
}

uint32_t Arranjo::get_size()
{
    return array_elements.size();
}

Valor Arranjo::get_value(uint32_t index)
{
    if (index >= array_elements.size()) {
        std::cerr << "Arranjo::get_value - indice invalido\n";
        return faz_valor_int(-1);
    }
    return array_elements[index];
}

void Arranjo::change_value(uint32_t index, Valor value)
{
    if (index >= array_elements.size()) {
        std::cerr << "Arranjo::change_value - indice invalido\n";
        return;
    }
    array_elements[index] = value;
}
