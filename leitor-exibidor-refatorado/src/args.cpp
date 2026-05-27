/*
 * args.cpp -- analise de argumentos CLI.
 * Aceita as flags -r= e -o= em qualquer ordem.
 */
#include "args.hpp"
#include <iostream>

using namespace std;

static const char* extrair_valor(const char* arg, const char* flag, size_t flag_len)
{
    if (strncmp(arg, flag, flag_len) == 0) {
        return arg + flag_len;
    }
    return nullptr;
}

const Parameters analisar_args(int argc, char* argv[])
{
    Parameters params;
    params.class_file_path  = nullptr;
    params.output_file_path = nullptr;

    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        const char* v;

        if ((v = extrair_valor(arg, "-r=", 3)) != nullptr) {
            params.class_file_path = const_cast<char*>(v);
        }
        else if ((v = extrair_valor(arg, "-o=", 3)) != nullptr) {
            params.output_file_path = const_cast<char*>(v);
        }
    }
    return params;
}

bool validar_parametros(const Parameters* params)
{
    if (!params->class_file_path || !params->output_file_path) {
        cerr << "Uso:\n"
             << "  bin -r=caminho/Classe.class -o=saida.txt\n";
        return false;
    }
    return true;
}
