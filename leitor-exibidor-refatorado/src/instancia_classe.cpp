#include "instancia_classe.hpp"
#include "exibidor_classe.hpp"
#include <cstdlib>
#include <iostream>

namespace {
    constexpr u2 CLASS_ACC_ABSTRACT     = 0x0400;
    constexpr u2 FIELD_STATIC_OR_FINAL  = 0x0008 | 0x0010;

    /* Devolve a celula Value default p/ um campo de instancia a partir
     * do primeiro caracter do descritor (B,C,D,F,I,J,S,Z ou ref). */
    Value default_para_descritor(char tag)
    {
        Value v;
        switch (tag) {
            case 'B': v.print_type = BYTE;    v.type = BYTE;    v.data.byte_value   = 0; break;
            case 'C': v.print_type = CHAR;    v.type = CHAR;    v.data.char_value   = 0; break;
            case 'D': v.print_type = DOUBLE;  v.type = DOUBLE;  v.data.double_value = 0; break;
            case 'F': v.print_type = FLOAT;   v.type = FLOAT;   v.data.float_value  = 0; break;
            case 'I': v.print_type = INT;     v.type = INT;     v.data.int_value    = 0; break;
            case 'J': v.print_type = LONG;    v.type = LONG;    v.data.long_value   = 0; break;
            case 'S': v.print_type = SHORT;   v.type = SHORT;   v.data.short_value  = 0; break;
            case 'Z': v.print_type = BOOLEAN; v.type = BOOLEAN; v.data.char_value   = 0; break;
            default:
                v.print_type = REFERENCE;
                v.type       = REFERENCE;
                v.data.object = nullptr;
                break;
        }
        return v;
    }
}

InstanciaClasse::InstanciaClasse(ClasseEstatica* rt)
    : classe_runtime(rt)
{
    ArquivoClasse* cf = classe_runtime->obter_arquivo();

    /* Interface/abstrata nao pode ser instanciada via `new` */
    if ((cf->access_flags & CLASS_ACC_ABSTRACT) != 0) {
        std::cerr << "InstantiationError: tentativa de instanciar classe abstrata\n";
        std::exit(1);
    }

    /* Inicializa os campos de instancia (nao-static, nao-final) */
    for (u2 i = 0; i < cf->fields_count; ++i) {
        FieldInfo f = cf->fields[i];
        if ((f.access_flags & FIELD_STATIC_OR_FINAL) != 0) {
            continue;
        }
        std::string nome      = formatar_constante(cf->constant_pool, f.name_index);
        std::string descritor = formatar_constante(cf->constant_pool, f.descriptor_index);

        definir_campo(default_para_descritor(descritor[0]), nome);
    }
}

ObjectType InstanciaClasse::tipo_objeto()
{
    return CLASS_INSTANCE;
}

ClasseEstatica* InstanciaClasse::obter_runtime()
{
    return classe_runtime;
}

void InstanciaClasse::definir_campo(Value v, const std::string& nome)
{
    class_fields[nome] = v;
}

Value InstanciaClasse::obter_campo(const std::string& nome)
{
    auto it = class_fields.find(nome);
    if (it == class_fields.end()) {
        std::cerr << "NoSuchFieldError: campo de instancia '" << nome << "' nao encontrado\n";
        std::exit(1);
    }
    return it->second;
}

bool InstanciaClasse::tem_campo(const std::string& nome)
{
    return class_fields.find(nome) != class_fields.end();
}
