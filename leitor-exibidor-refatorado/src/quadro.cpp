#include "quadro.hpp"
#include "exibidor_classe.hpp"
#include "area_metodos.hpp"
#include "util_classe.hpp"
#include <cassert>
#include <cstdlib>
#include <iostream>

namespace {
    /* mascara da spec p/ saber se um metodo e' estatico */
    constexpr u2 METHOD_ACC_STATIC = 0x0008;

    void copiar_argumentos(std::map<uint32_t, Value>& destino,
                           const std::vector<Value>& args)
    {
        for (size_t i = 0; i < args.size(); ++i) {
            destino[static_cast<uint32_t>(i)] = args[i];
        }
    }
}

/* ----------------------------------------------------------------------- */
/* Construtores                                                            */
/* ----------------------------------------------------------------------- */
Quadro::Quadro(InstanciaClasse* recvr,
               ClasseEstatica* rt,
               std::string nome_metodo,
               std::string descritor,
               std::vector<Value> argumentos)
    : pc(0)
    , object(recvr)
{
    copiar_argumentos(local_variables, argumentos);

    MethodInfo* m = buscar_metodo(rt, nome_metodo, descritor);
    assert(m != nullptr);
    method = *m;
    /* este construtor e' para metodos de instancia */
    assert((method.access_flags & METHOD_ACC_STATIC) == 0);

    localizar_atributos();
}

Quadro::Quadro(ClasseEstatica* rt,
               std::string nome_metodo,
               std::string descritor,
               std::vector<Value> argumentos)
    : pc(0)
    , object(nullptr)
{
    copiar_argumentos(local_variables, argumentos);

    MethodInfo* m = buscar_metodo(rt, nome_metodo, descritor);
    assert(m != nullptr);
    method = *m;
    /* este construtor e' para metodos estaticos */
    assert((method.access_flags & METHOD_ACC_STATIC) != 0);

    localizar_atributos();
}

/* ----------------------------------------------------------------------- */
/* Pool de constantes da classe que define o metodo                        */
/* ----------------------------------------------------------------------- */
ConstantPoolInfo** Quadro::obter_pool_constantes()
{
    return &(classe_runtime->obter_arquivo()->constant_pool);
}

/* ----------------------------------------------------------------------- */
/* Variaveis locais                                                        */
/* ----------------------------------------------------------------------- */
Value Quadro::obter_local(uint32_t slot)
{
    if (slot >= code_attribute->max_locals) {
        std::cerr << "Acesso a variavel local inexistente (get slot=" << slot << ")\n";
        std::exit(1);
    }
    return local_variables[slot];
}

void Quadro::definir_local(Value valor, uint32_t slot)
{
    if (slot >= code_attribute->max_locals) {
        std::cerr << "Acesso a variavel local inexistente (set slot=" << slot << ")\n";
        std::exit(1);
    }
    local_variables[slot] = valor;
}

/* ----------------------------------------------------------------------- */
/* Pilha de operandos                                                      */
/* ----------------------------------------------------------------------- */
void Quadro::empilhar_operando(Value operando)
{
    operand_stack.push(operando);
}

Value Quadro::desempilhar_operando()
{
    if (operand_stack.empty()) {
        std::cerr << "Pilha de operandos vazia (IndexOutOfBoundsException)\n";
        std::exit(1);
    }
    Value v = operand_stack.top();
    operand_stack.pop();
    return v;
}

std::stack<Value> Quadro::copiar_pilha_operandos()
{
    return operand_stack;
}

void Quadro::carregar_pilha_operandos(std::stack<Value> backup)
{
    operand_stack = backup;
}

/* ----------------------------------------------------------------------- */
/* Acesso ao bytecode                                                      */
/* ----------------------------------------------------------------------- */
InstanciaClasse* Quadro::obter_objeto()
{
    return object;
}

u1* Quadro::obter_codigo(uint32_t pc_local)
{
    return code_attribute->code + pc_local;
}

u2 Quadro::max_locais()
{
    return code_attribute->max_locals;
}

u4 Quadro::tamanho_codigo()
{
    return code_attribute->code_length;
}

/* ----------------------------------------------------------------------- */
/* Resolucao do metodo subindo a hierarquia                                */
/* ----------------------------------------------------------------------- */
MethodInfo* Quadro::buscar_metodo(ClasseEstatica* rt,
                                  const std::string& nome,
                                  const std::string& descritor)
{
    AreaMetodos& metodos = AreaMetodos::instancia();
    ClasseEstatica* atual = rt;

    while (atual != nullptr) {
        ArquivoClasse* cf = atual->obter_arquivo();

        for (u2 i = 0; i < cf->methods_count; ++i) {
            MethodInfo* m = &cf->methods[i];
            std::string mname = formatar_constante(cf->constant_pool, m->name_index);
            std::string mdesc = formatar_constante(cf->constant_pool, m->descriptor_index);
            if (mname == nome && mdesc == descritor) {
                classe_runtime = atual;
                return m;
            }
        }

        if (cf->super_class == 0) {
            atual = nullptr;
        } else {
            std::string nome_super = formatar_constante(cf->constant_pool, cf->super_class);
            atual = metodos.obter_classe(nome_super);
        }
    }
    return nullptr;
}

/* ----------------------------------------------------------------------- */
/* Localiza Code/Exceptions dentro do MethodInfo                           */
/* ----------------------------------------------------------------------- */
void Quadro::localizar_atributos()
{
    ConstantPoolInfo* pool = *obter_pool_constantes();

    code_attribute       = nullptr;
    exceptions_attribute = nullptr;

    for (u2 i = 0; i < method.attributes_count; ++i) {
        AttributeInfo* attr = &method.attributes[i];
        ConstUtf8Info nome  = pool[attr->attribute_name_index - 1].info.utf8_info;

        if (UtilClasse::utf8_igual_a(nome, "Code")) {
            code_attribute = &attr->info.code_info;
        } else if (UtilClasse::utf8_igual_a(nome, "Exceptions")) {
            exceptions_attribute = &attr->info.exceptions_info;
        }

        if (code_attribute && exceptions_attribute) {
            break;
        }
    }
}
