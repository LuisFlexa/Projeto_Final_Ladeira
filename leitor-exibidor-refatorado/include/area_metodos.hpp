/*
 * area_metodos.hpp
 * "Method Area" da nossa JVM (secao 2.5.4 da spec).
 *
 * Singleton que mantem o mapa <nome_qualificado_da_classe -> ClasseEstatica*>.
 * Quando o Executor precisa de uma classe que ainda nao foi carregada,
 * pede aqui via carregar_classe; o singleton tenta abrir o .class
 * correspondente no `caminho_classes` (definido pelo main).
 */
#ifndef AREA_METODOS_HPP
#define AREA_METODOS_HPP

#include "tipos_basicos.hpp"
#include "classe_estatica.hpp"
#include <map>
#include <string>

class AreaMetodos {
public:
    static AreaMetodos& instancia()
    {
        static AreaMetodos unica;
        return unica;
    }

    ClasseEstatica* carregar_classe(const std::string& nome);
    ClasseEstatica* obter_classe(const std::string& nome);

    /* diretorio onde procurar .class adicionais (definido pelo main) */
    std::string caminho_classes = "";

    AreaMetodos(const AreaMetodos&)            = delete;
    AreaMetodos& operator=(const AreaMetodos&) = delete;

private:
    AreaMetodos()  = default;
    ~AreaMetodos() = default;

    bool inserir_classe(ClasseEstatica* rt);

    std::map<std::string, ClasseEstatica*> classes;
};

#endif
