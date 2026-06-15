#include "constant_pool.h"
#include <stdlib.h>
#include <string.h>

// Helper reading declarations from references.c
u1 read_u1(FILE *fp);
u2 read_u2(FILE *fp);
u4 read_u4(FILE *fp);

int read_constant_pool(ClassFile *cf, FILE *fp) {
    cf->constant_pool_count = read_u2(fp);
    cf->constant_pool = (cp_info*)malloc((cf->constant_pool_count) * sizeof(cp_info));
    if (!cf->constant_pool) return -1;
    memset(cf->constant_pool, 0, (cf->constant_pool_count) * sizeof(cp_info));

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
                i++;
                if(i < cf->constant_pool_count) {
                     cf->constant_pool[i].tag = 0; // null tag for second part of Long/Double
                }
                break;
            case CONSTANT_Double:
                cf->constant_pool[i].info.double_info.high_bytes = read_u4(fp);
                cf->constant_pool[i].info.double_info.low_bytes = read_u4(fp);
                i++;
                if(i < cf->constant_pool_count) {
                     cf->constant_pool[i].tag = 0; // null tag for second part of Long/Double
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
    return 0;
}

void free_constant_pool(ClassFile *cf) {
    if (!cf || !cf->constant_pool) return;
    for (int i = 1; i < cf->constant_pool_count; i++) {
        if (cf->constant_pool[i].tag == CONSTANT_Utf8) {
            free(cf->constant_pool[i].info.utf8_info.bytes);
        }
    }
    free(cf->constant_pool);
    cf->constant_pool = NULL;
}
