/*
 * exibidor_classe.cpp
 * ------------------------------------------------------------------
 * Producao da saida textual estilo jclasslib a partir de um
 * ArquivoClasse* (estrutura preenchida por LeitorClasse).
 *
 * A saida espelha o que o utilitario jclasslib produz para um
 * .class: secoes "General Information", "Constant Pool",
 * "Interfaces", "Fields", "Methods" e "Attributes", com a
 * desmontagem do bytecode dentro de cada atributo Code.
 *
 * O formato exato (espacos, tabs, ordem dos campos, abreviacoes)
 * foi preservado igual ao do leitor original (leitor-exibidor-v2)
 * porque a validacao do trabalho compara byte-a-byte com a saida
 * baseline em saidas/. Qualquer alteracao cosmetica quebraria a
 * comparacao.
 *
 * Estrategia de impressao:
 *  - Um FILE* global (saida_global) e' setado por escrever_arquivo()
 *    e usado por todas as funcoes escrever_* / auxiliares. Evita
 *    passar o FILE* a cada chamada interna.
 *  - O dispatch principal e' a cascata de if/else if em
 *    escrever_atributo(), que se chama recursivamente para os
 *    sub-atributos do Code.
 *  - tabela_mnemonicos[] (declarada em exibidor_classe.hpp) mapeia
 *    cada opcode 0x00..0xff para o nome textual da instrucao
 *    (ex.: 0x60 -> "iadd").
 */
#include "exibidor_classe.hpp"
#include "util_classe.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

namespace {
    /* FILE* global usado por todas as funcoes escrever_*. Setado em
     * escrever_arquivo() antes da primeira escrita. Globais sao
     * normalmente evitadas, mas aqui simplificam muito as chamadas
     * recursivas de escrever_atributo() (que de outro modo teria
     * que propagar o FILE* por toda a cadeia). */
    FILE* saida_global = nullptr;

    /* Tabelas paralelas: mascara em MASCARAS_FLAG[i] corresponde ao
     * nome em NOMES_FLAG[i]. A ordem (public, final, super, ...) e' a
     * mesma usada pelo jclasslib na saida textual - mudar a ordem
     * quebraria a comparacao com a baseline. */
    const char* const NOMES_FLAG[] = {
        "public ", "final ", "super ", "interface ", "abstract ",
        "private ", "protected ", "static ", "volatile ", "transient "
    };
    const uint16_t MASCARAS_FLAG[] = {
        0x0001, 0x0010, 0x0020, 0x0200, 0x0400,
        0x0002, 0x0004, 0x0008, 0x0040, 0x0080
    };
    constexpr size_t QTD_FLAGS = sizeof(MASCARAS_FLAG) / sizeof(MASCARAS_FLAG[0]);
}

/* ----------------------------------------------------------------------- */
/* Saida principal                                                         */
/*                                                                         */
/* Ponto de entrada do exibidor. Recebe o ArquivoClasse* ja lido pelo      */
/* LeitorClasse e o FILE* de saida (geralmente stdout ou um .txt).         */
/* Imprime as seis secoes em ordem fixa, cada uma com cabecalho e          */
/* delimitadores { ... } para combinar com a baseline do jclasslib.        */
/* ----------------------------------------------------------------------- */
void escrever_arquivo(ArquivoClasse* arq, FILE* saida)
{
    saida_global = saida;

    std::fprintf(saida_global, "General Information\n{\n");
    escrever_info_geral(arq);
    std::fprintf(saida_global, "}\n\n");

    std::fprintf(saida_global, "Constant Pool (Member count: %d)\n{\n", arq->constant_pool_count);
    escrever_pool_constantes(arq);
    std::fprintf(saida_global, "}\n\n");

    std::fprintf(saida_global, "Interfaces (Member count: %d) \n{\n", arq->interfaces_count);
    escrever_interfaces(arq);
    std::fprintf(saida_global, "}\n\n");

    std::fprintf(saida_global, "Fields (Member count: %d)\n{\n", arq->fields_count);
    escrever_campos(arq);
    std::fprintf(saida_global, "}\n\n");

    std::fprintf(saida_global, "Methods (Member count: %d)\n{\n", arq->methods_count);
    escrever_metodos(arq);
    std::fprintf(saida_global, "}\n\n");

    std::fprintf(saida_global, "Attributes (Member count: %d)\n{\n", arq->attributes_count);
    escrever_atributos(arq);
    std::fprintf(saida_global, "}\n\n");
}

/* ----------------------------------------------------------------------- */
/* Secao "General Information"                                             */
/*                                                                         */
/* Imprime metadata do cabecalho: versao, contagens, this/super class.     */
/* O texto "[X.Y]" ao lado da major version e' a versao legivel do Java    */
/* (52 -> 1.8). Quando super_class == 0 a classe e' java/lang/Object.      */
/* ----------------------------------------------------------------------- */
void escrever_info_geral(ArquivoClasse* arq)
{
    FILE* o = saida_global;
    std::fprintf(o, "\t Minor Version: \t\t %hu\n", arq->minor_version);
    std::fprintf(o, "\t Major Version: \t\t %hu [%.1f]\n",
                 arq->major_version, UtilClasse::versao_legivel(arq));
    std::fprintf(o, "\t Constant Pool count: \t\t %hu\n", arq->constant_pool_count);
    std::fprintf(o, "\t Access Flags: \t\t\t 0x%.4X [%s]\n",
                 arq->access_flags, flags_acesso_str(arq->access_flags));
    std::fprintf(o, "\t This Class: \t\t\t ConstantPoolInfo #%hu <%s>\n",
                 arq->this_class,
                 formatar_constante(arq->constant_pool, arq->this_class));
    if (arq->super_class == 0) {
        std::fprintf(o, "\t Super class: \t\t\t none\n");
    } else {
        std::fprintf(o, "\t Super class: \t\t\t ConstantPoolInfo #%hu <%s>\n",
                     arq->super_class,
                     formatar_constante(arq->constant_pool, arq->super_class));
    }
    std::fprintf(o, "\t Interfaces count: \t\t %hu\n", arq->interfaces_count);
    std::fprintf(o, "\t Fields count: \t\t\t %hu\n", arq->fields_count);
    std::fprintf(o, "\t Methods pool count: \t\t %hu\n", arq->methods_count);
    std::fprintf(o, "\t Attributes pool count: \t %hu\n", arq->attributes_count);
}

