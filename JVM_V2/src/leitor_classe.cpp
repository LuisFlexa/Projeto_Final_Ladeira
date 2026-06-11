/*
 * leitor_classe.cpp
 * ------------------------------------------------------------------
 * Implementacao do parser de arquivos .class (Java SE 1.8 / major 52).
 *
 * O formato binario do .class esta definido na JVM Spec, capitulo 4.
 * A estrutura no disco e' rigorosamente sequencial e big-endian:
 *
 *   ClassFile {
 *     u4 magic;                 // 0xCAFEBABE
 *     u2 minor_version;
 *     u2 major_version;
 *     u2 constant_pool_count;
 *     cp_info constant_pool[constant_pool_count - 1];   // 1-indexado!
 *     u2 access_flags;
 *     u2 this_class;
 *     u2 super_class;
 *     u2 interfaces_count;
 *     u2 interfaces[interfaces_count];
 *     u2 fields_count;
 *     field_info fields[fields_count];
 *     u2 methods_count;
 *     method_info methods[methods_count];
 *     u2 attributes_count;
 *     attribute_info attributes[attributes_count];
 *   }
 *
 * A ordem das chamadas em ler_arquivo() reflete exatamente essa ordem.
 * As funcoes ler_const_* leem UMA entrada do pool por vez; o caller
 * (ler_pool_constantes) faz o despacho pelo tag. As funcoes ler_attr_*
 * sao similares, e o despacho ali e' feito por NOME (string UTF-8 do
 * pool), nao por tag - ver ler_atributo().
 *
 * Convencao de tipos basicos (definida em tipos_basicos.hpp):
 *   u1 = uint8_t, u2 = uint16_t, u4 = uint32_t.
 */
#include "leitor_classe.hpp"
#include "util_classe.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

/* Construtor: detecta a endianness da maquina hospedeira uma unica
 * vez na construcao. O resultado e' usado pelas funcoes ler_u2/u4
 * para decidir se precisam fazer byte-swap dos bytes lidos.
 * Como o .class spec garante BIG-ENDIAN, em maquinas x86/x64 (LE)
 * temos sempre que reverter; em hardware BE (raro hoje), basta
 * um fread direto. */
LeitorClasse::LeitorClasse()
    : maquina_little_endian(host_little_endian())
{
}

/* Pequeno truque classico: pega o endereco de um int=1 e olha o
 * primeiro byte. Em little-endian o byte menos significativo (1)
 * vem primeiro, entao *char == 1. Em big-endian o byte mais
 * significativo (0) vem primeiro, entao *char == 0. */
bool LeitorClasse::host_little_endian()
{
    int n = 1;
    return *reinterpret_cast<char*>(&n) == 1;
}

/* -------------------------------------------------------------------------
 * Entrada principal
 *
 * Le um .class inteiro do FILE* aberto e retorna um ArquivoClasse*
 * preenchido. A funcao falha (std::exit) se o arquivo nao for um
 * .class valido ou se a versao for incompativel.
 *
 * O fluxo segue exatamente a ordem do formato binario - cada
 * chamada le um bloco consecutivo de bytes da posicao atual do fp.
 * -----------------------------------------------------------------------*/
