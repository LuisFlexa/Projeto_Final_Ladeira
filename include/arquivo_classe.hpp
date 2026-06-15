/*
 * arquivo_classe.hpp
 * Modelo em memoria de um arquivo .class de Java 8.
 *
 * A classe ArquivoClasse e' um container "burro": guarda exatamente
 * os mesmos campos previstos pela secao 4.1 da spec da JVM,
 * preenchidos pelo LeitorClasse e consumidos pelo ExibidorClasse e
 * pelo Executor de bytecodes.
 */
#ifndef ARQUIVO_CLASSE_HPP
#define ARQUIVO_CLASSE_HPP

#include "tipos_basicos.hpp"

class ArquivoClasse {
public:
    ArquivoClasse() = default;

    /* cabecalho do arquivo */
    u4 magic;
    u2 minor_version;
    u2 major_version;

    /* pool de constantes (1-indexado na spec) */
    u2 constant_pool_count;
    ConstantPoolInfo* constant_pool;

    /* informacoes da propria classe */
    u2 access_flags;
    u2 this_class;
    u2 super_class;

    /* tabelas de interfaces, campos, metodos, atributos */
    u2 interfaces_count;
    u2* interfaces;

    u2 fields_count;
    FieldInfo* fields;

    u2 methods_count;
    MethodInfo* methods;

    u2 attributes_count;
    AttributeInfo* attributes;
};

#endif