/* ----------------------------------------------------------------------- */
/* Secao "Constant Pool"                                                   */
/*                                                                         */
/* Itera todas as entradas reais (count - 1) e imprime conforme a tag.     */
/* Long e Double ocupam dois slots: o slot seguinte (ConstNull) gera a     */
/* linha "(large numeric continued)". O indice impresso e' 1-based.        */
/* ----------------------------------------------------------------------- */
void escrever_pool_constantes(ArquivoClasse* arq)
{
    FILE* o = saida_global;
    ConstantPoolInfo* pool = arq->constant_pool;

    for (int i = 0; i < arq->constant_pool_count - 1; ++i) {
        ConstantPoolInfo e = pool[i];
        int idx = i + 1;

        switch (e.tag) {
            case ConstClass: {
                ConstClassInfo ci = e.info.class_info;
                std::fprintf(o, "\t [%d] ConstClassInfo\n", idx);
                std::fprintf(o, "\t\t Class name: \t\t\t ConstantPoolInfo #%hu <%s>\n",
                             ci.name_index,
                             formatar_constante(pool, ci.name_index));
                break;
            }
            case ConstFieldRef: {
                ConstFieldRefInfo fr = e.info.field_ref_info;
                std::fprintf(o, "\t [%d] ConstFieldRefInfo\n", idx);
                std::fprintf(o, "\t\t Class name: \t\t\t ConstantPoolInfo #%hu <%s>\n ",
                             fr.class_index, formatar_constante(pool, fr.class_index));
                std::fprintf(o, "\t\t Name and type: \t\t ConstantPoolInfo #%hu <%s>\n",
                             fr.name_and_type_index, formatar_constante(pool, fr.name_and_type_index));
                break;
            }
            case ConstMethodRef: {
                ConstMethodRefInfo mr = e.info.method_ref_info;
                std::fprintf(o, "\t [%d] ConstMethodRefInfo\n", idx);
                std::fprintf(o, "\t\t Class name: \t\t\t ConstantPoolInfo #%hu <%s>\n ",
                             mr.class_index, formatar_constante(pool, mr.class_index));
                std::fprintf(o, "\t\t Name and type: \t\t ConstantPoolInfo #%hu <%s>\n",
                             mr.name_and_type_index, formatar_constante(pool, mr.name_and_type_index));
                break;
            }
            case ConstInterfaceMethodRef: {
                ConstInterfaceMethodRefInfo ir = e.info.interface_method_ref_info;
                std::fprintf(o, "\t [%d] ConstInterfaceMethodRefInfo\n", idx);
                std::fprintf(o, "\t\t Class name: \t\t\t ConstantPoolInfo #%hu <%s>\n ",
                             ir.class_index, formatar_constante(pool, ir.class_index));
                std::fprintf(o, "\t\t Name and type: \t\t ConstantPoolInfo #%hu <%s>\n",
                             ir.name_and_type_index, formatar_constante(pool, ir.name_and_type_index));
                break;
            }
            case ConstStr: {
                ConstStrInfo s = e.info.str_info;
                std::fprintf(o, "\t [%d] ConstStrInfo\n", idx);
                std::fprintf(o, "\t String: \t\t\t\t ConstantPoolInfo #%hu <%s>\n",
                             s.string_index, formatar_constante(pool, s.string_index));
                break;
            }
            case ConstInt: {
                ConstIntInfo ii = e.info.int_info;
                std::fprintf(o, "\t [%d] ConstIntInfo\n", idx);
                std::fprintf(o, "\t Bytes: \t\t\t\t 0x%.8X\n", ii.bytes);
                std::fprintf(o, "\t Integer: \t\t\t\t %s\n", formatar_constante(pool, idx));
                break;
            }
            case ConstFloat: {
                ConstFloatInfo fi = e.info.float_info;
                std::fprintf(o, "\t [%d] ConstFloatInfo\n", idx);
                std::fprintf(o, "\t\t Bytes: \t\t\t 0x%.8X\n", fi.bytes);
                std::fprintf(o, "\t\t Float: \t\t\t %s\n", formatar_constante(pool, idx));
                break;
            }
            case ConstLong: {
                ConstLongInfo li = e.info.long_info;
                std::fprintf(o, "\t [%d] ConstLongInfo\n", idx);
                std::fprintf(o, "\t\t High Bytes: \t\t\t 0x%.8X\n", li.high_bytes);
                std::fprintf(o, "\t\t Low Bytes: \t\t\t 0x%.8X\n", li.low_bytes);
                std::fprintf(o, "\t\t Long: \t\t\t\t %s\n", formatar_constante(pool, idx));
                break;
            }
            case ConstDouble: {
                ConstDoubleInfo di = e.info.double_info;
                std::fprintf(o, "\t [%d] ConstDoubleInfo\n", idx);
                std::fprintf(o, "\t\t High Bytes: \t\t\t 0x%.8X\n", di.high_bytes);
                std::fprintf(o, "\t\t Low Bytes: \t\t\t 0x%.8X\n", di.low_bytes);
                std::fprintf(o, "\t\t Double: \t\t\t %s\n", formatar_constante(pool, idx));
                break;
            }
            case ConstNameType: {
                ConstNameTypeInfo nt = e.info.name_type_info;
                std::fprintf(o, "\t [%d] ConstNameTypeInfo\n", idx);
                std::fprintf(o, "\t\t Name: \t\t\t\t ConstantPoolInfo #%hu <%s>\n",
                             nt.name_index, formatar_constante(pool, nt.name_index));
                std::fprintf(o, "\t\t Descriptor: \t\t\t ConstantPoolInfo #%hu <%s>\n",
                             nt.descriptor_index, formatar_constante(pool, nt.descriptor_index));
                break;
            }
            case ConstUtf8: {
                ConstUtf8Info u8 = e.info.utf8_info;
                const char* s = formatar_constante(pool, idx);
                std::fprintf(o, "\t [%d] ConstUtf8Info\n", idx);
                std::fprintf(o, "\t\t Length of byte array: \t\t %hu\n", u8.length);
                std::fprintf(o, "\t\t Length of string: \t\t %llu\n",
                             (unsigned long long)std::strlen(s));
                std::fprintf(o, "\t\t String: \t\t\t %s\n", s);
                break;
            }
            case ConstNull: {
                std::fprintf(o, "\t [%d] (large numeric continued)\n", idx);
                break;
            }
            default:
                std::cerr << "The .class file has an invalid tag in its constant pool.\n";
                std::exit(5);
        }
        std::fprintf(o, "\n");
    }
}