ArquivoClasse* LeitorClasse::ler_arquivo(FILE* fp)
{
    ArquivoClasse* arq = new ArquivoClasse();

    /* 1) Magic number: 4 bytes que devem ser 0xCAFEBABE.
     *    Se nao for, o arquivo nao e' um .class - aborta. */
    ler_magic(fp, arq);
    if (!magic_valido(arq)) {
        std::cerr << "Invalid file format!\n"
                  << "The expected Magic Signature \"0xCAFEBABE\" for the .class file was not found.\n";
        delete arq;
        std::exit(3);
    }

    /* 2) Versoes: minor (u2) seguido de major (u2).
     *    Major <= 52 corresponde a Java SE 8 ou inferior - so esses
     *    sao suportados por este leitor. */
    ler_versoes(fp, arq);
    if (!versao_permitida(arq, 45)) {
        double mv = UtilClasse::versao_para_msg_erro(arq);
        if (mv != 0) {
            std::cerr << "Versions higher than Java SE 1.8 (52) are not supported. "
                      << "The class file version is Java SE" << mv << ".\n";
            delete arq;
            std::exit(4);
        }
    }

    /* 3) Constant Pool: primeiro o tamanho (u2), depois as entradas. */
    ler_pool_count(fp, arq);
    ler_pool_constantes(fp, arq);

    /* 4) Cabecalho da classe: flags + indices p/ this/super class. */
    ler_flags_acesso(fp, arq);
    ler_this_class(fp, arq);
    ler_super_class(fp, arq);

    /* 5) Interfaces implementadas: contador + array de indices. */
    ler_qtd_interfaces(fp, arq);
    ler_interfaces(fp, arq);

    /* 6) Campos (variaveis de classe e de instancia). */
    ler_qtd_campos(fp, arq);
    ler_campos(fp, arq);

    /* 7) Metodos (incluindo o construtor <init> e <clinit>). */
    ler_qtd_metodos(fp, arq);
    ler_metodos(fp, arq);

    /* 8) Atributos top-level da classe (SourceFile, InnerClasses, ...) */
    ler_qtd_atributos(fp, arq);
    ler_atributos(fp, arq);

    return arq;
}

/* -------------------------------------------------------------------------
 * IO bruto
 *
 * Tres primitivas para ler inteiros sem sinal de 1, 2 e 4 bytes
 * em big-endian (que e' o formato do .class por especificacao).
 *
 * Em hardware little-endian, ler dois ou quatro bytes direto com
 * fread() produziria o valor com os bytes invertidos - por isso
 * lemos byte a byte e re-empilhamos com shifts manuais.
 * -----------------------------------------------------------------------*/
u1 LeitorClasse::ler_u1(FILE* fp)
{
    /* 1 byte: nao precisa de byte-swap, le direto. */
    u1 b = 0;
    std::fread(&b, sizeof(u1), 1, fp);
    return b;
}

u2 LeitorClasse::ler_u2(FILE* fp)
{
    /* Spec e' sempre Big-Endian. Se a maquina e' LE precisamos
     * montar byte a byte (high byte primeiro); se ja' for BE basta
     * um fread direto. */
    if (maquina_little_endian) {
        u2 hi = ler_u1(fp);
        u2 lo = ler_u1(fp);
        return static_cast<u2>((hi << 8) | lo);
    }
    u2 v = 0;
    std::fread(&v, sizeof(u2), 1, fp);
    return v;
}

u4 LeitorClasse::ler_u4(FILE* fp)
{
    /* Mesma logica de ler_u2, mas com 4 bytes. b1 e' o mais
     * significativo (vem primeiro no stream). */
    if (maquina_little_endian) {
        u4 b1 = ler_u1(fp);
        u4 b2 = ler_u1(fp);
        u4 b3 = ler_u1(fp);
        u4 b4 = ler_u1(fp);
        return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
    }
    u4 v = 0;
    std::fread(&v, sizeof(u4), 1, fp);
    return v;
}

/* -------------------------------------------------------------------------
 * Cabecalho
 * -----------------------------------------------------------------------*/
void LeitorClasse::ler_magic(FILE* fp, ArquivoClasse* arq)
{
    arq->magic = ler_u4(fp);
}

bool LeitorClasse::magic_valido(ArquivoClasse* arq)
{
    return arq->magic == 0xCAFEBABE;
}

void LeitorClasse::ler_versoes(FILE* fp, ArquivoClasse* arq)
{
    arq->minor_version = ler_u2(fp);
    arq->major_version = ler_u2(fp);
}

bool LeitorClasse::versao_permitida(ArquivoClasse* arq, uint16_t major_max)
{
    return arq->major_version <= major_max;
}

/* -------------------------------------------------------------------------
 * Constant pool
 * -----------------------------------------------------------------------*/
void LeitorClasse::ler_pool_count(FILE* fp, ArquivoClasse* arq)
{
    arq->constant_pool_count = ler_u2(fp);
}

