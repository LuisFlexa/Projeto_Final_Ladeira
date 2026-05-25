/*
 * args.hpp
 * Analise dos argumentos de linha de comando aceitos pelo binario:
 *   -r=<arquivo>.class -o=<saida>.txt   modo leitor/exibidor
 *   -e=<arquivo>.class                  modo executor
 */
#ifndef ARGS_HPP
#define ARGS_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* class_file_path;
    char* output_file_path;
    bool  execute;
} Parameters;

const Parameters analisar_args(int argc, char* argv[]);
bool             validar_parametros(const Parameters* params);

#endif
