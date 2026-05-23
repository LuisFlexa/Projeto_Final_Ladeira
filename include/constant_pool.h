#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H

#include "ClassFile.h"
#include <stdio.h>

int read_constant_pool(ClassFile *cf, FILE *fp);
void free_constant_pool(ClassFile *cf);

#endif