void LeitorClasse::ler_pool_constantes(FILE* fp, ArquivoClasse* arq)
{
    /* O constant_pool_count e' um valor "off-by-one" classico do .class:
     * se o pool tem N entradas reais, o count vale N+1 (a entrada 0 e'
     * sempre reservada/invalida na spec). Por isso lemos (count - 1)
     * entradas e armazenamos zero-indexadas internamente. Todo acesso
     * por indice externo precisa subtrair 1 (ver buscar_utf8). */
    u2 n = arq->constant_pool_count - 1;
    arq->constant_pool = static_cast<ConstantPoolInfo*>(
        std::malloc(sizeof(ConstantPoolInfo) * n));

    ConstantPoolInfo* cur = arq->constant_pool;
    for (u2 i = 0; i < n; ++i, ++cur) {
        /* Cada entrada comeca por um tag (u1) que define o tipo da
         * estrutura cp_info que vem em seguida. Despachamos pela tag. */
        cur->tag = ler_u1(fp);
        switch (cur->tag) {
            case ConstClass:
                cur->info.class_info = ler_const_class(fp);
                break;
            case ConstFieldRef:
                cur->info.field_ref_info = ler_const_fieldref(fp);
                break;
            case ConstMethodRef:
                cur->info.method_ref_info = ler_const_methodref(fp);
                break;
            case ConstInterfaceMethodRef:
                cur->info.interface_method_ref_info = ler_const_interfaceref(fp);
                break;
            case ConstStr:
                cur->info.str_info = ler_const_str(fp);
                break;
            case ConstInt:
                cur->info.int_info = ler_const_int(fp);
                break;
            case ConstFloat:
                cur->info.float_info = ler_const_float(fp);
                break;
            case ConstLong:
                cur->info.long_info = ler_const_long(fp);
                /* Quirk da spec: Long e Double ocupam DOIS slots no pool.
                 * O slot seguinte e' marcado como invalido (ConstNull)
                 * para que os indices subsequentes batam com o esperado. */
                (++cur)->tag = ConstNull;
                ++i;
                break;
            case ConstDouble:
                cur->info.double_info = ler_const_double(fp);
                (++cur)->tag = ConstNull;
                ++i;
                break;
            case ConstNameType:
                cur->info.name_type_info = ler_const_name_type(fp);
                break;
            case ConstUtf8:
                cur->info.utf8_info = ler_const_utf8(fp);
                break;
            default:
                std::cerr << "The .class file has an invalid tag in its constant pool.\n";
                std::exit(5);
        }
    }
}

ConstClassInfo LeitorClasse::ler_const_class(FILE* fp)
{
    ConstClassInfo c;
    c.name_index = ler_u2(fp);
    return c;
}

ConstFieldRefInfo LeitorClasse::ler_const_fieldref(FILE* fp)
{
    ConstFieldRefInfo c;
    c.class_index         = ler_u2(fp);
    c.name_and_type_index = ler_u2(fp);
    return c;
}

ConstMethodRefInfo LeitorClasse::ler_const_methodref(FILE* fp)
{
    ConstMethodRefInfo c;
    c.class_index         = ler_u2(fp);
    c.name_and_type_index = ler_u2(fp);
    return c;
}

ConstInterfaceMethodRefInfo LeitorClasse::ler_const_interfaceref(FILE* fp)
{
    ConstInterfaceMethodRefInfo c;
    c.class_index         = ler_u2(fp);
    c.name_and_type_index = ler_u2(fp);
    return c;
}

ConstStrInfo LeitorClasse::ler_const_str(FILE* fp)
{
    ConstStrInfo c;
    c.string_index = ler_u2(fp);
    return c;
}

ConstIntInfo LeitorClasse::ler_const_int(FILE* fp)
{
    ConstIntInfo c;
    c.bytes = ler_u4(fp);
    return c;
}

ConstFloatInfo LeitorClasse::ler_const_float(FILE* fp)
{
    ConstFloatInfo c;
    c.bytes = ler_u4(fp);
    return c;
}

ConstLongInfo LeitorClasse::ler_const_long(FILE* fp)
{
    ConstLongInfo c;
    c.high_bytes = ler_u4(fp);
    c.low_bytes  = ler_u4(fp);
    return c;
}

ConstDoubleInfo LeitorClasse::ler_const_double(FILE* fp)
{
    ConstDoubleInfo c;
    c.high_bytes = ler_u4(fp);
    c.low_bytes  = ler_u4(fp);
    return c;
}

