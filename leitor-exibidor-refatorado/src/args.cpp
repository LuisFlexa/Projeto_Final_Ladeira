/*
 * args.cpp -- analise de argumentos CLI.
 * Aceita as flags -r= -o= -e= em qualquer ordem.
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
    params.execute          = false;

    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        const char* v;

        if ((v = extrair_valor(arg, "-r=", 3)) != nullptr) {
            params.execute         = false;
            params.class_file_path = const_cast<char*>(v);
        }
        else if ((v = extrair_valor(arg, "-e=", 3)) != nullptr) {
            params.execute         = true;
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
    bool faltou_classe = !params->class_file_path;
    bool faltou_saida_p_leitor = !params->execute && !params->output_file_path;

    if (faltou_classe || faltou_saida_p_leitor) {
        cerr << "Uso:\n"
             << "  Leitor/Exibidor:  bin -r=caminho/Classe.class -o=saida.txt\n"
             << "  Executor:         bin -e=caminho/Classe.class\n";
        return false;
    }
    return true;
}
