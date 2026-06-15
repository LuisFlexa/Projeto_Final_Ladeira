/*
 * classe_instancia.cpp
 *
 * Implementacao de ClasseInstancia. Portado de JVM/src/InstanceClass.cpp.
 */
#include "classe_instancia.hpp"
#include "exibidor_classe.hpp"  // formatar_constante
#include <cstdlib>
#include <iostream>

using std::string;

// Flag de acesso de CLASSE para "abstrata" (spec, tabela 4.1-B). Nao ha'
// enum de flags de classe em tipos_basicos.hpp (so' de campo/metodo), por
// isso a constante e' local aqui.
static const u2 CLASS_FLAG_ACC_ABSTRACT = 0x0400;

// Zera um Valor conforme o primeiro caractere do descritor do campo
// (B C D F I J S Z -> primitivos; qualquer outro -> referencia nula).
static Valor valor_zerado_para_descritor(char tipo_descritor)
{
    switch (tipo_descritor) {
    case 'B': return faz_valor_byte(0);     // byte
    case 'C': return faz_valor_char(0);     // char
    case 'D': return faz_valor_double(0);   // double
    case 'F': return faz_valor_float(0);    // float
    case 'I': return faz_valor_int(0);      // int
    case 'J': return faz_valor_long(0);     // long
    case 'S': return faz_valor_short(0);    // short
    case 'Z': return faz_valor_booleano(false); // boolean
    default:  return faz_valor_referencia(0); // referencia (classe ou array)
    }
}

// Construtor: inicializa os campos de instancia com valores padrao.
ClasseInstancia::ClasseInstancia(ClasseEstatica* classe_runtime)
    : classe_runtime(classe_runtime)
{
    ArquivoClasse* arquivo_classe = classe_runtime->get_arquivo_classe();
    FieldInfo* fields = arquivo_classe->fields;

    // Classe abstrata nao pode ser instanciada.
    if ((arquivo_classe->access_flags & CLASS_FLAG_ACC_ABSTRACT) != 0) {
        std::cerr << "InstantiationError" << std::endl;
        exit(1);
    }

    // Considera apenas campos de instancia (nem static, nem final).
    const u2 mascara_static_final = FIELD_FLAG_ACC_STATIC | FIELD_FLAG_ACC_FINAL;

    for (int i = 0; i < arquivo_classe->fields_count; i++) {
        FieldInfo field = fields[i];

        if ((field.access_flags & mascara_static_final) == 0) {
            string nome_campo =
                formatar_constante(arquivo_classe->constant_pool, field.name_index);
            string descritor_campo =
                formatar_constante(arquivo_classe->constant_pool, field.descriptor_index);

            Valor valor = valor_zerado_para_descritor(descritor_campo[0]);
            insert_value_into_field(valor, nome_campo);
        }
    }
}

// Destrutor (vazio).
ClasseInstancia::~ClasseInstancia()
{
}

// Tipo do objeto: instancia de classe.
TipoObjeto ClasseInstancia::tipo_objeto()
{
    return INSTANCIA_CLASSE;
}

// Retorna a ClasseEstatica que define esta instancia.
ClasseEstatica* ClasseInstancia::get_classe_runtime()
{
    return classe_runtime;
}

// Insere/atualiza o valor de um campo de instancia.
void ClasseInstancia::insert_value_into_field(Valor valor, const string& nome_campo)
{
    campos_instancia[nome_campo] = valor;
}

// Retorna o valor de um campo de instancia; erro se nao existir.
Valor ClasseInstancia::get_value_from_field(const string& nome_campo)
{
    if (campos_instancia.count(nome_campo) == 0) {
        std::cerr << "NoSuchFieldError" << std::endl;
        exit(1);
    }

    return campos_instancia[nome_campo];
}

// Verifica se o campo de instancia existe.
bool ClasseInstancia::field_exists(const string& nome_campo)
{
    return campos_instancia.count(nome_campo) > 0;
}