ConstNameTypeInfo LeitorClasse::ler_const_name_type(FILE* fp)
{
    ConstNameTypeInfo c;
    c.name_index       = ler_u2(fp);
    c.descriptor_index = ler_u2(fp);
    return c;
}

ConstUtf8Info LeitorClasse::ler_const_utf8(FILE* fp)
{
    /* CONSTANT_Utf8: u2 length, seguido de `length` bytes.
     * NAO e' UTF-8 canonico, e' "modified UTF-8" (codifica '\0' e
     * caracteres BMP de forma especial). Para fins de impressao
     * simples basta ler como sequencia de bytes; ver
     * formatar_constante() no exibidor para o filtro de printable. */
    ConstUtf8Info c;
    c.length = ler_u2(fp);
    c.bytes  = static_cast<u1*>(std::malloc(sizeof(u1) * c.length));
    for (u2 i = 0; i < c.length; ++i) {
        c.bytes[i] = ler_u1(fp);
    }
    return c;
}

/* -------------------------------------------------------------------------
 * Estrutura principal
 * -----------------------------------------------------------------------*/
void LeitorClasse::ler_flags_acesso(FILE* fp, ArquivoClasse* arq)
{
    arq->access_flags = ler_u2(fp);
}

void LeitorClasse::ler_this_class(FILE* fp, ArquivoClasse* arq)
{
    arq->this_class = ler_u2(fp);
}

void LeitorClasse::ler_super_class(FILE* fp, ArquivoClasse* arq)
{
    arq->super_class = ler_u2(fp);
}

void LeitorClasse::ler_qtd_interfaces(FILE* fp, ArquivoClasse* arq)
{
    arq->interfaces_count = ler_u2(fp);
}

void LeitorClasse::ler_interfaces(FILE* fp, ArquivoClasse* arq)
{
    arq->interfaces = static_cast<u2*>(std::malloc(sizeof(u2) * arq->interfaces_count));
    for (u2 i = 0; i < arq->interfaces_count; ++i) {
        arq->interfaces[i] = ler_u2(fp);
    }
}

void LeitorClasse::ler_qtd_campos(FILE* fp, ArquivoClasse* arq)
{
    arq->fields_count = ler_u2(fp);
}

void LeitorClasse::ler_campos(FILE* fp, ArquivoClasse* arq)
{
    /* Cada field_info na spec:
     *   u2 access_flags
     *   u2 name_index       -> aponta p/ Utf8 no pool (nome do campo)
     *   u2 descriptor_index -> aponta p/ Utf8 (descritor de tipo, ex "I")
     *   u2 attributes_count
     *   attribute_info attributes[attributes_count]   (ex.: ConstantValue)
     */
    arq->fields = static_cast<FieldInfo*>(std::malloc(sizeof(FieldInfo) * arq->fields_count));
    for (u2 i = 0; i < arq->fields_count; ++i) {
        FieldInfo f;
        f.access_flags     = ler_u2(fp);
        f.name_index       = ler_u2(fp);
        f.descriptor_index = ler_u2(fp);
        f.attributes_count = ler_u2(fp);
        f.attributes       = static_cast<AttributeInfo*>(
            std::malloc(sizeof(AttributeInfo) * f.attributes_count));
        for (u2 j = 0; j < f.attributes_count; ++j) {
            f.attributes[j] = ler_atributo(fp, arq);
        }
        arq->fields[i] = f;
    }
}

void LeitorClasse::ler_qtd_metodos(FILE* fp, ArquivoClasse* arq)
{
    arq->methods_count = ler_u2(fp);
}

void LeitorClasse::ler_metodos(FILE* fp, ArquivoClasse* arq)
{
    arq->methods = static_cast<MethodInfo*>(std::malloc(sizeof(MethodInfo) * arq->methods_count));
    for (u2 i = 0; i < arq->methods_count; ++i) {
        MethodInfo* m = &arq->methods[i];
        m->access_flags     = ler_u2(fp);
        m->name_index       = ler_u2(fp);
        m->descriptor_index = ler_u2(fp);
        m->attributes_count = ler_u2(fp);
        m->attributes       = static_cast<AttributeInfo*>(
            std::malloc(sizeof(AttributeInfo) * m->attributes_count));
        for (u2 j = 0; j < m->attributes_count; ++j) {
            m->attributes[j] = ler_atributo(fp, arq);
        }
    }
}

