/*
 * area_metodos.hpp
 *
 * Area de metodos da JVM: singleton que CARREGA e faz CACHE de classes a
 * partir do disco, evitando reler o mesmo .class. Cada classe e' guardada
 * como uma ClasseEstatica, indexada pelo seu nome canonico (o valor de
 * this_class no pool de constantes) — e' por esse nome que o executor
 * procura classes ao resolver `new`, `getstatic`, `invoke*`, etc.
 *
 * Portado de JVM/headers/MethodArea.hpp + JVM/src/MethodArea.cpp
 * (MethodArea -> AreaMetodos, StaticClass -> ClasseEstatica,
 * ClassFile -> ArquivoClasse). Singleton no mesmo padrao de LeitorClasse.
 */
#ifndef AREA_METODOS_HPP
#define AREA_METODOS_HPP

#include "classe_estatica.hpp"
#include <map>
#include <string>

class AreaMetodos {
public:
    // Unica instancia (singleton).
    static AreaMetodos& instancia()
    {
        static AreaMetodos unica;
        return unica;
    }

    // Carrega a classe de nome 'nome' (do cache, se ja' carregada; senao
    // le do disco, cacheia e retorna). Devolve sempre um ponteiro estavel
    // para a ClasseEstatica.
    ClasseEstatica* carregar_classe(const std::string& nome);

    // Retorna a ClasseEstatica ja' carregada com esse nome, ou nullptr.
    ClasseEstatica* obter_classe(const std::string& nome);

    // Diretorio onde estao os .class (definido pelo main, passo 9).
    std::string class_path = "";

    AreaMetodos(const AreaMetodos&)            = delete;
    AreaMetodos& operator=(const AreaMetodos&) = delete;

private:
    AreaMetodos() = default;
    ~AreaMetodos() = default;

    // Insere uma classe ja' carregada, indexada pelo nome de this_class.
    // Retorna false se ja' havia uma classe com esse nome.
    bool inserir_classe(ClasseEstatica* classe_runtime);

    // Classes ja' carregadas, indexadas pelo nome canonico (this_class).
    std::map<std::string, ClasseEstatica*> classes;
};

#endif // AREA_METODOS_HPP