/* ----------------------------------------------------------------------- */
/* Secao "Interfaces"                                                      */
/* ----------------------------------------------------------------------- */
void escrever_interfaces(ArquivoClasse* arq)
{
    FILE* o = saida_global;
    for (int i = 0; i < arq->interfaces_count; ++i) {
        const char* nome = formatar_constante(arq->constant_pool, arq->interfaces[i]);
        std::fprintf(o, "\t Interface %d \n", i);
        std::fprintf(o, "\t\t Interface: \t\t ConstantPoolInfo #%hu <%s>\n ",
                     arq->interfaces[i], nome);
    }
}

/* ----------------------------------------------------------------------- */
/* Secao "Fields"                                                          */
/* ----------------------------------------------------------------------- */
void escrever_campos(ArquivoClasse* arq)
{
    FILE* o = saida_global;
    for (u2 i = 0; i < arq->fields_count; ++i) {
        FieldInfo f = arq->fields[i];
        std::fprintf(o, "\t[%hu] %s\n", i,
                     formatar_constante(arq->constant_pool, f.name_index));
        std::fprintf(o, "\t{\n");
        std::fprintf(o, "\t\tName: \t\t\t ConstantPoolInfo #%hu <%s>\n ",
                     f.name_index, formatar_constante(arq->constant_pool, f.name_index));
        std::fprintf(o, "\t\tDescriptor: \t ConstantPoolInfo #%hu <%s>\n ",
                     f.descriptor_index,
                     formatar_constante(arq->constant_pool, f.descriptor_index));
        std::fprintf(o, "\t\tAccess flags: \t %x [%s]\n",
                     f.access_flags, flags_acesso_str(f.access_flags));
        std::fprintf(o, "\t\tAttributes:\n");
        for (u2 j = 0; j < f.attributes_count; ++j) {
            escrever_atributo(f.attributes[j], j, arq->constant_pool, 3);
        }
        std::fprintf(o, "\t}\n");
    }
}

/* ----------------------------------------------------------------------- */
/* Secao "Methods"                                                         */
/* ----------------------------------------------------------------------- */
void escrever_metodos(ArquivoClasse* arq)
{
    FILE* o = saida_global;
    for (int i = 0; i < arq->methods_count; ++i) {
        MethodInfo m = arq->methods[i];
        const char* nome = formatar_constante(arq->constant_pool, m.name_index);
        const char* desc = formatar_constante(arq->constant_pool, m.descriptor_index);
        const char* fl   = flags_acesso_str(m.access_flags);
        std::fprintf(o, "\t[%d] %s\n", i, nome);
        std::fprintf(o, "\t{\n");
        std::fprintf(o, "\t\tName: \t\t ConstantPoolInfo #%d <%s>\n", m.name_index, nome);
        std::fprintf(o, "\t\tDescriptor: \t ConstantPoolInfo #%d <%s>\n", m.descriptor_index, desc);
        std::fprintf(o, "\t\tAccess flags: \t 0x%.4X [%s]\n", m.access_flags, fl);
        std::fprintf(o, "\t\tAttributes:\n");
        for (int j = 0; j < m.attributes_count; ++j) {
            escrever_atributo(m.attributes[j], j, arq->constant_pool, 3);
        }
        std::fprintf(o, "\t}\n");
    }
}

/* ----------------------------------------------------------------------- */
/* Secao "Attributes" (top-level)                                          */
/* ----------------------------------------------------------------------- */
void escrever_atributos(ArquivoClasse* arq)
{
    for (uint16_t i = 0; i < arq->attributes_count; ++i) {
        escrever_atributo(arq->attributes[i], i, arq->constant_pool, 1);
    }
}

