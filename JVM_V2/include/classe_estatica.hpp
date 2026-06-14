/*
 * classe_estatica.hpp
 *
 * Representa uma classe CARREGADA em tempo de execucao: guarda o
 * ArquivoClasse com os dados do .class e um mapa com os valores dos
 * campos ESTATICOS (ACC_STATIC) da classe. No construtor os estaticos
 * sao apenas zerados conforme seu descritor; a execucao do <clinit>
 * (bloco de inicializacao estatica) ainda nao acontece aqui.
 *
 * Portado de JVM/headers/StaticClass.hpp + JVM/src/StaticClass.cpp
 * (StaticClass -> ClasseEstatica, ClassFile -> ArquivoClasse,
 * Value -> Valor).
 */
#ifndef CLASSE_ESTATICA_HPP
#define CLASSE_ESTATICA_HPP

#include "arquivo_classe.hpp"
#include "tipos_runtime.hpp"
#include <map>
#include <string>

class ClasseEstatica {
public:
    // Recebe o ponteiro para o ArquivoClasse com os dados da classe e
    // inicializa (zera) os campos estaticos conforme seus descritores.
    ClasseEstatica(ArquivoClasse* arquivo_classe);

    // Retorna o ArquivoClasse passado ao construtor.
    ArquivoClasse* get_arquivo_classe();

    // Altera/insere o valor de um campo estatico.
    void insert_value(const Valor& valor, const std::string& nome_campo);

    // Acessa o valor de um campo estatico (erro se nao existir).
    Valor get_value(const std::string& nome_campo);

    // Verifica se um campo estatico existe.
    bool check_field(const std::string& nome_campo);

private:
    // ArquivoClasse a ser acessado.
    ArquivoClasse* arquivo_classe;

    // Nomes e valores dos campos estaticos.
    std::map<std::string, Valor> campos_estaticos;
};

#endif // CLASSE_ESTATICA_HPP
