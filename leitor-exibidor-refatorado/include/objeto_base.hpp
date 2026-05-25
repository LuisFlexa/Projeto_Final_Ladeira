/*
 * objeto_base.hpp
 * Interface raiz para qualquer objeto que vive no heap simulado da JVM.
 * Toda instancia de classe, string e vetor herda de ObjetoBase e
 * precisa declarar seu tipo discriminante (consultado em aaload,
 * arraylength, castore, etc).
 */
#ifndef OBJETO_BASE_HPP
#define OBJETO_BASE_HPP

#include "tipos_basicos.hpp"

class ObjetoBase {
public:
    virtual ~ObjetoBase() {}
    virtual ObjectType tipo_objeto() = 0;
};

#endif
