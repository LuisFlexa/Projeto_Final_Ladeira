#include "objeto_str.hpp"

ObjetoStr::ObjetoStr(const std::string& s)
    : valor_str(s)
{
}

ObjectType ObjetoStr::tipo_objeto()
{
    return STRING_INSTANCE;
}

std::string ObjetoStr::obter_str() const
{
    return valor_str;
}

void ObjetoStr::definir_str(const std::string& s)
{
    valor_str = s;
}
