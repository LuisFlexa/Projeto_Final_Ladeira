/*
 * main.cpp
 *
 * Entrada do binario JVM_V2. Dois modos:
 *
 *   bin -r=<arq.class> -o=<saida.txt>   leitor/exibidor (regressao intacta)
 *   bin -e=<arq.class>                  execucao de bytecode
 */
#include "args.hpp"
#include "leitor_classe.hpp"
#include "exibidor_classe.hpp"
#include "area_metodos.hpp"
#include "classe_estatica.hpp"
#include "executor.hpp"
#include <cstdio>
#include <iostream>
#include <string>

// Extrai o diretorio do caminho do arquivo (ex.: "examples/Sum.class" -> "examples/").
static std::string diretorio_de(const char* caminho)
{
    std::string s(caminho);
    for (char& c : s) {
        if (c == '\\') c = '/';
    }
    const size_t pos = s.rfind('/');
    if (pos == std::string::npos) return "";
    return s.substr(0, pos + 1);
}

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

    // Modo leitor: abrir o arquivo de saida ANTES de ler a classe para que,
    // em caso de exit() antecipado (ex.: magic invalido), o arquivo exista
    // e fique vazio — comportamento esperado pelo suite de regressao.
    FILE* saida = nullptr;
    if (!params.execute) {
        saida = std::fopen(params.output_file_path, "w+");
        if (saida == nullptr) {
            std::cerr << "Could not write to output file at \""
                      << params.output_file_path << "\"\n";
            std::fclose(fp);
            return -1;
        }
    }

    LeitorClasse& leitor = LeitorClasse::instancia();
    ArquivoClasse* arq   = leitor.ler_arquivo(fp);
    std::fclose(fp);

    if (params.execute) {
        AreaMetodos& area = AreaMetodos::instancia();
        area.class_path   = diretorio_de(params.class_file_path);

        // Constroi a ClasseEstatica diretamente e registra no cache sem
        // empilhar <clinit> aqui — executar_metodos empilhara main + <clinit>
        // na ordem correta (evita empilhamento duplo de <clinit>).
        ClasseEstatica* c = new ClasseEstatica(arq);
        area.registrar_classe(c);

        Executor::instancia().executar_metodos(c);
    } else {
        escrever_arquivo(arq, saida);
        std::fclose(saida);
    }

    return 0;
}
