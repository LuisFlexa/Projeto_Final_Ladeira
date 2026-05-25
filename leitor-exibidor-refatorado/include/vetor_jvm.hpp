/*
 * vetor_jvm.hpp
 * Implementacao do array Java no heap simulado.
 *
 * Importante: para multiarrays (int[][], String[][][], ...) cada
 * dimensao e' um VetorJVM cujo `tipo_vetor` e' REFERENCE e cujos
 * elementos sao Value{REFERENCE, ObjetoBase*} apontando para outros
 * VetorJVM. So' a dimensao mais interna carrega o tipo primitivo.
 */
#ifndef VETOR_JVM_HPP
#define VETOR_JVM_HPP

#include "tipos_basicos.hpp"
#include "objeto_base.hpp"
#include <vector>

class VetorJVM : public ObjetoBase {
public:
    explicit VetorJVM(ValueType tipo);
    ~VetorJVM() override = default;

    ObjectType tipo_objeto() override;
    ValueType  tipo_conteudo();

    /* mutacoes */
    void  anexar(Value valor);
    void  alterar(uint32_t indice, Value valor);
    Value remover_em(uint32_t indice);
    Value remover_final();
    Value remover_inicio();

    /* consultas */
    uint32_t tamanho();
    Value    obter(uint32_t indice);

private:
    bool indice_invalido(uint32_t indice) const;

    ValueType            tipo_vetor;
    std::vector<Value>   elementos;
};

#endif
