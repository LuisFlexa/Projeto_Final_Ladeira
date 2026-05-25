/*
 * main.cpp
 *
 * Entrada do leitor-exibidor + executor de bytecodes JVM 8.
 *
 * Dois modos de operacao, escolhidos pela CLI (ver args.hpp):
 *   -r=<arq.class> -o=<saida.txt>   modo leitor: imprime a classe
 *                                   em texto estilo jclasslib
 *   -e=<arq.class>                  modo executor: roda a `main` da classe
 */
#include "args.hpp"
#include "leitor_classe.hpp"
#include "exibidor_classe.hpp"
#include "classe_estatica.hpp"
#include "area_metodos.hpp"
#include "executor.hpp"
#include "util_classe.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

/* Converte string in-place para maiusculas (usada na conferencia
 * de nome de classe em sistemas case-insensitive como Windows). */
void para_upper(std::string& s)
{
    for (char& c : s) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
}

/* Devolve o diretorio do caminho dado (ex.: "examples/X.class" -> "examples/")
 * ou string vazia se nao houver separador. */
std::string diretorio_de(const std::string& caminho)
{
    size_t pos = caminho.find_last_of("/\\");
    if (pos == std::string::npos) {
        return std::string();
    }
    return caminho.substr(0, pos + 1);
}

/* Garante que o nome da classe (this_class no constant pool) bate
 * com o nome do arquivo .class informado na CLI. Aborta se nao bater. */
void exigir_nome_compativel(const std::string& caminho_arq, const std::string& nome_no_pool)
{
    std::string a = caminho_arq;
    std::string b = nome_no_pool;
    para_upper(a);
    para_upper(b);
    if (!UtilClasse::conferir_nome_classe(a, b)) {
        std::cerr << "File name different from class name\n";
        std::exit(1);
    }
}

/* ------------------------------------------------------------------ */
/* Modo executor                                                       */
/* ------------------------------------------------------------------ */
int modo_executor(const Parameters& params)
{
    AreaMetodos& metodos = AreaMetodos::instancia();
    metodos.caminho_classes = diretorio_de(params.class_file_path);

    ClasseEstatica* rt = metodos.carregar_classe(params.class_file_path);

    /* Conferir nome da classe x nome do arquivo */
    std::string nome_no_pool = formatar_constante(
        rt->obter_arquivo()->constant_pool,
        rt->obter_arquivo()->this_class
    );
    exigir_nome_compativel(params.class_file_path, nome_no_pool);

    Executor& exec = Executor::instancia();
    if (!exec.metodo_existe(rt, "main", "([Ljava/lang/String;)V")) {
        std::cerr << "The file does not have a main method.\n";
        std::exit(2);
    }
    exec.executar(rt);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Modo leitor/exibidor                                                */
/* ------------------------------------------------------------------ */
int modo_leitor(const Parameters& params, FILE* fp)
{
    FILE* saida = std::fopen(params.output_file_path, "w+");
    if (saida == nullptr) {
        std::cerr << "Could not write to output file at \"" << params.output_file_path << "\"";
        return -1;
    }
    LeitorClasse& leitor = LeitorClasse::instancia();
    ArquivoClasse* arq = leitor.ler_arquivo(fp);
    escrever_arquivo(arq, saida);
    std::fclose(saida);
    return 0;
}

} /* namespace anonimo */

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

    int rc = params.execute
        ? modo_executor(params)
        : modo_leitor(params, fp);

    std::fclose(fp);
    return rc;
}
