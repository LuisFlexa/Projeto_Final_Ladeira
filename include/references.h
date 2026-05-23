#ifndef REFERENCES_H
#define REFERENCES_H

#include "ClassFile.h"
#include <stdio.h>

ClassFile* read_class_file(FILE *fp);
void free_class_file(ClassFile *cf);

#endif
