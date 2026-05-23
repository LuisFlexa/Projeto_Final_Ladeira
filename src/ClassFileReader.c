#include "ClassFileReader.h"
#include <stdlib.h>
#include <string.h>

static u1 read_u1(FILE *fp) {
    u1 res;
    if (fread(&res, 1, 1, fp) != 1) return 0;
    return res;
}

static u2 read_u2(FILE *fp) {
    u2 res = 0;
    res = (read_u1(fp) << 8) | read_u1(fp);
    return res;
}

static u4 read_u4(FILE *fp) {
    u4 res = 0;
    res = (read_u1(fp) << 24) | (read_u1(fp) << 16) | (read_u1(fp) << 8) | read_u1(fp);
    return res;
}

ClassFile* read_class_file(FILE *fp) {
    ClassFile *cf = (ClassFile*)malloc(sizeof(ClassFile));
    if (!cf) return NULL;
    memset(cf, 0, sizeof(ClassFile));

    cf->magic = read_u4(fp);
    if (cf->magic != 0xCAFEBABE) {
        printf("Error: Invalid magic number.\n");
        free(cf);
        return NULL;
    }

    cf->minor_version = read_u2(fp);
    cf->major_version = read_u2(fp);
    cf->constant_pool_count = read_u2(fp);

    cf->constant_pool = (cp_info*)malloc((cf->constant_pool_count) * sizeof(cp_info));
    for (int i = 1; i < cf->constant_pool_count; i++) {
        cf->constant_pool[i].tag = read_u1(fp);
        switch (cf->constant_pool[i].tag) {
            case CONSTANT_Class:
                cf->constant_pool[i].info.class_info.name_index = read_u2(fp);
                break;
            case CONSTANT_Fieldref:
                cf->constant_pool[i].info.fieldref_info.class_index = read_u2(fp);
                cf->constant_pool[i].info.fieldref_info.name_and_type_index = read_u2(fp);
                break;
            case CONSTANT_Methodref:
                cf->constant_pool[i].info.methodref_info.class_index = read_u2(fp);
                cf->constant_pool[i].info.methodref_info.name_and_type_index = read_u2(fp);
                break;
            case CONSTANT_InterfaceMethodref:
                cf->constant_pool[i].info.interface_methodref_info.class_index = read_u2(fp);
                cf->constant_pool[i].info.interface_methodref_info.name_and_type_index = read_u2(fp);
                break;
            case CONSTANT_String:
                cf->constant_pool[i].info.string_info.string_index = read_u2(fp);
                break;
            case CONSTANT_Integer:
                cf->constant_pool[i].info.integer_info.bytes = read_u4(fp);
                break;
            case CONSTANT_Float:
                cf->constant_pool[i].info.float_info.bytes = read_u4(fp);
                break;
            case CONSTANT_Long:
                cf->constant_pool[i].info.long_info.high_bytes = read_u4(fp);
                cf->constant_pool[i].info.long_info.low_bytes = read_u4(fp);
                i++; // Long and Double take 2 entries
                if(i < cf->constant_pool_count) {
                     cf->constant_pool[i].tag = 0; // null tag for second part
                }
                break;
            case CONSTANT_Double:
                cf->constant_pool[i].info.double_info.high_bytes = read_u4(fp);
                cf->constant_pool[i].info.double_info.low_bytes = read_u4(fp);
                i++;
                if(i < cf->constant_pool_count) {
                     cf->constant_pool[i].tag = 0; // null tag
                }
                break;
            case CONSTANT_NameAndType:
                cf->constant_pool[i].info.name_and_type_info.name_index = read_u2(fp);
                cf->constant_pool[i].info.name_and_type_info.descriptor_index = read_u2(fp);
                break;
            case CONSTANT_Utf8:
                cf->constant_pool[i].info.utf8_info.length = read_u2(fp);
                cf->constant_pool[i].info.utf8_info.bytes = (u1*)malloc(cf->constant_pool[i].info.utf8_info.length + 1);
                fread(cf->constant_pool[i].info.utf8_info.bytes, 1, cf->constant_pool[i].info.utf8_info.length, fp);
                cf->constant_pool[i].info.utf8_info.bytes[cf->constant_pool[i].info.utf8_info.length] = '\0';
                break;
            default:
                printf("Error: Unknown constant pool tag %d at index %d\n", cf->constant_pool[i].tag, i);
                break;
        }
    }

    return cf;
}

void free_class_file(ClassFile *cf) {
    if (!cf) return;
    for (int i = 1; i < cf->constant_pool_count; i++) {
        if (cf->constant_pool[i].tag == CONSTANT_Utf8) {
            free(cf->constant_pool[i].info.utf8_info.bytes);
        }
    }
    free(cf->constant_pool);
    free(cf);
}
