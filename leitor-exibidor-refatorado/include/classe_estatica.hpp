/*
 * classe_estatica.hpp
 * Equivalente em runtime ao "Class object" de Java.
 *
 * Mantem o ArquivoClasse carregado pelo leitor e a tabela de campos
 * estaticos (static fields) cuja inicializacao default e' feita pelo
 * construtor. A execucao do <clinit> em si fica a cargo do Executor.
 */
#ifndef CLASSE_ESTATICA_HPP
#define CLASSE_ESTATICA_HPP

#include "arquivo_classe.hpp"
#include <map>
#include <string>

class ClasseEstatica {
public:
    explicit ClasseEstatica(ArquivoClasse* cf);

    ArquivoClasse* obter_arquivo();

    /* manipulacao dos campos estaticos */
    void  definir_campo(Value v, const std::string& nome);
    Value obter_campo(std::string nome);
    bool  tem_campo(const std::string& nome);

    /* derivacao do nome simples (sem .class) a partir de argv */
    std::string inicializar_arquivo(char* argv[]);

private:
    ArquivoClasse*               class_file;
    std::map<std::string, Value> static_fields;
};

#endif