/* ----------------------------------------------------------------------- */
/* Detalhe de UM atributo (usado recursivamente p/ Code->attributes)       */
/*                                                                         */
/* Despacha pelo NOME (Utf8 no pool) e formata o conteudo especifico.      */
/* O parametro `nivel` controla a indentacao com tabs - aumenta a cada     */
/* recursao para atributos aninhados dentro de Code.                       */
/* ----------------------------------------------------------------------- */
void escrever_atributo(AttributeInfo a, uint32_t idx, ConstantPoolInfo* pool, uint8_t nivel)
{
    FILE* o = saida_global;
    const char* nome_attr = formatar_constante(pool, a.attribute_name_index);

    UtilClasse::indentar_arquivo(o, nivel);
    std::fprintf(o, "[%d] %s\n", idx, nome_attr);
    UtilClasse::indentar_arquivo(o, nivel + 1);
    std::fprintf(o, "Attribute name index:\t ConstantPoolInfo #%d\n", a.attribute_name_index);
    UtilClasse::indentar_arquivo(o, nivel + 1);
    std::fprintf(o, "Attribute length:\t %d\n", a.attribute_length);

    ConstUtf8Info utf = pool[a.attribute_name_index - 1].info.utf8_info;

    if (UtilClasse::utf8_igual_a(utf, "ConstantValue")) {
        ConstValueAttribute cv = a.info.const_value_info;
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Constant value index:\t ConstantPoolInfo #%d <%s>\n",
                     cv.const_value_index,
                     formatar_constante(pool, cv.const_value_index));
    }
    else if (UtilClasse::utf8_igual_a(utf, "Code")) {
        CodeAttribute cd = a.info.code_info;
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Maximum stack depth:\t %d\n", cd.max_stack);
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Maximum local variables: %d\n", cd.max_locals);
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Code length:\t\t %u\n", cd.code_length);
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Exception table:\n");
        UtilClasse::indentar_arquivo(o, nivel + 2);
        if (cd.exception_table_length > 0) {
            std::fprintf(o, "Nr.\t start_pc\t end_pc\t handler_pc\t catch_type\t verbose\n");
            for (uint16_t i = 0; i < cd.exception_table_length; ++i) {
                ExceptionTable et = cd.exception_table[i];
                UtilClasse::indentar_arquivo(o, nivel + 2);
                std::fprintf(o, "%d\t %d\t %d\t %d\t ", i, et.start_pc, et.end_pc, et.handler_pc);
                if (et.catch_type == 0) {
                    std::fprintf(o, "0\n");
                } else {
                    std::fprintf(o, "ConstantPoolInfo #%d\t %s\n",
                                 et.catch_type, formatar_constante(pool, et.catch_type));
                }
            }
        } else {
            std::fprintf(o, "Exception table is empty.\n");
        }
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Bytecode:\n");
        escrever_bytecode(cd, pool, nivel + 2);
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Attributes:\n");
        for (uint16_t i = 0; i < cd.attributes_count; ++i) {
            escrever_atributo(cd.attributes[i], i, pool, nivel + 2);
        }
    }
    else if (UtilClasse::utf8_igual_a(utf, "Exceptions")) {
        ExceptionsAttribute ex = a.info.exceptions_info;
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Nr.\t exception\t verbose\n");
        for (uint16_t i = 0; i < ex.number_of_exceptions; ++i) {
            UtilClasse::indentar_arquivo(o, nivel + 1);
            std::fprintf(o, "%d\t ConstantPoolInfo #%d\t %s\n",
                         i, ex.exception_index_table[i],
                         formatar_constante(pool, ex.exception_index_table[i]));
        }
    }
    else if (UtilClasse::utf8_igual_a(utf, "InnerClasses")) {
        InnerClassesAttribute ic = a.info.inner_classes_info;
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Nr.\t inner_class\t outer_class\t inner_name\t access flags\n");
        for (uint16_t i = 0; i < ic.number_of_classes; ++i) {
            Class c = ic.classes[i];
            UtilClasse::indentar_arquivo(o, nivel + 1);
            std::fprintf(o, "%d\t ConstantPoolInfo #%d <%s>\t ConstantPoolInfo #%d <%s>\t "
                            "ConstantPoolInfo #%d <%s>\t 0x%.4X [%s]\n",
                         i,
                         c.inner_class_info_index, formatar_constante(pool, c.inner_class_info_index),
                         c.outer_class_info_index, formatar_constante(pool, c.outer_class_info_index),
                         c.inner_name_index,       formatar_constante(pool, c.inner_name_index),
                         c.inner_class_access_flags, flags_acesso_str(c.inner_class_access_flags));
        }
    }
    else if (UtilClasse::utf8_igual_a(utf, "Synthetic")) {
        /* sem informacao adicional */
    }
    else if (UtilClasse::utf8_igual_a(utf, "SourceFile")) {
        SourceFileAttribute sf = a.info.source_file_info;
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Constant value index:\t ConstantPoolInfo #%d <%s>\n",
                     sf.sourcefile_index, formatar_constante(pool, sf.sourcefile_index));
    }
    else if (UtilClasse::utf8_igual_a(utf, "LineNumberTable")) {
        LineNumberTableAttribute ln = a.info.line_number_table_info;
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Nr.\t start_pc\t line_number\n");
        for (uint16_t i = 0; i < ln.line_number_table_length; ++i) {
            LineNumberTable t = ln.line_number_table[i];
            UtilClasse::indentar_arquivo(o, nivel + 1);
            std::fprintf(o, "%d\t %d\t %d\n", i, t.start_pc, t.line_number);
        }
    }
    else if (UtilClasse::utf8_igual_a(utf, "LocalVariableTable")) {
        LocalVariableTableAttribute lv = a.info.local_variable_table_info;
        UtilClasse::indentar_arquivo(o, nivel + 1);
        std::fprintf(o, "Nr.\t start_pc\t length\t index\t name\t descriptor\n");
        for (uint16_t i = 0; i < lv.local_variable_table_length; ++i) {
            LocalVariableTable v = lv.local_variable_table[i];
            UtilClasse::indentar_arquivo(o, nivel + 1);
            std::fprintf(o, "%d\t %d\t %d\t %d\t ConstantPoolInfo #%d <%s>\t "
                            "ConstantPoolInfo #%d <%s>\n",
                         i, v.start_pc, v.length, v.index,
                         v.name_index,       formatar_constante(pool, v.name_index),
                         v.descriptor_index, formatar_constante(pool, v.descriptor_index));
        }
    }
    else if (UtilClasse::utf8_igual_a(utf, "Deprecated") ||
             UtilClasse::utf8_igual_a(utf, "StackMapTable")) {
        /* sem detalhe adicional */
    }
    else {
        std::cerr << "The .class file has an invalid attribute. (escrever_atributo)\n";
        std::exit(6);
    }
    std::fprintf(o, "\n");
}

