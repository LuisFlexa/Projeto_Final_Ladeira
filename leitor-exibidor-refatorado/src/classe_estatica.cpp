#include "classe_estatica.hpp"
#include "exibidor_classe.hpp"
#include <cstdlib>
#include <iostream>

namespace {
    /* Mesma logica que InstanciaClasse usa, mas duplicada aqui pra
     * manter a unidade de compilacao auto-suficiente. */
    Value default_para_descritor_estatico(char tag)
    {
        Value v;
        v.print_type = REFERENCE;
        v.type       = REFERENCE;
        v.data.object = nullptr;

        switch (tag) {
            case 'B': v.print_type = BYTE;    v.type = BYTE;    v.data.byte_value   = 0; break;
            case 'C': v.print_type = CHAR;    v.type = CHAR;    v.data.char_value   = 0; break;
            case 'D': v.print_type = DOUBLE;  v.type = DOUBLE;  v.data.double_value = 0; break;
            case 'F': v.print_type = FLOAT;   v.type = FLOAT;   v.data.float_value  = 0; break;
            case 'I': v.print_type = INT;     v.type = INT;     v.data.int_value    = 0; break;
            case 'J': v.print_type = LONG;    v.type = LONG;    v.data.long_value   = 0; break;
            case 'S': v.print_type = SHORT;   v.type = SHORT;   v.data.short_value  = 0; break;
            case 'Z': v.print_type = BOOLEAN; v.type = BOOLEAN; v.data.char_value   = 0; break;
            default: break; /* referencia, ja' default */
        }
        return v;
    }
}

ClasseEstatica::ClasseEstatica(ArquivoClasse* cf)
    : class_file(cf)
{
    /* Inicializa cada campo static com seu default-zero */
    for (u2 i = 0; i < cf->fields_count; ++i) {
        FieldInfo f = cf->fields[i];
        if ((f.access_flags & FIELD_FLAG_ACC_STATIC) == 0) {
            continue;
        }
        std::string nome      = formatar_constante(cf->constant_pool, f.name_index);
        std::string descritor = formatar_constante(cf->constant_pool, f.descriptor_index);
        definir_campo(default_para_descritor_estatico(descritor[0]), nome);
    }
}

ArquivoClasse* ClasseEstatica::obter_arquivo()
{
    return class_file;
}

void ClasseEstatica::definir_campo(Value v, const std::string& nome)
{
    static_fields[nome] = v;
}

Value ClasseEstatica::obter_campo(std::string nome)
{
    auto it = static_fields.find(nome);
    if (it == static_fields.end()) {
        std::cerr << "NoSuchFieldError: campo estatico '" << nome << "' nao encontrado\n";
        std::exit(1);
    }
    return it->second;
}

bool ClasseEstatica::tem_campo(const std::string& nome)
{
    return static_fields.find(nome) != static_fields.end();
}

std::string ClasseEstatica::inicializar_arquivo(char* argv[])
{
    /* argv[2] foi o segundo argumento posicional na CLI original;
     * remove o sufixo ".class" para devolver so' o nome base. */
    const std::string ext = ".class";
    std::string s(argv[2]);
    if (s.size() >= ext.size() &&
        s.compare(s.size() - ext.size(), ext.size(), ext) == 0) {
        s.resize(s.size() - ext.size());
    }
    return s;
}
