/*
 * tipos_runtime.hpp
 *
 * Modelo de VALOR em tempo de execucao da JVM. Enquanto tipos_basicos.hpp
 * descreve os bytes crus do arquivo .class, este header descreve o que vive
 * na pilha de operandos e nas variaveis locais durante a execucao: um valor
 * tipado (Valor), seu tipo primitivo (TipoValor) e o tipo de um objeto
 * referenciado no heap (TipoObjeto).
 *
 * Portado de JVM/headers/BaseType.hpp (apenas o trecho de runtime:
 * ObjectType, ValueType e Value). As estruturas do formato .class que
 * tambem viviam la' ja' estao em tipos_basicos.hpp e nao sao duplicadas aqui.
 *
 * Camadas separadas: este header NAO inclui tipos_basicos.hpp e nao deve ser
 * incluido por ele. Quem precisa de valores de runtime inclui este arquivo.
 */
#ifndef TIPOS_RUNTIME_HPP
#define TIPOS_RUNTIME_HPP

#include <stdint.h>

/* ---------------------------------------------------------------------------
 * Forward declaration
 *
 * Valor pode guardar um ponteiro para um Objeto no heap (referencia). A
 * definicao de Objeto vem no passo 3 (objeto.hpp); aqui basta declara-lo.
 * -------------------------------------------------------------------------*/
class Objeto;

/* ---------------------------------------------------------------------------
 * TipoObjeto (= ObjectType da referencia)
 *
 * Discrimina o tipo concreto de um Objeto no heap.
 * -------------------------------------------------------------------------*/
enum TipoObjeto {
    INSTANCIA_CLASSE,  // instancia de uma classe (objeto comum)
    INSTANCIA_STRING,  // instancia de String
    ARRANJO            // instancia de array
};

/* ---------------------------------------------------------------------------
 * TipoValor (= ValueType da referencia)
 *
 * Tipo de um valor que vive na pilha de operandos / variaveis locais.
 * PADDING marca a "segunda metade" de um valor de 64 bits (long/double),
 * que ocupam duas posicoes na pilha conforme a especificacao.
 * -------------------------------------------------------------------------*/
enum TipoValor {
    BOOLEANO,
    BYTE,
    CHAR,
    SHORT,
    INT,
    FLOAT,
    LONG,
    DOUBLE,
    ENDERECO_RETORNO,
    REFERENCIA,
    PADDING
};

/* ---------------------------------------------------------------------------
 * Valor (= Value da referencia)
 *
 * Um valor tipado de runtime. 'tipo_print' guarda o tipo logico original
 * para fins de exibicao/categoria, enquanto 'tipo' guarda o tipo efetivo do
 * dado armazenado na union 'dados'. A leitura correta do union depende de
 * 'tipo'.
 * -------------------------------------------------------------------------*/
struct Valor {
    TipoValor tipo_print;  // tipo logico para exibicao
    TipoValor tipo;        // tipo do dado efetivamente guardado em 'dados'
    union {
        bool     valor_booleano;
        int8_t   valor_byte;
        uint8_t  valor_char;
        int16_t  valor_short;
        int32_t  valor_int;
        float    valor_float;
        int64_t  valor_long;
        double   valor_double;
        uint32_t endereco_retorno;
        Objeto*  objeto;
    } dados;
};

/* ---------------------------------------------------------------------------
 * Helpers de construcao
 *
 * A referencia usava designated initializers (.type = ..., .data = {...}),
 * que sao extensao do g++ e nao fazem parte do C++14 padrao. Para manter
 * portabilidade e legibilidade, criamos fabricas inline. 'tipo_print' segue
 * 'tipo' por padrao; ajuste apos a construcao quando precisar diferencia-los.
 * -------------------------------------------------------------------------*/
inline Valor faz_valor_booleano(bool v) {
    Valor x; x.tipo = BOOLEANO; x.tipo_print = BOOLEANO; x.dados.valor_booleano = v; return x;
}
inline Valor faz_valor_byte(int8_t v) {
    Valor x; x.tipo = BYTE; x.tipo_print = BYTE; x.dados.valor_byte = v; return x;
}
inline Valor faz_valor_char(uint8_t v) {
    Valor x; x.tipo = CHAR; x.tipo_print = CHAR; x.dados.valor_char = v; return x;
}
inline Valor faz_valor_short(int16_t v) {
    Valor x; x.tipo = SHORT; x.tipo_print = SHORT; x.dados.valor_short = v; return x;
}
inline Valor faz_valor_int(int32_t v) {
    Valor x; x.tipo = INT; x.tipo_print = INT; x.dados.valor_int = v; return x;
}
inline Valor faz_valor_float(float v) {
    Valor x; x.tipo = FLOAT; x.tipo_print = FLOAT; x.dados.valor_float = v; return x;
}
inline Valor faz_valor_long(int64_t v) {
    Valor x; x.tipo = LONG; x.tipo_print = LONG; x.dados.valor_long = v; return x;
}
inline Valor faz_valor_double(double v) {
    Valor x; x.tipo = DOUBLE; x.tipo_print = DOUBLE; x.dados.valor_double = v; return x;
}
inline Valor faz_valor_endereco_retorno(uint32_t v) {
    Valor x; x.tipo = ENDERECO_RETORNO; x.tipo_print = ENDERECO_RETORNO; x.dados.endereco_retorno = v; return x;
}
inline Valor faz_valor_referencia(Objeto* v) {
    Valor x; x.tipo = REFERENCIA; x.tipo_print = REFERENCIA; x.dados.objeto = v; return x;
}
inline Valor faz_valor_padding() {
    Valor x; x.tipo = PADDING; x.tipo_print = PADDING; x.dados.valor_long = 0; return x;
}

#endif /* TIPOS_RUNTIME_HPP */
