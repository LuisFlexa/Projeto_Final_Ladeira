#include "area_metodos.hpp"
#include "leitor_classe.hpp"
#include "exibidor_classe.hpp"
#include "executor.hpp"
#include "pilha_jvm.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>

namespace {
    /* Constroi o caminho candidato para um .class a partir do nome
     * qualificado vindo do bytecode. Se o nome ja' termina em
     * ".class" devolve como esta'; senao concatena caminho_base+nome+".class". */
    std::string resolver_caminho(const std::string& base, const std::string& nome)
    {
        const std::string ext = ".class";
        if (nome.size() >= ext.size() &&
            nome.compare(nome.size() - ext.size(), ext.size(), ext) == 0) {
            return nome;
        }
        return base + nome + ext;
    }
}

ClasseEstatica* AreaMetodos::carregar_classe(const std::string& nome)
{
    /* Cache: se ja' carregamos, devolve a mesma instancia */
    if (classes.count(nome) > 0) {
        return obter_classe(nome);
    }

    std::string caminho = resolver_caminho(caminho_classes, nome);

    FILE* fp = std::fopen(caminho.c_str(), "rb");
    if (fp == nullptr) {
        std::cerr << "Nao foi possivel abrir a classe: " << nome << "\n";
        std::exit(1);
    }

    LeitorClasse&   leitor    = LeitorClasse::instancia();
    ArquivoClasse*  arq       = leitor.ler_arquivo(fp);
    ClasseEstatica* runtime   = new ClasseEstatica(arq);
    inserir_classe(runtime);
    std::fclose(fp);

    /* Se a classe tem <clinit>, programa-o pra rodar antes do proximo
     * bytecode do chamador (i.e., empilha um Quadro novo no topo). */
    Executor& exec = Executor::instancia();
    if (exec.metodo_existe(runtime, "<clinit>", "()V")) {
        PilhaJVM& pilha = PilhaJVM::instancia();
        pilha.empilhar_quadro(new Quadro(runtime, "<clinit>", "()V"));
    }
    return runtime;
}

ClasseEstatica* AreaMetodos::obter_classe(const std::string& nome)
{
    auto it = classes.find(nome);
    return (it == classes.end()) ? nullptr : it->second;
}

bool AreaMetodos::inserir_classe(ClasseEstatica* rt)
{
    ArquivoClasse* cf = rt->obter_arquivo();
    const char* chave = formatar_constante(cf->constant_pool, cf->this_class);
    if (classes.count(chave) > 0) {
        return false;
    }
    classes[chave] = rt;
    return true;
}
