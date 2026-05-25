/*
 * instancia_classe.hpp
 * Representa um objeto (resultado de `new`) no heap simulado.
 *
 * Guarda uma referencia para a ClasseEstatica (metadata) e um mapa
 * dos seus campos de instancia, inicializados com o valor default
 * apropriado para o tipo declarado.
 */
#ifndef INSTANCIA_CLASSE_HPP
#define INSTANCIA_CLASSE_HPP

#include "tipos_basicos.hpp"
#include "objeto_base.hpp"
#include "classe_estatica.hpp"
#include <map>
#include <string>

class InstanciaClasse : public ObjetoBase {
public:
    explicit InstanciaClasse(ClasseEstatica* rt);
    ~InstanciaClasse() override = default;

    ObjectType tipo_objeto() override;

    ClasseEstatica* obter_runtime();

    /* manipulacao de campos de instancia */
    void  definir_campo(Value v, const std::string& nome);
    Value obter_campo(const std::string& nome);
    bool  tem_campo(const std::string& nome);

private:
    ClasseEstatica*               classe_runtime;
    std::map<std::string, Value>  class_fields;
};

#endif
