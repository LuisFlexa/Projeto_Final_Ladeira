/*
 * exibidor_classe.hpp
 *
 * Funcoes que despejam um ArquivoClasse para um FILE* num formato
 * semelhante ao jclasslib. Cada secao do .class vira um bloco
 * delimitado por chaves; entradas do pool de constantes mostram tanto
 * o indice (#N) quanto o valor "resolvido" entre <colchetes angulares>.
 *
 * Estas funcoes nao alteram o ArquivoClasse - so' leem.
 */
#ifndef EXIBIDOR_CLASSE_HPP
#define EXIBIDOR_CLASSE_HPP

#include "tipos_basicos.hpp"
#include "arquivo_classe.hpp"
#include "leitor_classe.hpp"
#include <cstdio>
#include <string>

/* Saida principal. As demais sao chamadas em sequencia por esta. */
void escrever_arquivo(ArquivoClasse* arq, FILE* saida);

/* Cada secao do formato .class */
void escrever_info_geral(ArquivoClasse* arq);
void escrever_pool_constantes(ArquivoClasse* arq);
void escrever_interfaces(ArquivoClasse* arq);
void escrever_campos(ArquivoClasse* arq);
void escrever_metodos(ArquivoClasse* arq);
void escrever_atributos(ArquivoClasse* arq);

/* Auxiliares usadas em outras unidades (executor, instancia, etc) */
void escrever_atributo(AttributeInfo a, uint32_t idx, ConstantPoolInfo* pool, uint8_t nivel);
void escrever_bytecode(CodeAttribute c, ConstantPoolInfo* pool, uint8_t nivel);

const char* flags_acesso_str(u2 access_flags);
const char* formatar_constante(ConstantPoolInfo* pool, u2 index);

/* ---------------------------------------------------------------------------
 * Tabela com os mnemonicos dos 202 opcodes da JVM 8, indexados pelo opcode.
 * Esta lista e' apenas para exibicao no leitor; o executor mantem a sua
 * propria tabela de ponteiros para funcao.
 * -------------------------------------------------------------------------*/
static const std::string tabela_mnemonicos[] = {
    /* 0x00 */ "nop", "aconst_null",
    /* 0x02 */ "iconst_m1", "iconst_0", "iconst_1", "iconst_2",
    /* 0x06 */ "iconst_3", "iconst_4", "iconst_5",
    /* 0x09 */ "lconst_0", "lconst_1",
    /* 0x0B */ "fconst_0", "fconst_1", "fconst_2",
    /* 0x0E */ "dconst_0", "dconst_1",
    /* 0x10 */ "bipush", "sipush",
    /* 0x12 */ "ldc", "ldc_w", "ldc2_w",
    /* 0x15 */ "iload", "lload", "fload", "dload", "aload",
    /* 0x1A */ "iload_0", "iload_1", "iload_2", "iload_3",
    /* 0x1E */ "lload_0", "lload_1", "lload_2", "lload_3",
    /* 0x22 */ "fload_0", "fload_1", "fload_2", "fload_3",
    /* 0x26 */ "dload_0", "dload_1", "dload_2", "dload_3",
    /* 0x2A */ "aload_0", "aload_1", "aload_2", "aload_3",
    /* 0x2E */ "iaload", "laload", "faload", "daload",
    /* 0x32 */ "aaload", "baload", "caload", "saload",
    /* 0x36 */ "istore", "lstore", "fstore", "dstore", "astore",
    /* 0x3B */ "istore_0", "istore_1", "istore_2", "istore_3",
    /* 0x3F */ "lstore_0", "lstore_1", "lstore_2", "lstore_3",
    /* 0x43 */ "fstore_0", "fstore_1", "fstore_2", "fstore_3",
    /* 0x47 */ "dstore_0", "dstore_1", "dstore_2", "dstore_3",
    /* 0x4B */ "astore_0", "astore_1", "astore_2", "astore_3",
    /* 0x4F */ "iastore", "lastore", "fastore", "dastore",
    /* 0x53 */ "aastore", "bastore", "castore", "sastore",
    /* 0x57 */ "pop", "pop2",
    /* 0x59 */ "dup", "dup_x1", "dup_x2", "dup2", "dup2_x1", "dup2_x2",
    /* 0x5F */ "swap",
    /* 0x60 */ "iadd", "ladd", "fadd", "dadd",
    /* 0x64 */ "isub", "lsub", "fsub", "dsub",
    /* 0x68 */ "imul", "lmul", "fmul", "dmul",
    /* 0x6C */ "idiv", "ldiv", "fdiv", "ddiv",
    /* 0x70 */ "irem", "lrem", "frem", "drem",
    /* 0x74 */ "ineg", "lneg", "fneg", "dneg",
    /* 0x78 */ "ishl", "lshl", "ishr", "lshr",
    /* 0x7C */ "iushr", "lushr",
    /* 0x7E */ "iand", "land", "ior", "lor", "ixor", "lxor",
    /* 0x84 */ "iinc",
    /* 0x85 */ "i2l", "i2f", "i2d", "l2i", "l2f", "l2d",
    /* 0x8B */ "f2i", "f2l", "f2d", "d2i", "d2l", "d2f",
    /* 0x91 */ "i2b", "i2c", "i2s",
    /* 0x94 */ "lcmp", "fcmpl", "fcmpg", "dcmpl", "dcmpg",
    /* 0x99 */ "ifeq", "ifne", "iflt", "ifge", "ifgt", "ifle",
    /* 0x9F */ "if_icmpeq", "if_icmpne", "if_icmplt", "if_icmpge", "if_icmpgt", "if_icmple",
    /* 0xA5 */ "if_acmpeq", "if_acmpne",
    /* 0xA7 */ "goto", "jsr", "ret",
    /* 0xAA */ "tableswitch", "lookupswitch",
    /* 0xAC */ "ireturn", "lreturn", "freturn", "dreturn", "areturn", "return",
    /* 0xB2 */ "getstatic", "putstatic", "getfield", "putfield",
    /* 0xB6 */ "invokevirtual", "invokespecial", "invokestatic", "invokeinterface",
    /* 0xBA */ "UNUSED",
    /* 0xBB */ "new", "newarray", "anewarray", "arraylength", "athrow",
    /* 0xC0 */ "checkcast", "instanceof",
    /* 0xC2 */ "monitorenter", "monitorexit",
    /* 0xC4 */ "wide", "multianewarray",
    /* 0xC6 */ "ifnull", "ifnonnull",
    /* 0xC8 */ "goto_w", "jsr_w"
};

#endif
