/*
 * tipos_basicos.hpp
 *
 * Reune os tipos e estruturas crus que aparecem no formato .class
 * conforme a especificacao JVM 8. Nada de logica aqui, so' descricao
 * de bytes: u1/u2/u4, enum dos tags do pool de constantes, structs
 * do pool, FieldInfo, MethodInfo, AttributeInfo e seus parentes.
 *
 * Tudo o que precisa montar ou interpretar um arquivo .class
 * (leitor, exibidor, executor) inclui este header.
 */
#ifndef TIPOS_BASICOS_HPP
#define TIPOS_BASICOS_HPP

#include <stdint.h>

/* ---------------------------------------------------------------------------
 * Aliases para tamanhos da especificacao
 * -------------------------------------------------------------------------*/
typedef uint8_t  u1;
typedef uint16_t u2;
typedef uint32_t u4;

/* ---------------------------------------------------------------------------
 * Discriminantes
 * -------------------------------------------------------------------------*/
enum ObjectType {
    CLASS_INSTANCE,
    STRING_INSTANCE,
    ARRAY
};
typedef enum ObjectType ObjectType;

enum ValueType {
    BOOLEAN,
    BYTE,
    CHAR,
    SHORT,
    INT,
    FLOAT,
    LONG,
    DOUBLE,
    RETURN_ADDR,
    REFERENCE,
    PADDING
};
typedef enum ValueType ValueType;

/* ---------------------------------------------------------------------------
 * Forward declarations
 * -------------------------------------------------------------------------*/
class ObjetoBase;
struct Value;
struct ConstantPoolInfo;
struct ConstClassInfo;
struct ConstFieldRefInfo;
struct ConstMethodRefInfo;
struct ConstInterfaceMethodRefInfo;
struct ConstStrInfo;
struct ConstIntInfo;
struct ConstFloatInfo;
struct ConstLongInfo;
struct ConstDoubleInfo;
struct ConstNameTypeInfo;
struct ConstUtf8Info;
struct FieldInfo;
struct AttributeInfo;
struct ConstValueAttribute;
struct ExceptionTable;
struct CodeAttribute;
struct ExceptionsAttribute;
struct Class;
struct InnerClassesAttribute;
struct SyntheticAttribute;
struct SourceFileAttribute;
struct LineNumberTable;
struct LineNumberTableAttribute;
struct LocalVariableTable;
struct LocalVariableTableAttribute;
struct DeprecatedAttribute;
struct MethodInfo;

/* ---------------------------------------------------------------------------
 * Value: celula generica usada na pilha de operandos e em variaveis locais.
 * `print_type` preserva o tipo de origem do dado para fins de exibicao
 * (por exemplo, BYTE empurrado por bipush mantem BYTE mesmo armazenado
 * em int_value); `type` indica em qual campo da uniao o dado vive.
 * -------------------------------------------------------------------------*/
struct Value {
    ValueType print_type;
    ValueType type;
    union {
        bool      boolean_value;
        int8_t    byte_value;
        uint8_t   char_value;
        int16_t   short_value;
        int32_t   int_value;
        float     float_value;
        int64_t   long_value;
        double    double_value;
        u4        return_address;
        ObjetoBase* object;
    } data;
};
typedef struct Value Value;

/* ---------------------------------------------------------------------------
 * Aliases de struct (necessarios em C, mantidos por compatibilidade)
 * -------------------------------------------------------------------------*/
typedef struct ConstantPoolInfo ConstantPoolInfo;
typedef struct ConstClassInfo ConstClassInfo;
typedef struct ConstFieldRefInfo ConstFieldRefInfo;
typedef struct ConstMethodRefInfo ConstMethodRefInfo;
typedef struct ConstInterfaceMethodRefInfo ConstInterfaceMethodRefInfo;
typedef struct ConstStrInfo ConstStrInfo;
typedef struct ConstIntInfo ConstIntInfo;
typedef struct ConstFloatInfo ConstFloatInfo;
typedef struct ConstLongInfo ConstLongInfo;
typedef struct ConstDoubleInfo ConstDoubleInfo;
typedef struct ConstNameTypeInfo ConstNameTypeInfo;
typedef struct ConstUtf8Info ConstUtf8Info;
typedef struct FieldInfo FieldInfo;
typedef struct AttributeInfo AttributeInfo;
typedef struct ConstValueAttribute ConstValueAttribute;
typedef struct ExceptionTable ExceptionTable;
typedef struct CodeAttribute CodeAttribute;
typedef struct ExceptionsAttribute ExceptionsAttribute;
typedef struct Class Class;
typedef struct InnerClassesAttribute InnerClassesAttribute;
typedef struct SyntheticAttribute SyntheticAttribute;
typedef struct SourceFileAttribute SourceFileAttribute;
typedef struct LineNumberTable LineNumberTable;
typedef struct LineNumberTableAttribute LineNumberTableAttribute;
typedef struct LocalVariableTable LocalVariableTable;
typedef struct LocalVariableTableAttribute LocalVariableTableAttribute;
typedef struct DeprecatedAttribute DeprecatedAttribute;
typedef struct MethodInfo MethodInfo;

/* ---------------------------------------------------------------------------
 * Flags de acesso para FieldInfo (mascaras da spec)
 * -------------------------------------------------------------------------*/
