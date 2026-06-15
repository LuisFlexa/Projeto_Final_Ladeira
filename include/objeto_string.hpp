/*
 * objeto_string.hpp
 *
 * Objeto que representa uma String da linguagem Java dentro da JVM. Herda de
 * Objeto e modela uma string imutavel, guardando o conteudo num std::string.
 *
 * Portado de JVM/headers/StrObject.hpp (StrObject -> ObjetoString).
 */
#ifndef OBJETO_STRING_HPP
#define OBJETO_STRING_HPP

#include "objeto.hpp"
#include <string>

class ObjetoString : public Objeto {
public:
    // Constroi a string com um conteudo inicial (vazio por padrao).
    ObjetoString(std::string s = "");
    ~ObjetoString();

    // Tipo do objeto: INSTANCIA_STRING.
    TipoObjeto tipo_objeto();

    // Le o conteudo atual da string.
    std::string get_str();

    // Define/atualiza o conteudo da string.
    void set_str(std::string s);

private:
    // Conteudo do objeto Java String.
    std::string str_interna;
};

#endif // OBJETO_STRING_HPP