/* ----------------------------------------------------------------------- */
/* Desmontagem do bytecode                                                 */
/*                                                                         */
/* escrever_bytecode percorre o array cd.code byte a byte e imprime cada   */
/* instrucao no formato:                                                   */
/*                                                                         */
/*     <linha>\t<offset>\t<mnemonico> [operandos]                          */
/*                                                                         */
/* Onde:                                                                   */
/*   linha    = contador sequencial de instrucao (1-based, igual jclasslib)*/
/*   offset   = posicao da instrucao DENTRO do array de bytecode (PC)      */
/*   mnemonico= nome textual do opcode (tabela_mnemonicos[op])             */
/*                                                                         */
/* O grande desafio aqui e' que cada opcode tem um tamanho diferente em    */
/* bytes - alguns nao tem operando (1 byte), outros tem 1, 2, 4, ou        */
/* tamanho variavel (tableswitch/lookupswitch com padding ao multiplo de   */
/* 4 e wide que prefixa o opcode com 0xC4). Por isso o switch e' feito     */
/* manualmente por faixas de opcode e cada bloco avanca o ponteiro `p` do  */
/* tamanho correto antes do `continue`.                                    */
/*                                                                         */
/* Referencia: JVM Spec capitulo 6 ("The Java Virtual Machine              */
/* Instruction Set") e capitulo 4.7.3 (atributo Code).                     */
/* ----------------------------------------------------------------------- */
void escrever_bytecode(CodeAttribute cd, ConstantPoolInfo* pool, uint8_t nivel)
{
    /* Passo 0 - setup. Pegamos o FILE* global de saida e definimos os
     * limites do percorrimento. `inicio` aponta para o primeiro byte
     * do bytecode; `fim` e' o sentinela (1 alem do ultimo byte valido);
     * `p` e' o cursor que avanca conforme cada instrucao e' decodificada;
     * `linha` e' o contador de instrucao, comeca em 1 igual ao jclasslib. */
    FILE* o = saida_global;
    u1* inicio = cd.code;
    u1* fim    = cd.code + cd.code_length;
    u1* p      = inicio;
    uint32_t linha = 1;

    /* Loop principal: enquanto houver bytecode pra decodificar. */
    while (p < fim) {
        /* Passo 1 - calcula o offset (PC) da instrucao atual relativo
         * ao inicio do bytecode. E' o "endereco" usado por instrucoes
         * de salto. Tambem captura o opcode (primeiro byte da instrucao). */
        u4 offset = static_cast<u4>(p - inicio);
        u1 op = *p;

        /* Passo 2 - imprime o prefixo comum: indentacao + numero da
         * linha + offset + mnemonico. As ramificacoes abaixo so
         * acrescentarao os operandos especificos e o newline. */
        UtilClasse::indentar_arquivo(o, nivel);
        std::fprintf(o, "%d\t%d\t%s", linha++, offset, tabela_mnemonicos[op].c_str());

        /* Passo 3 - classificar o opcode pela sua categoria estrutural.
         *
         * Categoria A: instrucoes de 1 BYTE (sem operando inline).
         * Cobrem os opcodes mais simples: constantes empilhadas (iconst_*,
         * aconst_null, ...), loads/stores curtos (iload_0..3, istore_0..3),
         * aritmeticas (iadd, isub, imul, idiv, ...), comparacoes (lcmp,
         * fcmpl, ...), conversoes (i2l, i2f, ...), returns (ireturn,
         * areturn, return), athrow, monitorenter, monitorexit, etc.
         *
         * As faixas vem da tabela de opcodes da JVM Spec; junte tudo num
         * unico predicado para o "fast path" sem operando. */
        const bool sem_operando =
            op <= 0x0f ||
            (op >= 0x1a && op <= 0x35) ||
            (op >= 0x3b && op <= 0x83) ||
            (op >= 0x85 && op <= 0x98) ||
            (op >= 0xac && op <= 0xb1) ||
            op == 0xbe || op == 0xbf || op == 0xc2 || op == 0xc3;

        if (sem_operando) {
            /* Caso A: nenhuma info adicional. Fecha a linha e avanca 1. */
            std::fprintf(o, "\n");
            p += 1;
            continue;
        }

        /* Categoria B: ldc (0x12) - empilha constante do pool.
         * Formato: 1 byte de indice (so 1 byte, faixa 0..255).
         * Imprime "#idx <valor formatado>". */
        if (op == 0x12) {
            std::fprintf(o, " #%d <%s>\n", *(p + 1), formatar_constante(pool, *(p + 1)));
            p += 2;
            continue;
        }

        /* Categoria C: newarray (0xbc) - cria array de tipo primitivo.
         * O byte seguinte (atype) identifica o tipo:
         *   4=boolean, 5=char, 6=float, 7=double,
         *   8=byte,    9=short,10=int,  11=long
         * O array de nomes abaixo segue essa ordem (indice - 4). */
        if (op == 0xbc) {
            u1 atype = *(p + 1);
            const char* nomes[] = {
                "boolean", "char", "float", "double", "byte", "short", "int", "long"
            };
            std::fprintf(o, " %d (%s)\n", atype, nomes[atype - 4]);
            p += 2;
            continue;
        }

        /* Categoria D: instrucoes com 1 BYTE de operando inteiro.
         * Cobre:
         *   bipush (0x10)      - empilha byte como int (com sinal extendido)
         *   iload..aload (0x15..0x19) - load por indice de local
         *   istore..astore (0x36..0x3a) - store por indice de local
         *   ret (0xa9)         - retorno de subrotina (subroutine return) */
        if (op == 0x10 || (op >= 0x15 && op <= 0x19) ||
            (op >= 0x36 && op <= 0x3a) || op == 0xa9) {
            std::fprintf(o, " %d\n", *(p + 1));
            p += 2;
            continue;
        }

        /* Categoria E: sipush (0x11) - empilha short como int.
         * 2 bytes COM SINAL formando um int16_t. Big-endian. */
        if (op == 0x11) {
            int16_t n = (int16_t)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " %d\n", n);
            p += 3;
            continue;
        }

        /* Categoria F: iinc (0x84) - incrementa variavel local.
         * 1 byte indice da local + 1 byte de constante a somar.
         * Formato de saida "X by Y" (X=indice, Y=incremento). */
        if (op == 0x84) {
            std::fprintf(o, " %d by %d\n", *(p + 1), *(p + 2));
            p += 3;
            continue;
        }

        /* Categoria G: instrucoes com 2 BYTES de indice no constant pool.
         * Cobre uma familia grande:
         *   ldc_w (0x13), ldc2_w (0x14)        - load constante "wide"
         *   getstatic..invokestatic (0xb2..0xb8) - acesso a campos e
         *                                          chamadas de metodos
         *   new (0xbb), anewarray (0xbd)       - alocacao de objeto/array
         *   checkcast (0xc0), instanceof (0xc1) - testes de tipo
         * Em todos: monta-se um u2 big-endian e imprime "#idx <valor>". */
        if (op == 0x13 || op == 0x14 ||
            (op >= 0xb2 && op <= 0xb8) ||
            op == 0xbb || op == 0xbd ||
            op == 0xc0 || op == 0xc1) {
            u2 n = (u2)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " #%d <%s>\n", n, formatar_constante(pool, n));
            p += 3;
            continue;
        }

        /* Categoria H: saltos com offset 2-bytes COM SINAL.
         * Cobre:
         *   if* (0x99..0xa6)  - comparacoes condicionais
         *   goto (0xa7)       - salto incondicional
         *   jsr  (0xa8)       - salto para subrotina
         *   ifnull (0xc6), ifnonnull (0xc7) - testes de null
         * O destino e' OFFSET RELATIVO ao PC desta instrucao; imprimimos
         * o PC absoluto destino e o offset com sinal entre parenteses. */
        if ((op >= 0x99 && op <= 0xa8) || op == 0xc6 || op == 0xc7) {
            int16_t n = (int16_t)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " %d (%+d)\n", offset + n, n);
            p += 3;
            continue;
        }

        /* Categoria I: multianewarray (0xc5).
         * 2 bytes de indice no pool (classe do array) + 1 byte com
         * a quantidade de dimensoes do array a criar. */
        if (op == 0xc5) {
            u2 n = (u2)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " #%d <%s> dim %d\n", n, formatar_constante(pool, n), *(p + 3));
            p += 4;
            continue;
        }

        /* Categoria J: invokeinterface (0xb9).
         * 2 bytes de indice no pool + 1 byte de count (n. de args na pilha)
         * + 1 byte HISTORICAMENTE reservado que DEVE ser 0 (assert valida).
         * Total 5 bytes. */
        if (op == 0xb9) {
            u2 n = (u2)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " #%d <%s> count %d\n", n, formatar_constante(pool, n), *(p + 3));
            assert(*(p + 4) == 0);
            p += 5;
            continue;
        }

        /* Categoria K: tableswitch (0xaa) - switch denso (intervalo
         * contiguo de cases).
         *
         * Layout (apos o opcode):
         *   - 0 a 3 bytes de PADDING para alinhar o proximo campo a um
         *     multiplo de 4 a partir do inicio do bytecode (instrucao
         *     pseudo-variavel: tamanho depende do offset!).
         *   - 4 bytes default_offset (com sinal, big-endian)
         *   - 4 bytes low  (limite inferior do intervalo)
         *   - 4 bytes high (limite superior do intervalo)
         *   - (high - low + 1) entradas de 4 bytes (offset relativo
         *     do case correspondente).
         */
        if (op == 0xaa) {
            /* Calcula o padding para alinhar a 4. (offset+1) porque o
             * byte do opcode ja foi "consumido" - o alinhamento e'
             * medido a partir do byte APOS o opcode. */
            u4 pad = (4 - (offset + 1) % 4) % 4;

            /* Le os tres campos de 4 bytes big-endian. */
            int32_t def_off =
                ((int32_t)*(p + pad + 1) << 24) | (*(p + pad + 2) << 16) |
                (*(p + pad + 3) << 8) | *(p + pad + 4);
            int32_t low =
                ((int32_t)*(p + pad + 5) << 24) | (*(p + pad + 6) << 16) |
                (*(p + pad + 7) << 8) | *(p + pad + 8);
            int32_t high =
                ((int32_t)*(p + pad + 9) << 24) | (*(p + pad + 10) << 16) |
                (*(p + pad + 11) << 8) | *(p + pad + 12);
            std::fprintf(o, " default:%+d low:%d high:%d\n", def_off, low, high);

            /* Imprime cada case do intervalo [low, high]. */
            for (int32_t k = 0; k <= high - low; ++k) {
                int32_t jmp =
                    ((int32_t)*(p + pad + 13 + k * 4) << 24) | (*(p + pad + 14 + k * 4) << 16) |
                    (*(p + pad + 15 + k * 4) << 8) | *(p + pad + 16 + k * 4);
                UtilClasse::indentar_arquivo(o, nivel);
                std::fprintf(o, "\t\tcase %d: %+d\n", low + k, jmp);
            }

            /* Avanca p pelo tamanho total da instrucao:
             *   1 (opcode) + pad + 4 (default) + 4 (low) + 4 (high)
             *   + 4 * (high-low+1) entradas. */
            p += 1 + pad + 4 + 4 + 4 + (high - low + 1) * 4;
            continue;
        }

        /* Categoria L: lookupswitch (0xab) - switch esparso (pares
         * match/offset arbitrarios).
         *
         * Layout (apos o opcode):
         *   - 0 a 3 bytes de PADDING (mesmo alinhamento do tableswitch)
         *   - 4 bytes default_offset
         *   - 4 bytes npairs (quantidade de pares match->offset)
         *   - npairs entradas de 8 bytes: 4 bytes match + 4 bytes offset.
         */
        if (op == 0xab) {
            u4 pad = (4 - (offset + 1) % 4) % 4;
            int32_t def_off =
                ((int32_t)*(p + pad + 1) << 24) | (*(p + pad + 2) << 16) |
                (*(p + pad + 3) << 8) | *(p + pad + 4);
            int32_t npairs =
                ((int32_t)*(p + pad + 5) << 24) | (*(p + pad + 6) << 16) |
                (*(p + pad + 7) << 8) | *(p + pad + 8);
            std::fprintf(o, " default:%+d npairs:%d\n", def_off, npairs);

            /* Cada par tem 8 bytes: 4 do valor a casar, 4 do jump. */
            for (int32_t k = 0; k < npairs; ++k) {
                int32_t match =
                    ((int32_t)*(p + pad + 9 + k * 8) << 24) | (*(p + pad + 10 + k * 8) << 16) |
                    (*(p + pad + 11 + k * 8) << 8) | *(p + pad + 12 + k * 8);
                int32_t jmp =
                    ((int32_t)*(p + pad + 13 + k * 8) << 24) | (*(p + pad + 14 + k * 8) << 16) |
                    (*(p + pad + 15 + k * 8) << 8) | *(p + pad + 16 + k * 8);
                UtilClasse::indentar_arquivo(o, nivel);
                std::fprintf(o, "\t\tmatch %d: %+d\n", match, jmp);
            }

            /* Avanca: 1 (opcode) + pad + 4 (default) + 4 (npairs)
             *   + npairs * 8 bytes de entradas. */
            p += 1 + pad + 4 + 4 + npairs * 8;
            continue;
        }

        /* Categoria M: goto_w (0xc8) e jsr_w (0xc9) - saltos com
         * offset 4-bytes COM SINAL. Mesma logica dos saltos curtos
         * (categoria H) mas com alcance estendido. */
        if (op == 0xc8 || op == 0xc9) {
            int32_t n =
                ((int32_t)*(p + 1) << 24) | (*(p + 2) << 16) |
                (*(p + 3) << 8) | *(p + 4);
            std::fprintf(o, " %d (%+d)\n", offset + n, n);
            p += 5;
            continue;
        }

        /* Categoria N: wide (0xc4) - prefixo que ESTENDE o operando
         * da proxima instrucao de 1 para 2 bytes (e de 1+1 para 2+2
         * no caso especial do iinc).
         *
         * Layout:
         *   - 1 byte 0xc4 (o wide ja consumido aqui)
         *   - 1 byte = opcode modificado (ex.: iload, istore, iinc, ret)
         *   - 2 bytes = indice de variavel local
         *   - se opcode modificado == iinc (0x84), +2 bytes = constante
         *     com sinal a somar.
         *
         * A saida imprime DUAS linhas: uma para o wide e outra para
         * a instrucao modificada (offset+1, mnemonico estendido). */
        if (op == 0xc4) {
            u2 idx16 = (u2)(((*(p + 2)) << 8) | *(p + 3));
            /* Termina a linha do "wide" (sem operandos). */
            std::fprintf(o, "\n");
            /* Imprime a instrucao modificada na linha seguinte. */
            UtilClasse::indentar_arquivo(o, nivel);
            std::fprintf(o, "%d\t%d\t%s %d",
                         linha++, offset + 1,
                         tabela_mnemonicos[*(p + 1)].c_str(),
                         idx16);
            if (*(p + 1) == 0x84) {
                /* wide iinc tem +2 bytes para a constante. */
                int16_t cb = (int16_t)(((*(p + 4)) << 8) | *(p + 5));
                std::fprintf(o, " by %d", cb);
                p += 6;
            } else {
                p += 4;
            }
            std::fprintf(o, "\n");
            continue;
        }

        /* Se chegamos aqui um opcode invalido foi encontrado - aborta.
         * Isso normalmente indica .class corrompido ou desalinhamento
         * em alguma das categorias acima (pad incorreto, p. ex.). */
        std::fprintf(o, "Invalid instruction opcode.\n");
        std::exit(7);
    }
}