enum FIELD_FLAGS {
    FIELD_FLAG_ACC_PUBLIC    = 0x0001,
    FIELD_FLAG_ACC_PRIVATE   = 0x0002,
    FIELD_FLAG_ACC_PROTECTED = 0x0004,
    FIELD_FLAG_ACC_STATIC    = 0x0008,
    FIELD_FLAG_ACC_FINAL     = 0x0010,
    FIELD_FLAG_ACC_VOLATILE  = 0x0040,
    FIELD_FLAG_ACC_TRANSIENT = 0x0080,
    FIELD_FLAG_ACC_SYNTHETIC = 0x1000,
    FIELD_FLAG_ACC_ENUM      = 0x4000,
};

/* ---------------------------------------------------------------------------
 * FieldInfo / MethodInfo
 * -------------------------------------------------------------------------*/
struct FieldInfo {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    AttributeInfo* attributes;
};

struct MethodInfo {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
    AttributeInfo* attributes;
};

/* ---------------------------------------------------------------------------
 * Tags do pool de constantes (JVM 8 - Tabela 4.4)
 * -------------------------------------------------------------------------*/
typedef enum ConstType {
    ConstNull                = 0,  /* slot vazio apos Long/Double */
    ConstUtf8                = 1,
    ConstInt                 = 3,
    ConstFloat               = 4,
    ConstLong                = 5,
    ConstDouble              = 6,
    ConstClass               = 7,
    ConstStr                 = 8,
    ConstFieldRef            = 9,
    ConstMethodRef           = 10,
    ConstInterfaceMethodRef  = 11,
    ConstNameType            = 12
} ConstType;

/* ---------------------------------------------------------------------------
 * Estruturas internas de cada entrada do pool
 * -------------------------------------------------------------------------*/
struct ConstClassInfo               { u2 name_index; };
struct ConstFieldRefInfo            { u2 class_index; u2 name_and_type_index; };
struct ConstMethodRefInfo           { u2 class_index; u2 name_and_type_index; };
struct ConstInterfaceMethodRefInfo  { u2 class_index; u2 name_and_type_index; };
struct ConstStrInfo                 { u2 string_index; };
struct ConstIntInfo                 { u4 bytes; };
struct ConstFloatInfo               { u4 bytes; };
struct ConstLongInfo                { u4 high_bytes; u4 low_bytes; };
struct ConstDoubleInfo              { u4 high_bytes; u4 low_bytes; };
struct ConstNameTypeInfo            { u2 name_index; u2 descriptor_index; };
struct ConstUtf8Info                { u2 length; u1* bytes; };

struct ConstantPoolInfo {
    u1 tag;
    union {
        ConstClassInfo              class_info;
        ConstFieldRefInfo           field_ref_info;
        ConstMethodRefInfo          method_ref_info;
        ConstInterfaceMethodRefInfo interface_method_ref_info;
        ConstStrInfo                str_info;
        ConstIntInfo                int_info;
        ConstFloatInfo              float_info;
        ConstLongInfo               long_info;
        ConstDoubleInfo             double_info;
        ConstNameTypeInfo           name_type_info;
        ConstUtf8Info               utf8_info;
    } info;
};

/* ---------------------------------------------------------------------------
 * Atributos
 * -------------------------------------------------------------------------*/
struct ConstValueAttribute {
    u2 const_value_index;
};

struct ExceptionTable {
    u2 start_pc;
    u2 end_pc;
    u2 handler_pc;
    u2 catch_type;
};

struct CodeAttribute {
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1* code;
    u2 exception_table_length;
    ExceptionTable* exception_table;
    u2 attributes_count;
    AttributeInfo* attributes;
};

struct ExceptionsAttribute {
    u2 number_of_exceptions;
    u2* exception_index_table;
};

struct Class {
    u2 inner_class_info_index;
    u2 outer_class_info_index;
    u2 inner_name_index;
    u2 inner_class_access_flags;
};

struct InnerClassesAttribute {
    u2 number_of_classes;
    Class* classes;
};

struct SyntheticAttribute  { };
struct DeprecatedAttribute { };
struct DeprecatedAttribute1 { };

struct SourceFileAttribute {
    u2 sourcefile_index;
};

struct LineNumberTable {
    u2 start_pc;
    u2 line_number;
};

struct LineNumberTableAttribute {
    u2 line_number_table_length;
    LineNumberTable* line_number_table;
};

struct LocalVariableTable {
    u2 start_pc;
    u2 length;
    u2 name_index;
    u2 descriptor_index;
    u2 index;
};

struct LocalVariableTableAttribute {
    u2 local_variable_table_length;
    LocalVariableTable* local_variable_table;
};

struct AttributeInfo {
    u2 attribute_name_index;
    u4 attribute_length;
    union {
        ConstValueAttribute        const_value_info;
        CodeAttribute              code_info;
        ExceptionsAttribute        exceptions_info;
        InnerClassesAttribute      inner_classes_info;
        SyntheticAttribute         synthetic_info;
        SourceFileAttribute        source_file_info;
        LineNumberTableAttribute   line_number_table_info;
        LocalVariableTableAttribute local_variable_table_info;
        DeprecatedAttribute        deprecated_info;
        DeprecatedAttribute1       deprecated_info1;
    } info;
};

#endif
