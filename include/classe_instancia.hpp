/*
 * classe_instancia.hpp
 *
 * Representa uma INSTANCIA de objeto no heap: aponta para a ClasseEstatica
 * que a define e guarda um mapa com os valores dos seus campos de INSTANCIA
 * (campos que nao sao static nem final). E' um Objeto, logo um ponteiro para
 * ClasseInstancia pode ser guardado numa REFERENCIA dentro de um Valor.
 *
 * Portado de JVM/headers/InstanceClass.hpp + JVM/src/InstanceClass.cpp
 * (InstanceClass -> ClasseInstancia, StaticClass -> ClasseEstatica,
 * Object -> Objeto, Value -> Valor).
 */
#ifndef CLASSE_INSTANCIA_HPP
#define CLASSE_INSTANCIA_HPP

#include "classe_estatica.hpp"
#include "objeto.hpp"
#include "tipos_runtime.hpp"
#include <map>
#include <string>

class ClasseInstancia : public Objeto {
public:
    // Cria a instancia a partir da sua ClasseEstatica e inicializa (zera)
    // os campos de instancia conforme seus descritores.
    ClasseInstancia(ClasseEstatica* classe_runtime);

    ~ClasseInstancia();

    // Tipo do objeto: INSTANCIA_CLASSE.
    TipoObjeto tipo_objeto();

    // Retorna a ClasseEstatica associada a esta instancia.
    ClasseEstatica* get_classe_runtime();

    // Insere/atualiza o valor de um campo de instancia.
    void insert_value_into_field(Valor valor, const std::string& nome_campo);

    // Retorna o valor de um campo de instancia (erro se nao existir).
    Valor get_value_from_field(const std::string& nome_campo);

    // Verifica se o campo de instancia existe.
    bool field_exists(const std::string& nome_campo);

private:
    // Ponteiro para a definicao da classe (ClasseEstatica).
    ClasseEstatica* classe_runtime;

    // Armazenamento dos campos de instancia.
    std::map<std::string, Valor> campos_instancia;
};

#endif // CLASSE_INSTANCIA_HPP
