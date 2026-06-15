#ifndef CLASSFILE_H
#define CLASSFILE_H

#include <stdint.h>
#include <stdio.h>

typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;

#define CONSTANT_Utf8               1
#define CONSTANT_Integer            3
#define CONSTANT_Float              4
#define CONSTANT_Long               5
#define CONSTANT_Double             6
#define CONSTANT_Class              7
#define CONSTANT_String             8
#define CONSTANT_Fieldref           9
#define CONSTANT_Methodref          10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_NameAndType        12

typedef struct {
    u2 name_index;
} ConstClass_info;

typedef struct {
    u2 class_index;
    u2 name_and_type_index;
} ConstFieldref_info;

typedef struct {
    u2 class_index;
    u2 name_and_type_index;
} ConstMethodref_info;

typedef struct {
    u2 class_index;
    u2 name_and_type_index;
} ConstInterfaceMethodref_info;

typedef struct {
    u2 string_index;
} ConstString_info;

typedef struct {
    u4 bytes;
} ConstInteger_info;

typedef struct {
    u4 bytes;
} ConstFloat_info;

typedef struct {
    u4 high_bytes;
    u4 low_bytes;
} ConstLong_info;

typedef struct {
    u4 high_bytes;
    u4 low_bytes;
} ConstDouble_info;

typedef struct {
    u2 name_index;
    u2 descriptor_index;
} ConstNameAndType_info;

typedef struct {
    u2 length;
    u1 *bytes;
} ConstUtf8_info;

typedef struct {
    u1 tag;
    union {
        ConstClass_info class_info;
        ConstFieldref_info fieldref_info;
        ConstMethodref_info methodref_info;
        ConstInterfaceMethodref_info interface_methodref_info;
        ConstString_info string_info;
        ConstInteger_info integer_info;
        ConstFloat_info float_info;
        ConstLong_info long_info;
        ConstDouble_info double_info;
        ConstNameAndType_info name_and_type_info;
        ConstUtf8_info utf8_info;
    } info;
} cp_info;

typedef struct {
    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count;
    cp_info *constant_pool;
} ClassFile;

ClassFile* read_class_file(FILE *fp);
void free_class_file(ClassFile *cf);

#endif