void LeitorClasse::ler_qtd_atributos(FILE* fp, ArquivoClasse* arq)
{
    arq->attributes_count = ler_u2(fp);
}

void LeitorClasse::ler_atributos(FILE* fp, ArquivoClasse* arq)
{
    /* Le os atributos top-level da classe (ex: SourceFile, InnerClasses,
     * Deprecated). Atributos dentro de field/method/Code sao lidos pelas
     * proprias rotinas via ler_atributo(). */
    arq->attributes = static_cast<AttributeInfo*>(
        std::malloc(sizeof(AttributeInfo) * arq->attributes_count));
    for (u2 i = 0; i < arq->attributes_count; ++i) {
        arq->attributes[i] = ler_atributo(fp, arq);
    }
}

/* -------------------------------------------------------------------------
 * Atributos
 *
 * Atributo tem cabecalho generico (name_index + length) e payload
 * variavel. O dispatch e' feito por NOME (string UTF-8 do pool):
 * "Code", "ConstantValue", "Exceptions", etc.
 * -----------------------------------------------------------------------*/

/* Busca uma entrada Utf8 do pool a partir do indice 1-based usado
 * no .class. Subtrai 1 para acessar o array interno e valida com
 * assert que a tag realmente e' Utf8 (deve ser garantido pela spec). */
ConstUtf8Info LeitorClasse::buscar_utf8(u2 index, ArquivoClasse* arq)
{
    ConstantPoolInfo c = arq->constant_pool[index - 1];
    assert(c.tag == ConstUtf8);
    return c.info.utf8_info;
}

ConstValueAttribute LeitorClasse::ler_attr_constval(FILE* fp)
{
    ConstValueAttribute a;
    a.const_value_index = ler_u2(fp);
    return a;
}

ExceptionTable LeitorClasse::ler_exception_entry(FILE* fp)
{
    ExceptionTable e;
    e.start_pc   = ler_u2(fp);
    e.end_pc     = ler_u2(fp);
    e.handler_pc = ler_u2(fp);
    e.catch_type = ler_u2(fp);
    return e;
}

/*
 * ler_attr_code
 * -------------
 * Decodifica o atributo "Code" de um metodo. E' o atributo mais
 * complexo do .class porque carrega o bytecode propriamente dito,
 * a tabela de excecoes (try/catch) e ainda outros sub-atributos
 * aninhados (LineNumberTable, LocalVariableTable, StackMapTable...).
 *
 * Formato binario (JVM Spec 4.7.3):
 *
 *   Code_attribute {
 *     u2 attribute_name_index;   <-- ja lido por ler_atributo()
 *     u4 attribute_length;       <-- ja lido por ler_atributo()
 *     u2 max_stack;
 *     u2 max_locals;
 *     u4 code_length;
 *     u1 code[code_length];
 *     u2 exception_table_length;
 *     {  u2 start_pc;
 *        u2 end_pc;
 *        u2 handler_pc;
 *        u2 catch_type;
 *     } exception_table[exception_table_length];
 *     u2 attributes_count;
 *     attribute_info attributes[attributes_count];
 *   }
 */
