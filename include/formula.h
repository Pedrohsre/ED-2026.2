#ifndef FORMULA_H
#define FORMULA_H

#include "clausula.h"
#include "interpretacao.h"

/*
 * Fórmula em CNF (Conjunctive Normal Form): conjunção (AND) de cláusulas.
 * Variáveis são numeradas de 1 a num_variaveis.
 */

typedef struct Formula Formula;

/**
 * Cria uma fórmula sem cláusulas.
 *
 * @param num_variaveis  Quantidade de variáveis da fórmula.
 * @return               Ponteiro para a fórmula, ou NULL se faltar memória.
 *                       O chamador deve liberá-la com formula_destruir.
 */
Formula *formula_criar(int num_variaveis);

/**
 * Adiciona uma cláusula à fórmula.
 *
 * @param formula   Fórmula onde a cláusula será inserida. Não pode ser NULL.
 * @param clausula  Cláusula a inserir. Não pode ser NULL. A fórmula NÃO copia a
 *                  cláusula: em caso de sucesso ela passa a ser dona do ponteiro
 *                  (e o libera em formula_destruir).
 * @return          1 em caso de sucesso, 0 se faltar memória. Em caso de falha
 *                  o chamador continua dono da cláusula.
 */
int formula_adicionar_clausula(Formula *formula, Clausula *clausula);

/**
 * Devolve a quantidade de variáveis da fórmula.
 *
 * @param formula  Fórmula consultada. Não pode ser NULL.
 * @return         Número de variáveis.
 */
int formula_num_variaveis(const Formula *formula);

/**
 * Devolve a quantidade de cláusulas da fórmula.
 *
 * @param formula  Fórmula consultada. Não pode ser NULL.
 * @return         Número de cláusulas.
 */
int formula_num_clausulas(const Formula *formula);

/**
 * Avalia a fórmula sob uma interpretação parcial.
 *
 * @param formula        Fórmula a avaliar. Não pode ser NULL.
 * @param interpretacao  Interpretação parcial. Não pode ser NULL.
 * @return               AVALIACAO_FALSIFICADA se alguma cláusula é falsificada (I => ¬F);
 *                       AVALIACAO_SATISFEITA se todas as cláusulas são satisfeitas (I => F);
 *                       AVALIACAO_INDEFINIDA caso contrário.
 */
ResultadoAvaliacao formula_avaliar(const Formula *formula, const Interpretacao *interpretacao);

/**
 * Imprime a fórmula em formato DIMACS (útil para depurar o parser).
 *
 * @param formula  Fórmula a imprimir. Não pode ser NULL.
 */
void formula_imprimir(const Formula *formula);

/**
 * Libera a fórmula e todas as suas cláusulas.
 *
 * @param formula  Fórmula a destruir. Pode ser NULL (nada acontece).
 */
void formula_destruir(Formula *formula);

#endif
