/*
 * leitor_classe.hpp
 * Parser de arquivos .class no formato JVM 8 (secao 4.1 da spec).
 *
 * E' um singleton: o estado interno mais importante e' apenas o flag
 * de endianness da maquina, descoberto no construtor. A leitura em
 * si nao mantem estado entre chamadas - cada `ler_arquivo()` recebe
 * o FILE* e devolve um ArquivoClasse* completo, montado em ordem
 * estrita conforme a spec:
 *   magic, version, constant_pool, access_flags, this_class,
 *   super_class, interfaces, fields, methods, attributes.
 */
#ifndef LEITOR_CLASSE_HPP
#define LEITOR_CLASSE_HPP

#include "tipos_basicos.hpp"
#include "arquivo_classe.hpp"
#include <cstdio>

class LeitorClasse {
public:
    static LeitorClasse& instancia()
    {
        static LeitorClasse unico;
        return unico;
    }

    ArquivoClasse* ler_arquivo(FILE* fp);

    LeitorClasse(const LeitorClasse&)            = delete;
    LeitorClasse& operator=(const LeitorClasse&) = delete;

private:
    LeitorClasse();
    ~LeitorClasse() = default;

    /* ---- IO bruto (cuida do endianness Big-Endian da spec) ---- */
    u1 ler_u1(FILE* fp);
    u2 ler_u2(FILE* fp);
    u4 ler_u4(FILE* fp);

    /* ---- Cabecalho ---- */
    void ler_magic(FILE* fp, ArquivoClasse* arq);
    bool magic_valido(ArquivoClasse* arq);
    void ler_versoes(FILE* fp, ArquivoClasse* arq);
    bool versao_permitida(ArquivoClasse* arq, uint16_t major_maximo);

    /* ---- Constant pool ---- */
    void ler_pool_count(FILE* fp, ArquivoClasse* arq);
    void ler_pool_constantes(FILE* fp, ArquivoClasse* arq);

    ConstClassInfo              ler_const_class(FILE* fp);
    ConstFieldRefInfo           ler_const_fieldref(FILE* fp);
    ConstMethodRefInfo          ler_const_methodref(FILE* fp);
    ConstInterfaceMethodRefInfo ler_const_interfaceref(FILE* fp);
    ConstStrInfo                ler_const_str(FILE* fp);
    ConstIntInfo                ler_const_int(FILE* fp);
    ConstFloatInfo              ler_const_float(FILE* fp);
    ConstLongInfo               ler_const_long(FILE* fp);
    ConstDoubleInfo             ler_const_double(FILE* fp);
    ConstNameTypeInfo           ler_const_name_type(FILE* fp);
    ConstUtf8Info               ler_const_utf8(FILE* fp);

    /* ---- Estrutura principal ---- */
    void ler_flags_acesso(FILE* fp, ArquivoClasse* arq);
    void ler_this_class(FILE* fp, ArquivoClasse* arq);
    void ler_super_class(FILE* fp, ArquivoClasse* arq);

    void ler_qtd_interfaces(FILE* fp, ArquivoClasse* arq);
    void ler_interfaces(FILE* fp, ArquivoClasse* arq);

    void ler_qtd_campos(FILE* fp, ArquivoClasse* arq);
    void ler_campos(FILE* fp, ArquivoClasse* arq);

    void ler_qtd_metodos(FILE* fp, ArquivoClasse* arq);
    void ler_metodos(FILE* fp, ArquivoClasse* arq);

    void ler_qtd_atributos(FILE* fp, ArquivoClasse* arq);
    void ler_atributos(FILE* fp, ArquivoClasse* arq);

    /* ---- Atributos ---- */
    AttributeInfo               ler_atributo(FILE* fp, ArquivoClasse* arq);
    ConstValueAttribute         ler_attr_constval(FILE* fp);
    ExceptionTable              ler_exception_entry(FILE* fp);
    CodeAttribute               ler_attr_code(FILE* fp, ArquivoClasse* arq);
    ExceptionsAttribute         ler_attr_exceptions(FILE* fp);
    Class                       ler_inner_class_entry(FILE* fp);
    InnerClassesAttribute       ler_attr_inner(FILE* fp);
    SyntheticAttribute          ler_attr_synthetic();
    SourceFileAttribute         ler_attr_sourcefile(FILE* fp);
    LineNumberTable             ler_line_entry(FILE* fp);
    LineNumberTableAttribute    ler_attr_linetable(FILE* fp);
    LocalVariableTable          ler_localvar_entry(FILE* fp);
    LocalVariableTableAttribute ler_attr_localvar(FILE* fp);
    DeprecatedAttribute         ler_attr_deprecated();

    /* ---- Auxiliares ---- */
    ConstUtf8Info buscar_utf8(u2 index, ArquivoClasse* arq);
    bool          host_little_endian();

    bool maquina_little_endian;
};

#endif
