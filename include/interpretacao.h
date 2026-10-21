#ifndef INTERPRETACAO_H
#define INTERPRETACAO_H

/*
 * Interpretação parcial: associa a cada variável x1..xn um valor
 * verdadeiro, falso ou "não atribuído" (slide "Partial Interpretation").
 *
 * Literais seguem a convenção do DIMACS: 3 = x3 e -3 = ¬x3.
 */

/* Os valores VERDADEIRO/FALSO são +1/-1 de propósito: o valor de um literal
 * negativo é o valor da variável com o sinal trocado. */
typedef enum {
    VALOR_NAO_ATRIBUIDO = 0,
    VALOR_VERDADEIRO    = 1,
    VALOR_FALSO         = -1
} Valor;

/* Resultado de avaliar uma cláusula ou fórmula sob uma interpretação parcial.
 * Corresponde aos três casos do slide: I => F, I => ¬F, ou depende de variáveis livres. */
typedef enum {
    AVALIACAO_INDEFINIDA,
    AVALIACAO_SATISFEITA,
    AVALIACAO_FALSIFICADA
} ResultadoAvaliacao;

typedef struct Interpretacao Interpretacao;

/**
 * Cria uma interpretação com todas as variáveis não atribuídas.
 *
 * @param num_variaveis  Quantidade de variáveis (numeradas de 1 a num_variaveis).
 * @return               Ponteiro para a interpretação, ou NULL se faltar memória.
 *                       O chamador deve liberá-la com interpretacao_destruir.
 */
Interpretacao *interpretacao_criar(int num_variaveis);

/**
 * Devolve a quantidade de variáveis da interpretação.
 *
 * @param interpretacao  Interpretação consultada. Não pode ser NULL.
 * @return               Número de variáveis.
 */
int interpretacao_num_variaveis(const Interpretacao *interpretacao);

/**
 * Devolve o valor de uma variável.
 *
 * @param interpretacao  Interpretação consultada. Não pode ser NULL.
 * @param variavel       Número da variável, entre 1 e interpretacao_num_variaveis.
 * @return               VALOR_VERDADEIRO, VALOR_FALSO ou VALOR_NAO_ATRIBUIDO.
 */
Valor interpretacao_obter(const Interpretacao *interpretacao, int variavel);

/**
 * Atribui um valor a uma variável (use VALOR_NAO_ATRIBUIDO para desfazer).
 *
 * @param interpretacao  Interpretação a alterar. Não pode ser NULL.
 * @param variavel       Número da variável, entre 1 e interpretacao_num_variaveis.
 * @param valor          Novo valor da variável.
 */
void interpretacao_atribuir(Interpretacao *interpretacao, int variavel, Valor valor);

/**
 * Devolve o valor de um literal, levando em conta o sinal.
 *
 * @param interpretacao  Interpretação consultada. Não pode ser NULL.
 * @param literal        Literal DIMACS (3 = x3, -3 = ¬x3), diferente de 0
 *                       e com módulo entre 1 e interpretacao_num_variaveis.
 * @return               VALOR_VERDADEIRO, VALOR_FALSO ou VALOR_NAO_ATRIBUIDO.
 */
Valor interpretacao_valor_literal(const Interpretacao *interpretacao, int literal);

/**
 * Libera a interpretação.
 *
 * @param interpretacao  Interpretação a destruir. Pode ser NULL (nada acontece).
 */
void interpretacao_destruir(Interpretacao *interpretacao);

#endif