CodeAttribute LeitorClasse::ler_attr_code(FILE* fp, ArquivoClasse* arq)
{
    CodeAttribute c;

    /* Passo 1 - max_stack (u2): profundidade maxima da pilha de
     * operandos durante a execucao deste metodo. O verificador da
     * JVM usa isso para dimensionar o stack do frame. */
    c.max_stack   = ler_u2(fp);

    /* Passo 2 - max_locals (u2): quantos slots de variavel local
     * o metodo usa, incluindo "this" (em metodos de instancia) e
     * os parametros. long/double consomem 2 slots cada. */
    c.max_locals  = ler_u2(fp);

    /* Passo 3 - code_length (u4): tamanho em BYTES do array de
     * bytecode que vem a seguir. A spec garante > 0 e < 65536. */
    c.code_length = ler_u4(fp);

    /* Passo 4 - alocar o buffer de bytecode e ler os bytes brutos
     * um a um. Nao decodificamos opcodes aqui; isso e' tarefa do
     * exibidor (escrever_bytecode) ou do executor (Operations).
     * Cada byte e' u1; sem byte-swap porque sao bytes individuais. */
    c.code = static_cast<u1*>(std::malloc(sizeof(u1) * c.code_length));
    for (u4 i = 0; i < c.code_length; ++i) {
        c.code[i] = ler_u1(fp);
    }

    /* Passo 5 - exception_table_length (u2): quantas entradas de
     * tratamento de excecao seguem. Cada entrada representa um
     * bloco try/catch do codigo Java original. */
    c.exception_table_length = ler_u2(fp);

    /* Passo 6 - alocar e ler cada exception_table entry. Estrutura
     * de cada uma (ver ler_exception_entry):
     *   start_pc   = PC inicial do bloco protegido (try {...)
     *   end_pc     = PC final (exclusivo) do bloco protegido
     *   handler_pc = PC para onde saltar ao capturar a excecao
     *   catch_type = indice no pool p/ classe da excecao (0 = "any") */
    c.exception_table = static_cast<ExceptionTable*>(
        std::malloc(sizeof(ExceptionTable) * c.exception_table_length));
    for (u2 i = 0; i < c.exception_table_length; ++i) {
        c.exception_table[i] = ler_exception_entry(fp);
    }

    /* Passo 7 - attributes_count (u2): numero de sub-atributos do
     * Code (atributos aninhados). Os tipicos sao:
     *   - LineNumberTable     (mapa bytecode -> linha do .java)
     *   - LocalVariableTable  (nomes/descritores das locais)
     *   - StackMapTable       (info para o verificador moderno) */
    c.attributes_count = ler_u2(fp);

    /* Passo 8 - alocar e ler cada sub-atributo recursivamente.
     * ler_atributo() faz o dispatch pelo nome de cada um (string
     * Utf8 no pool), exatamente como no nivel topo da classe. */
    c.attributes = static_cast<AttributeInfo*>(
        std::malloc(sizeof(AttributeInfo) * c.attributes_count));
    for (u2 i = 0; i < c.attributes_count; ++i) {
        c.attributes[i] = ler_atributo(fp, arq);
    }

    /* Passo 9 - devolver o CodeAttribute completamente preenchido.
     * A partir daqui o fp esta posicionado no byte logo depois deste
     * atributo, pronto para o proximo. */
    return c;
}

ExceptionsAttribute LeitorClasse::ler_attr_exceptions(FILE* fp)
{
    ExceptionsAttribute e;
    e.number_of_exceptions = ler_u2(fp);
    e.exception_index_table = static_cast<u2*>(std::malloc(sizeof(u2) * e.number_of_exceptions));
    for (u2 i = 0; i < e.number_of_exceptions; ++i) {
        e.exception_index_table[i] = ler_u2(fp);
    }
    return e;
}

Class LeitorClasse::ler_inner_class_entry(FILE* fp)
{
    Class c;
    c.inner_class_info_index   = ler_u2(fp);
    c.outer_class_info_index   = ler_u2(fp);
    c.inner_name_index         = ler_u2(fp);
    c.inner_class_access_flags = ler_u2(fp);
    return c;
}

InnerClassesAttribute LeitorClasse::ler_attr_inner(FILE* fp)
{
    InnerClassesAttribute a;
    a.number_of_classes = ler_u2(fp);
    a.classes = static_cast<Class*>(std::malloc(sizeof(Class) * a.number_of_classes));
    for (u2 i = 0; i < a.number_of_classes; ++i) {
        a.classes[i] = ler_inner_class_entry(fp);
    }
    return a;
}

SyntheticAttribute LeitorClasse::ler_attr_synthetic()
{
    return SyntheticAttribute();
}

SourceFileAttribute LeitorClasse::ler_attr_sourcefile(FILE* fp)
{
    SourceFileAttribute s;
    s.sourcefile_index = ler_u2(fp);
    return s;
}

LineNumberTable LeitorClasse::ler_line_entry(FILE* fp)
{
    LineNumberTable l;
    l.start_pc    = ler_u2(fp);
    l.line_number = ler_u2(fp);
    return l;
}

