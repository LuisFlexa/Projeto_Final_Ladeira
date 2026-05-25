/*
 * objeto_str.hpp
 * Representa a "String" do Java no heap simulado.
 * Internamente nao distinguimos do java.lang.String real: armazenamos
 * o conteudo como std::string e exibimos como UTF-8.
 */
#ifndef OBJETO_STR_HPP
#define OBJETO_STR_HPP

#include "objeto_base.hpp"
#include <string>

class ObjetoStr : public ObjetoBase {
public:
    explicit ObjetoStr(const std::string& s = std::string());
    ~ObjetoStr() override = default;

    ObjectType  tipo_objeto() override;
    std::string obter_str() const;
    void        definir_str(const std::string& s);

private:
    std::string valor_str;
};

#endif
