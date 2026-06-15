/*
 * objeto.hpp
 *
 * Classe-base abstrata de tudo o que vive no heap da JVM: instancias de
 * classe, strings e arranjos. Um ponteiro Objeto* e' o que uma REFERENCIA
 * (TipoValor::REFERENCIA) guarda dentro de um Valor.
 *
 * Portado de JVM/headers/Object.hpp (Object -> Objeto, object_type ->
 * tipo_objeto).
 */
#ifndef OBJETO_HPP
#define OBJETO_HPP

#include "tipos_runtime.hpp"

// Classe-base abstrata dos objetos do heap. Cada subclasse declara seu
// TipoObjeto concreto via tipo_objeto().
class Objeto {
public:
    virtual ~Objeto() {}

    // Discrimina o tipo concreto do objeto (instancia, string ou arranjo).
    virtual TipoObjeto tipo_objeto() = 0;
};

#endif // OBJETO_HPP