LineNumberTableAttribute LeitorClasse::ler_attr_linetable(FILE* fp)
{
    LineNumberTableAttribute a;
    a.line_number_table_length = ler_u2(fp);
    a.line_number_table = static_cast<LineNumberTable*>(
        std::malloc(sizeof(LineNumberTable) * a.line_number_table_length));
    for (u2 i = 0; i < a.line_number_table_length; ++i) {
        a.line_number_table[i] = ler_line_entry(fp);
    }
    return a;
}

LocalVariableTable LeitorClasse::ler_localvar_entry(FILE* fp)
{
    LocalVariableTable l;
    l.start_pc         = ler_u2(fp);
    l.length           = ler_u2(fp);
    l.name_index       = ler_u2(fp);
    l.descriptor_index = ler_u2(fp);
    l.index            = ler_u2(fp);
    return l;
}

LocalVariableTableAttribute LeitorClasse::ler_attr_localvar(FILE* fp)
{
    LocalVariableTableAttribute a;
    a.local_variable_table_length = ler_u2(fp);
    a.local_variable_table = static_cast<LocalVariableTable*>(
        std::malloc(sizeof(LocalVariableTable) * a.local_variable_table_length));
    for (u2 i = 0; i < a.local_variable_table_length; ++i) {
        a.local_variable_table[i] = ler_localvar_entry(fp);
    }
    return a;
}

DeprecatedAttribute LeitorClasse::ler_attr_deprecated()
{
    return DeprecatedAttribute();
}

/*
 * ler_atributo - le um atributo generico do .class.
 *
 * Todo atributo comeca com o mesmo cabecalho:
 *   u2 attribute_name_index;  -> aponta p/ Utf8 com o nome
 *   u4 attribute_length;      -> tamanho do payload em bytes
 *
 * O nome (string) determina qual parser especializado chamar.
 * Atributos desconhecidos (exceto StackMapTable) sao tratados
 * como erro fatal. StackMapTable e' pulado por simplicidade.
 */
AttributeInfo LeitorClasse::ler_atributo(FILE* fp, ArquivoClasse* arq)
{
    AttributeInfo a;
    a.attribute_name_index = ler_u2(fp);
    a.attribute_length     = ler_u4(fp);
    ConstUtf8Info nome = buscar_utf8(a.attribute_name_index, arq);

    /* dispatch por nome (predefinidos da spec). */
    if      (UtilClasse::utf8_igual_a(nome, "ConstantValue"))      a.info.const_value_info        = ler_attr_constval(fp);
    else if (UtilClasse::utf8_igual_a(nome, "Code"))               a.info.code_info               = ler_attr_code(fp, arq);
    else if (UtilClasse::utf8_igual_a(nome, "Exceptions"))         a.info.exceptions_info         = ler_attr_exceptions(fp);
    else if (UtilClasse::utf8_igual_a(nome, "InnerClasses"))       a.info.inner_classes_info      = ler_attr_inner(fp);
    else if (UtilClasse::utf8_igual_a(nome, "Synthetic"))          a.info.synthetic_info          = ler_attr_synthetic();
    else if (UtilClasse::utf8_igual_a(nome, "SourceFile"))         a.info.source_file_info        = ler_attr_sourcefile(fp);
    else if (UtilClasse::utf8_igual_a(nome, "LineNumberTable"))    a.info.line_number_table_info  = ler_attr_linetable(fp);
    else if (UtilClasse::utf8_igual_a(nome, "LocalVariableTable")) a.info.local_variable_table_info = ler_attr_localvar(fp);
    else if (UtilClasse::utf8_igual_a(nome, "Deprecated"))         a.info.deprecated_info         = ler_attr_deprecated();
    else if (UtilClasse::utf8_igual_a(nome, "StackMapTable")) {
        /* StackMapTable nao e' suportado em detalhe;
         * apenas pulamos os bytes pra deixar o ponteiro no lugar correto. */
        for (u4 i = 0; i < a.attribute_length; ++i) {
            ler_u1(fp);
        }
    }
    else {
        std::cerr << "The .class file has an invalid attribute.\n";
        std::exit(6);
    }
    return a;
}
