/*
 * frame.cpp
 *
 * Implementacao de Frame. Portado de JVM/src/Frame.cpp.
 */
#include "frame.hpp"
#include "area_metodos.hpp"
#include "exibidor_classe.hpp"  // formatar_constante
#include "util_classe.hpp"      // UtilClasse::utf8_igual_a
#include <cassert>
#include <cstdlib>
#include <iostream>

using std::string;
using std::vector;

// Construtor para metodo de instancia.
Frame::Frame(ClasseInstancia* objeto, ClasseEstatica* classe_runtime,
             string nome_metodo, string descritor_metodo, vector<Valor> argumentos)
    : pc(0), object(objeto)
{
    // Copia os argumentos para as variaveis locais [0..n).
    for (int i = 0; i < (signed)argumentos.size(); i++) {
        local_variables[i] = argumentos[i];
    }

    // Localiza o metodo pelo nome e descritor (define class_runtime).
    MethodInfo* metodo = obterMethodNamed(classe_runtime, nome_metodo, descritor_metodo);
    assert(metodo != NULL);
    method = *metodo;

    // Garante que NAO e' um metodo estatico.
    assert((method.access_flags & FIELD_FLAG_ACC_STATIC) == 0);

    // Carrega os atributos do metodo (Code, Exceptions).
    encontrarAttributes();
}

// Construtor para metodo estatico.
Frame::Frame(ClasseEstatica* classe_runtime, string nome_metodo,
             string descritor_metodo, vector<Valor> argumentos)
    : pc(0), object(NULL)
{
    // Copia os argumentos para as variaveis locais [0..n).
    for (int i = 0; i < (signed)argumentos.size(); i++) {
        local_variables[i] = argumentos[i];
    }

    // Localiza o metodo pelo nome e descritor (define class_runtime).
    MethodInfo* metodo = obterMethodNamed(classe_runtime, nome_metodo, descritor_metodo);
    assert(metodo != NULL);
    method = *metodo;

    // Garante que o metodo E' estatico.
    assert((method.access_flags & FIELD_FLAG_ACC_STATIC) != 0);

    // Carrega os atributos do metodo (Code, Exceptions).
    encontrarAttributes();
}

Frame::~Frame()
{
}

// Endereco da constant pool da classe onde o metodo foi resolvido.
ConstantPoolInfo** Frame::get_constant_pool()
{
    return &(class_runtime->get_arquivo_classe()->constant_pool);
}

// Le o valor de uma variavel local pelo indice.
Valor Frame::get_local_variable_value(uint32_t index)
{
    if (index >= code_attribute->max_locals) {
        std::cerr << "Trying to get inexistent local variable" << std::endl;
        exit(1);
    }
    return local_variables[index];
}

// Define o valor de uma variavel local pelo indice.
void Frame::set_local_variable(Valor valor, uint32_t index)
{
    if (index >= code_attribute->max_locals) {
        std::cerr << "Trying to set inexistent local variable" << std::endl;
        exit(1);
    }
    local_variables[index] = valor;
}

// Empilha um operando.
void Frame::push_operand_stack(Valor operando)
{
    operand_stack.push(operando);
}

// Desempilha e retorna o topo da pilha de operandos.
Valor Frame::pop_operand_stack()
{
    if (operand_stack.size() == 0) {
        std::cerr << "IndexOutOfBoundsException" << std::endl;
        exit(1);
    }

    Valor topo = operand_stack.top();
    operand_stack.pop();
    return topo;
}

// Copia (snapshot) da pilha de operandos.
std::stack<Valor> Frame::copy_operand_stack()
{
    return operand_stack;
}

// Restaura a pilha de operandos a partir de um backup.
void Frame::load_operand_stack(std::stack<Valor> backup)
{
    operand_stack = backup;
}

// Objeto `this` associado ao frame (NULL em metodos estaticos).
ClasseInstancia* Frame::get_object()
{
    return object;
}

// Ponteiro para o bytecode no endereco dado.
u1* Frame::get_code(uint32_t address)
{
    return code_attribute->code + address;
}

// Busca o metodo na classe ou na hierarquia (super_class) por nome+descritor.
MethodInfo* Frame::obterMethodNamed(ClasseEstatica* classe_runtime,
                                    const string& nome, const string& descritor)
{
    AreaMetodos& area_metodos = AreaMetodos::instancia();
    ClasseEstatica* classe_atual = classe_runtime;
    MethodInfo* metodo;

    while (classe_atual != NULL) {
        ArquivoClasse* arquivo = classe_atual->get_arquivo_classe();

        for (int i = 0; i < arquivo->methods_count; i++) {
            metodo = &(arquivo->methods[i]);
            string nome_metodo = formatar_constante(arquivo->constant_pool, metodo->name_index);
            string desc_metodo = formatar_constante(arquivo->constant_pool, metodo->descriptor_index);

            if (nome_metodo == nome && desc_metodo == descritor) {
                // Efeito colateral: registra a classe onde o metodo foi achado,
                // para que get_constant_pool() use a pool correta.
                this->class_runtime = classe_atual;
                return metodo;
            }
        }

        // Sem superclasse: encerra. Senao, continua a busca na superclasse.
        if (arquivo->super_class == 0) {
            classe_atual = NULL;
        } else {
            string nome_super = formatar_constante(arquivo->constant_pool, arquivo->super_class);
            classe_atual = area_metodos.obter_classe(nome_super);
        }
    }

    return NULL;
}

// Localiza os atributos Code e Exceptions do metodo resolvido.
void Frame::encontrarAttributes()
{
    ConstantPoolInfo* constant_pool = *get_constant_pool();
    code_attribute = NULL;
    exceptions_attribute = NULL;

    for (int i = 0; i < method.attributes_count; i++) {
        AttributeInfo* attr = &(method.attributes[i]);
        ConstUtf8Info nome_attr = constant_pool[attr->attribute_name_index - 1].info.utf8_info;

        if (UtilClasse::utf8_igual_a(nome_attr, "Code")) {
            code_attribute = &(attr->info.code_info);
            if (exceptions_attribute != NULL)
                break;
        } else if (UtilClasse::utf8_igual_a(nome_attr, "Exceptions")) {
            exceptions_attribute = &(attr->info.exceptions_info);
            if (code_attribute != NULL)
                break;
        }
    }
}

// Tamanho do vetor de variaveis locais (max_locals do Code).
u2 Frame::get_local_variables_vector_size()
{
    return code_attribute->max_locals;
}

// Tamanho total do bytecode do metodo (code_length do Code).
u4 Frame::get_code_size()
{
    return code_attribute->code_length;
}
