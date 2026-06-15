/*
 * util_classe.hpp
 * Funcoes auxiliares de uso geral (todas estaticas).
 * Mantidas em uma classe-fachada apenas para namespacear.
 */
#ifndef UTIL_CLASSE_HPP
#define UTIL_CLASSE_HPP

#include "tipos_basicos.hpp"
#include "arquivo_classe.hpp"
#include <cstdio>
#include <sstream>
#include <string>

class UtilClasse {
public:
    /* Conversao da major_version do .class para o "marketing" do Java */
    static double versao_legivel(ArquivoClasse* arq);
    static double versao_para_msg_erro(ArquivoClasse* arq);

    /* Compara um UTF-8 da pool com uma C-string */
    static bool utf8_igual_a(ConstUtf8Info entry, const char* alvo);

    /* Indentacao com tabs */
    static void indentar_arquivo(FILE* out, uint8_t n);
    static void indentar_tela(uint8_t n);

    /* Move o conteudo de um stringstream para uma C-string alocada */
    static const char* ss_para_cstr(const std::stringstream& ss);

    /* Confere se o nome da classe (no constant pool) bate com o nome do arquivo */
    static bool conferir_nome_classe(std::string caminho_arq, std::string nome_classe);
};

#endif