/* ----------------------------------------------------------------------- */
/* Auxiliares de formatacao                                                */
/*                                                                         */
/* flags_acesso_str    - traduz mascara de access_flags em texto humano.   */
/* formatar_constante  - formata uma entrada do constant pool em string    */
/*                       legivel (resolve referencias para Utf8 quando     */
/*                       necessario, decodifica numeros, junta nome+desc   */
/*                       de NameAndType, etc.). E' recursivo para tipos    */
/*                       compostos (Class -> Utf8, FieldRef -> Class+NT,   */
/*                       e assim por diante).                              */
/* ----------------------------------------------------------------------- */
const char* flags_acesso_str(u2 access_flags)
{
    std::stringstream ss;
    for (size_t i = 0; i < QTD_FLAGS; ++i) {
        if (access_flags & MASCARAS_FLAG[i]) {
            ss << NOMES_FLAG[i];
        }
    }
    return UtilClasse::ss_para_cstr(ss);
}

const char* formatar_constante(ConstantPoolInfo* pool, u2 index)
{
    ConstantPoolInfo c = pool[index - 1];

    switch (c.tag) {
        case ConstClass:
            return formatar_constante(pool, c.info.class_info.name_index);

        case ConstFieldRef: {
            ConstFieldRefInfo fr = c.info.field_ref_info;
            ConstNameTypeInfo nt = pool[fr.name_and_type_index - 1].info.name_type_info;
            const char* cls = formatar_constante(pool, fr.class_index);
            const char* nm  = formatar_constante(pool, nt.name_index);
            std::stringstream ss;
            ss << cls << "." << nm;
            return UtilClasse::ss_para_cstr(ss);
        }
        case ConstMethodRef: {
            ConstMethodRefInfo mr = c.info.method_ref_info;
            ConstNameTypeInfo nt = pool[mr.name_and_type_index - 1].info.name_type_info;
            const char* cls = formatar_constante(pool, mr.class_index);
            const char* nm  = formatar_constante(pool, nt.name_index);
            std::stringstream ss;
            ss << cls << "." << nm;
            return UtilClasse::ss_para_cstr(ss);
        }
        case ConstInterfaceMethodRef: {
            ConstInterfaceMethodRefInfo ir = c.info.interface_method_ref_info;
            ConstNameTypeInfo nt = pool[ir.name_and_type_index - 1].info.name_type_info;
            const char* cls = formatar_constante(pool, ir.class_index);
            const char* nm  = formatar_constante(pool, nt.name_index);
            std::stringstream ss;
            ss << cls << "." << nm;
            return UtilClasse::ss_para_cstr(ss);
        }
        case ConstStr:
            return formatar_constante(pool, c.info.str_info.string_index);

        case ConstInt: {
            int32_t n = (int32_t)c.info.int_info.bytes;
            char* buf = static_cast<char*>(std::malloc(64));
            std::snprintf(buf, 64, "%d", n);
            return buf;
        }
        case ConstFloat: {
            /* Reconstrucao manual de float IEEE-754 a partir dos 4
             * bytes brutos:
             *   bit 31     = sinal (s)
             *   bits 30-23 = expoente (e), bias 127
             *   bits 22-0  = mantissa (m)
             * Se e == 0 e' subnormal; senao tem bit implicito de 1
             * (0x800000). O fator 2^(e-150) ja inclui o bias e o
             * shift de 23 bits da mantissa: -127 (bias) -23 = -150. */
            u4 b = c.info.float_info.bytes;
            int s = ((b >> 31) == 0) ? 1 : -1;
            int e = ((b >> 23) & 0xff);
            int m = (e == 0) ? (b & 0x7fffff) << 1 : (b & 0x7fffff) | 0x800000;
            float v = s * m * std::pow(2.0f, (float)(e - 150));
            char* buf = static_cast<char*>(std::malloc(64));
            std::snprintf(buf, 64, "%f", v);
            return buf;
        }
        case ConstLong: {
            ConstLongInfo li = c.info.long_info;
            int64_t v = ((int64_t)li.high_bytes << 32) + li.low_bytes;
            char* buf = static_cast<char*>(std::malloc(64));
            std::snprintf(buf, 64, "%lld", (long long)v);
            return buf;
        }
        case ConstDouble: {
            /* Mesmo principio do float, mas com 64 bits:
             *   bit 63     = sinal
             *   bits 62-52 = expoente (bias 1023)
             *   bits 51-0  = mantissa
             * Fator 2^(e-1075): -1023 (bias) -52 (shift) = -1075. */
            ConstDoubleInfo di = c.info.double_info;
            int64_t bits = ((int64_t)di.high_bytes << 32) + di.low_bytes;
            int s = ((bits >> 63) == 0) ? 1 : -1;
            int e = (int)((bits >> 52) & 0x7ffL);
            int64_t m = (e == 0)
                        ? (bits & 0xfffffffffffffL) << 1
                        : (bits & 0xfffffffffffffL) | 0x10000000000000L;
            double v = s * m * std::pow(2.0, (double)(e - 1075));
            char* buf = static_cast<char*>(std::malloc(64));
            std::snprintf(buf, 64, "%f", v);
            return buf;
        }
        case ConstNameType: {
            ConstNameTypeInfo nt = c.info.name_type_info;
            const char* nm   = formatar_constante(pool, nt.name_index);
            const char* desc = formatar_constante(pool, nt.descriptor_index);
            size_t total = std::strlen(nm) + std::strlen(desc) + 1;
            char* res = static_cast<char*>(std::malloc(total));
            std::strcpy(res, nm);
            std::strcat(res, desc);
            return res;
        }
        case ConstUtf8: {
            /* Copia os bytes da string filtrando NAO-imprimiveis.
             * Isto evita lixo no terminal quando o .class contem
             * "modified UTF-8" com bytes do BMP estendido. */
            ConstUtf8Info u = c.info.utf8_info;
            char* buf = static_cast<char*>(std::malloc((u.length + 1) * sizeof(char)));
            uint16_t j = 0;
            for (uint16_t i = 0; i < u.length; ++i) {
                if (std::isprint(u.bytes[i])) {
                    buf[j++] = (char)u.bytes[i];
                }
            }
            buf[j] = '\0';
            return buf;
        }
        default:
            std::cerr << "The .class file has an invalid tag (" << c.tag << ") in its constant pool.\n";
            std::exit(5);
    }
    return nullptr;
}
