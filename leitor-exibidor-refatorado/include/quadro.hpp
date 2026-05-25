/*
 * quadro.hpp
 * Frame de execucao da JVM.
 *
 * Cada chamada de metodo (incluindo <init> e <clinit>) recebe seu
 * proprio Quadro contendo:
 *   - PC (program counter dentro do code attribute)
 *   - mapa de variaveis locais (slot -> Value)
 *   - pilha de operandos
 *   - ponteiros para o codigo do metodo e seu exception table
 *
 * O ciclo de vida de um Quadro e' controlado por PilhaJVM: o Executor
 * chama PilhaJVM::empilhar_quadro(new Quadro(...)) para invokes e
 * PilhaJVM::desempilhar_quadro() para returns.
 */
#ifndef QUADRO_HPP
#define QUADRO_HPP

#include "tipos_basicos.hpp"
#include "instancia_classe.hpp"
#include "classe_estatica.hpp"
#include <map>
#include <stack>
#include <string>
#include <vector>

class Quadro {
public:
    /* construtor para metodos de instancia (this != null) */
    Quadro(InstanciaClasse* recvr,
           ClasseEstatica* rt,
           std::string nome_metodo,
           std::string descritor,
           std::vector<Value> argumentos);

    /* construtor para metodos estaticos */
    Quadro(ClasseEstatica* rt,
           std::string nome_metodo,
           std::string descritor,
           std::vector<Value> argumentos = std::vector<Value>());

    ~Quadro() = default;

    /* acesso ao pool da classe que define o metodo em execucao */
    ConstantPoolInfo** obter_pool_constantes();

    /* variaveis locais */
    Value obter_local(uint32_t slot);
    void  definir_local(Value valor, uint32_t slot);

    /* pilha de operandos */
    void              empilhar_operando(Value operando);
    Value             desempilhar_operando();
    std::stack<Value> copiar_pilha_operandos();
    void              carregar_pilha_operandos(std::stack<Value> backup);

    InstanciaClasse* obter_objeto();
    u1*              obter_codigo(uint32_t pc_local);
    u2               max_locais();
    u4               tamanho_codigo();

    /* program counter publico (acesso direto p/ desempenho) */
    u4 pc;

private:
    /* busca o MethodInfo subindo a hierarquia de superclasses */
    MethodInfo* buscar_metodo(ClasseEstatica* rt,
                              const std::string& nome,
                              const std::string& descritor);

    /* localiza Code/Exceptions dentro do MethodInfo carregado */
    void localizar_atributos();

    ClasseEstatica*       classe_runtime;
    InstanciaClasse*      object;
    MethodInfo            method;
    CodeAttribute*        code_attribute;
    ExceptionsAttribute*  exceptions_attribute;

    std::map<uint32_t, Value> local_variables;
    std::stack<Value>         operand_stack;
};

#endif
