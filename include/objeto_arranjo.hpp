/*
 * objeto_arranjo.hpp
 *
 * Objeto que representa um array (arranjo) da JVM. Herda de Objeto e guarda
 * os elementos num std::vector<Valor>, junto do tipo dos elementos
 * (necessario para newarray/anewarray no passo 8).
 *
 * Portado de JVM/headers/Array.hpp (Array -> Arranjo).
 */
#ifndef OBJETO_ARRANJO_HPP
#define OBJETO_ARRANJO_HPP

#include "objeto.hpp"
#include <cstdint>
#include <vector>

class Arranjo : public Objeto {
public:
    // Constroi um arranjo cujos elementos sao do tipo informado.
    Arranjo(TipoValor tipo);
    ~Arranjo();

    // Tipo do objeto: ARRANJO.
    TipoObjeto tipo_objeto();

    // Tipo dos valores armazenados no arranjo.
    TipoValor array_content_type();

    // Insere um valor no final.
    void push_value(Valor value);

    // Remove e retorna o valor na posicao dada.
    Valor remove_at(uint32_t index);

    // Remove e retorna o ultimo valor.
    Valor remove_last();

    // Remove e retorna o primeiro valor.
    Valor remove_first();

    // Quantidade de elementos.
    uint32_t get_size();

    // Le o valor na posicao dada.
    Valor get_value(uint32_t index);

    // Altera o valor na posicao dada.
    void change_value(uint32_t index, Valor value);

private:
    TipoValor array_type;                 // tipo dos elementos
    std::vector<Valor> array_elements;    // elementos do arranjo
};

#endif // OBJETO_ARRANJO_HPP
