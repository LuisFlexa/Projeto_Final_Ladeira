/*
 * exibidor_classe.cpp
 *
 * Producao da saida textual estilo jclasslib. O formato (espacos,
 * tabs, ordem dos campos) foi mantido igual ao do leitor original
 * para preservar a equivalencia funcional com a baseline de teste.
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
    FILE* saida_global = nullptr;   /* usada por todas as escrever_* */

    /* mascaras + nomes em ordem fixa (a mesma usada por jclasslib) */
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
/* ----------------------------------------------------------------------- */
void escrever_bytecode(CodeAttribute cd, ConstantPoolInfo* pool, uint8_t nivel)
{
    FILE* o = saida_global;
    u1* inicio = cd.code;
    u1* fim    = cd.code + cd.code_length;
    u1* p      = inicio;
    uint32_t linha = 1;

    while (p < fim) {
        u4 offset = static_cast<u4>(p - inicio);
        u1 op = *p;

        UtilClasse::indentar_arquivo(o, nivel);
        std::fprintf(o, "%d\t%d\t%s", linha++, offset, tabela_mnemonicos[op].c_str());

        /* Categoria 1: instrucao sem operando */
        const bool sem_operando =
            op <= 0x0f ||
            (op >= 0x1a && op <= 0x35) ||
            (op >= 0x3b && op <= 0x83) ||
            (op >= 0x85 && op <= 0x98) ||
            (op >= 0xac && op <= 0xb1) ||
            op == 0xbe || op == 0xbf || op == 0xc2 || op == 0xc3;

        if (sem_operando) {
            std::fprintf(o, "\n");
            p += 1;
            continue;
        }

        /* ldc: 1 byte de indice no pool */
        if (op == 0x12) {
            std::fprintf(o, " #%d <%s>\n", *(p + 1), formatar_constante(pool, *(p + 1)));
            p += 2;
            continue;
        }

        /* newarray: tipo do array */
        if (op == 0xbc) {
            u1 atype = *(p + 1);
            const char* nomes[] = {
                "boolean", "char", "float", "double", "byte", "short", "int", "long"
            };
            std::fprintf(o, " %d (%s)\n", atype, nomes[atype - 4]);
            p += 2;
            continue;
        }

        /* bipush, *load_x (15..19), *store_x (36..3a), ret: 1 byte de operando */
        if (op == 0x10 || (op >= 0x15 && op <= 0x19) ||
            (op >= 0x36 && op <= 0x3a) || op == 0xa9) {
            std::fprintf(o, " %d\n", *(p + 1));
            p += 2;
            continue;
        }

        /* sipush: 2 bytes signed */
        if (op == 0x11) {
            int16_t n = (int16_t)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " %d\n", n);
            p += 3;
            continue;
        }

        /* iinc */
        if (op == 0x84) {
            std::fprintf(o, " %d by %d\n", *(p + 1), *(p + 2));
            p += 3;
            continue;
        }

        /* ldc_w, ldc2_w, getstatic..invokestatic, new, anewarray, checkcast, instanceof */
        if (op == 0x13 || op == 0x14 ||
            (op >= 0xb2 && op <= 0xb8) ||
            op == 0xbb || op == 0xbd ||
            op == 0xc0 || op == 0xc1) {
            u2 n = (u2)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " #%d <%s>\n", n, formatar_constante(pool, n));
            p += 3;
            continue;
        }

        /* if*, goto, ifnull, ifnonnull: 2 bytes signed (offset relativo) */
        if ((op >= 0x99 && op <= 0xa8) || op == 0xc6 || op == 0xc7) {
            int16_t n = (int16_t)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " %d (%+d)\n", offset + n, n);
            p += 3;
            continue;
        }

        /* multianewarray */
        if (op == 0xc5) {
            u2 n = (u2)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " #%d <%s> dim %d\n", n, formatar_constante(pool, n), *(p + 3));
            p += 4;
            continue;
        }

        /* invokeinterface */
        if (op == 0xb9) {
            u2 n = (u2)(((*(p + 1)) << 8) | *(p + 2));
            std::fprintf(o, " #%d <%s> count %d\n", n, formatar_constante(pool, n), *(p + 3));
            assert(*(p + 4) == 0);
            p += 5;
            continue;
        }

        /* tableswitch */
        if (op == 0xaa) {
            u4 pad = (4 - (offset + 1) % 4) % 4;
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
            for (int32_t k = 0; k <= high - low; ++k) {
                int32_t jmp =
                    ((int32_t)*(p + pad + 13 + k * 4) << 24) | (*(p + pad + 14 + k * 4) << 16) |
                    (*(p + pad + 15 + k * 4) << 8) | *(p + pad + 16 + k * 4);
                UtilClasse::indentar_arquivo(o, nivel);
                std::fprintf(o, "\t\tcase %d: %+d\n", low + k, jmp);
            }
            p += 1 + pad + 4 + 4 + 4 + (high - low + 1) * 4;
            continue;
        }

        /* lookupswitch */
        if (op == 0xab) {
            u4 pad = (4 - (offset + 1) % 4) % 4;
            int32_t def_off =
                ((int32_t)*(p + pad + 1) << 24) | (*(p + pad + 2) << 16) |
                (*(p + pad + 3) << 8) | *(p + pad + 4);
            int32_t npairs =
                ((int32_t)*(p + pad + 5) << 24) | (*(p + pad + 6) << 16) |
                (*(p + pad + 7) << 8) | *(p + pad + 8);
            std::fprintf(o, " default:%+d npairs:%d\n", def_off, npairs);
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
            p += 1 + pad + 4 + 4 + npairs * 8;
            continue;
        }

        /* goto_w, jsr_w */
        if (op == 0xc8 || op == 0xc9) {
            int32_t n =
                ((int32_t)*(p + 1) << 24) | (*(p + 2) << 16) |
                (*(p + 3) << 8) | *(p + 4);
            std::fprintf(o, " %d (%+d)\n", offset + n, n);
            p += 5;
            continue;
        }

        /* wide */
        if (op == 0xc4) {
            u2 idx16 = (u2)(((*(p + 2)) << 8) | *(p + 3));
            std::fprintf(o, "\n");
            UtilClasse::indentar_arquivo(o, nivel);
            std::fprintf(o, "%d\t%d\t%s %d",
                         linha++, offset + 1,
                         tabela_mnemonicos[*(p + 1)].c_str(),
                         idx16);
            if (*(p + 1) == 0x84) {
                int16_t cb = (int16_t)(((*(p + 4)) << 8) | *(p + 5));
                std::fprintf(o, " by %d", cb);
                p += 6;
            } else {
                p += 4;
            }
            std::fprintf(o, "\n");
            continue;
        }

        std::fprintf(o, "Invalid instruction opcode.\n");
        std::exit(7);
    }
}

/* ----------------------------------------------------------------------- */
/* Auxiliares de formatacao                                                */
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
