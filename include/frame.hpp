/*
 * frame.hpp
 *
 * Frame (quadro) de execucao de um metodo da JVM: encapsula o estado local de
 * uma invocacao — pilha de operandos, variaveis locais, contador de programa
 * (pc) e o atributo Code do metodo. Cada chamada de metodo cria um Frame, que
 * vive na PilhaExecucao enquanto o metodo executa.
 *
 * Portado de JVM/headers/Frame.hpp + JVM/src/Frame.cpp.
 * Adaptacoes: BasicTypes.hpp (inexistente na referencia) -> tipos_runtime.hpp;
 * StaticClass -> ClasseEstatica, InstanceClass -> ClasseInstancia,
 * Value -> Valor, MethodArea -> AreaMetodos.
 */
#ifndef FRAME_HPP
#define FRAME_HPP

#include "tipos_runtime.hpp"
#include "classe_estatica.hpp"
#include "classe_instancia.hpp"
#include <map>
#include <stack>
#include <string>
#include <vector>

class Frame {
public:
    // Construtor para metodos de instancia (recebe o objeto `this`).
    Frame(ClasseInstancia* objeto, ClasseEstatica* classe_runtime,
          std::string nome_metodo, std::string descritor_metodo,
          std::vector<Valor> argumentos);

    // Construtor para metodos estaticos (sem objeto `this`).
    Frame(ClasseEstatica* classe_runtime, std::string nome_metodo,
          std::string descritor_metodo,
          std::vector<Valor> argumentos = std::vector<Valor>());

    ~Frame();

    // Endereco da constant pool da classe onde o metodo foi resolvido.
    ConstantPoolInfo** get_constant_pool();

    // Le o valor de uma variavel local pelo indice.
    Valor get_local_variable_value(uint32_t index);

    // Define o valor de uma variavel local pelo indice.
    void set_local_variable(Valor valor, uint32_t index);

    // Empilha um operando na pilha de operandos do frame.
    void push_operand_stack(Valor operando);

    // Desempilha e retorna o topo da pilha de operandos.
    Valor pop_operand_stack();

    // Copia (snapshot) da pilha de operandos.
    std::stack<Valor> copy_operand_stack();

    // Restaura a pilha de operandos a partir de um backup.
    void load_operand_stack(std::stack<Valor> backup);

    // Objeto `this` associado ao frame (NULL em metodos estaticos).
    ClasseInstancia* get_object();

    // Ponteiro para o bytecode no endereco dado (base do Code + address).
    u1* get_code(uint32_t address);

    // Contador de programa (posicao da instrucao atual).
    u4 pc;

    // Tamanho do vetor de variaveis locais (max_locals do Code).
    u2 get_local_variables_vector_size();

    // Tamanho total do bytecode do metodo (code_length do Code).
    u4 get_code_size();

private:
    // Busca o metodo (nome + descritor) na classe e, se nao achar, sobe a
    // hierarquia de superclasses. Efeito colateral: ajusta class_runtime para
    // a classe onde o metodo foi encontrado (define a pool correta).
    MethodInfo* obterMethodNamed(ClasseEstatica* classe_runtime,
                                 const std::string& nome,
                                 const std::string& descritor);

    // Localiza os atributos Code e Exceptions do metodo resolvido.
    void encontrarAttributes();

    ClasseEstatica* class_runtime;                 // classe onde o metodo vive
    ClasseInstancia* object;                       // objeto `this` (ou NULL)
    MethodInfo method;                             // metodo resolvido
    CodeAttribute* code_attribute;                 // atributo Code do metodo
    ExceptionsAttribute* exceptions_attribute;     // atributo Exceptions (ou NULL)
    std::map<uint32_t, Valor> local_variables;     // variaveis locais (indice -> valor)
    std::stack<Valor> operand_stack;               // pilha de operandos
};

#endif // FRAME_HPP
