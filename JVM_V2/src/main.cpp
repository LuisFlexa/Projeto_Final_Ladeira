/*
 * main.cpp
 *
 * Entrada do leitor-exibidor de bytecodes JVM 8.
 *
 *   bin -r=<arq.class> -o=<saida.txt>
 *
 * Le' o arquivo .class indicado e gera um relatorio textual estilo
 * jclasslib no arquivo de saida.
 */
#include "args.hpp"
#include "leitor_classe.hpp"
#include "exibidor_classe.hpp"
#include <cstdio>
#include <iostream>

int main(int argc, char* argv[])
{
    std::cout << std::fixed;

    const Parameters params = analisar_args(argc, argv);
    if (!validar_parametros(&params)) {
        return -1;
    }

    FILE* fp = std::fopen(params.class_file_path, "rb");
    if (fp == nullptr) {
        std::cerr << "Could not find file at \"" << params.class_file_path << "\"\n";
        return -1;
    }

    FILE* saida = std::fopen(params.output_file_path, "w+");
    if (saida == nullptr) {
        std::cerr << "Could not write to output file at \"" << params.output_file_path << "\"\n";
        std::fclose(fp);
        return -1;
    }

    LeitorClasse& leitor = LeitorClasse::instancia();
    ArquivoClasse* arq = leitor.ler_arquivo(fp);
    escrever_arquivo(arq, saida);

    std::fclose(saida);
    std::fclose(fp);
    return 0;
}
