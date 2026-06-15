/*
 * objeto_string.cpp
 *
 * Implementacao de ObjetoString. Portado de JVM/src/StrObject.cpp.
 */
#include "objeto_string.hpp"

ObjetoString::ObjetoString(std::string s)
    : str_interna(s)
{
}

ObjetoString::~ObjetoString()
{
    // std::string se limpa sozinho; nada a fazer.
}

TipoObjeto ObjetoString::tipo_objeto()
{
    return INSTANCIA_STRING;
}

std::string ObjetoString::get_str()
{
    return str_interna;
}

void ObjetoString::set_str(std::string s)
{
    str_interna = s;
}
