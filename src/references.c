#include "references.h"
#include "constant_pool.h"
#include <stdlib.h>
#include <string.h>

u1 read_u1(FILE *fp) {
    u1 res;
    if (fread(&res, 1, 1, fp) != 1) return 0;
    return res;
}

u2 read_u2(FILE *fp) {
    u2 res = 0;
    res = (read_u1(fp) << 8) | read_u1(fp);
    return res;
}

u4 read_u4(FILE *fp) {
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

    if (read_constant_pool(cf, fp) != 0) {
        free(cf);
        return NULL;
    }

    return cf;
}

void free_class_file(ClassFile *cf) {
    if (!cf) return;
    free_constant_pool(cf);
    free(cf);
}
