/*
 * area_metodos.cpp
 *
 * Implementacao de AreaMetodos. Portado de JVM/src/MethodArea.cpp.
 *
 * Nota sobre o clinit: na referencia, carregar_classe tambem empilhava o
 * frame de <clinit> da classe (usando Executor + PilhaExecucao/Frame). Esses
 * componentes so' aparecem nos passos 6/7, entao aqui a carga NAO executa o
 * clinit — ver "TODO clinit" abaixo, a ser religado ao final do passo 7.
 */
#include "area_metodos.hpp"
#include "leitor_classe.hpp"
#include "exibidor_classe.hpp"  // formatar_constante
#include "executor.hpp"         // Executor (verifica_metodo + clinit)
#include "frame.hpp"            // Frame do <clinit>
#include "pilha_execucao.hpp"   // PilhaExecucao
#include <cstdio>
#include <cstdlib>
#include <iostream>

using std::string;

ClasseEstatica* AreaMetodos::carregar_classe(const string& nome)
{
    // Caminho rapido: ja' carregada sob este nome (ex.: lookup do executor
    // pelo nome de this_class). Evita reler o arquivo do disco.
    if (classes.count(nome) > 0) {
        return obter_classe(nome);
    }

    // Normaliza para caminho de arquivo: class_path + nome + ".class",
    // a menos que 'nome' ja' termine em ".class".
    string caminho = nome;
    const string ext = ".class";
    if (caminho.size() < ext.size()
        || caminho.compare(caminho.size() - ext.size(), ext.size(), ext) != 0) {
        caminho = class_path + caminho + ext;
    }

    FILE* fp = fopen(caminho.c_str(), "rb");
    if (fp == NULL) {
        std::cerr << "No support for this class: " << nome << std::endl;
        exit(1);
    }

    ArquivoClasse* arquivo = LeitorClasse::instancia().ler_arquivo(fp);
    fclose(fp);

    ClasseEstatica* classe_runtime = new ClasseEstatica(arquivo);

    // Insere sob a chave canonica (nome de this_class). Se ja' existia uma
    // classe com essa this_class — caso da 2a chamada feita pelo caminho do
    // arquivo, cuja string nao bate com o caminho rapido acima — descarta a
    // recem-criada e devolve a ja' cacheada, garantindo ponteiro estavel.
    // (Na referencia esse caso devolvia o ponteiro novo: o cache nao pegava.)
    if (!inserir_classe(classe_runtime)) {
        string chave = formatar_constante(arquivo->constant_pool, arquivo->this_class);
        ClasseEstatica* existente = obter_classe(chave);
        delete classe_runtime;
        return existente;
    }

    // Se a classe declara <clinit> ()V (bloco de inicializacao estatica),
    // empilha seu Frame na PilhaExecucao para que o Executor o rode antes do
    // primeiro uso da classe. (Religado no passo 7, fecha o TODO do passo 5.)
    Executor& executor = Executor::instancia();
    if (executor.verifica_metodo(classe_runtime, "<clinit>", "()V")) {
        PilhaExecucao::instancia().empilhar_frame(
            new Frame(classe_runtime, "<clinit>", "()V"));
    }

    return classe_runtime;
}

ClasseEstatica* AreaMetodos::obter_classe(const string& nome)
{
    auto it = classes.find(nome);
    if (it == classes.end()) {
        return nullptr;
    }
    return it->second;
}

bool AreaMetodos::inserir_classe(ClasseEstatica* classe_runtime)
{
    ArquivoClasse* arquivo = classe_runtime->get_arquivo_classe();
    string chave = formatar_constante(arquivo->constant_pool, arquivo->this_class);

    if (classes.count(chave) > 0) {
        return false;
    }

    classes[chave] = classe_runtime;
    return true;
}
