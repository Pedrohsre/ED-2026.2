#ifndef CLAUSULA_H
#define CLAUSULA_H

#include "interpretacao.h"

/*
 * Cláusula: disjunção (OR) de literais. Literais seguem a convenção do DIMACS
 * (3 = x3, -3 = ¬x3). Uma cláusula sem literais é falsa.
 */

typedef struct Clausula Clausula;

/**
 * Cria uma cláusula vazia.
 *
 * @return  Ponteiro para a cláusula, ou NULL se faltar memória.
 *          O chamador deve liberá-la com clausula_destruir, a menos que
 *          a entregue a uma fórmula (formula_adicionar_clausula).
 */
Clausula *clausula_criar(void);

/**
 * Adiciona um literal à cláusula.
 *
 * @param clausula  Cláusula onde o literal será inserido. Não pode ser NULL.
 * @param literal   Literal DIMACS, diferente de 0. O valor é copiado.
 * @return          1 em caso de sucesso, 0 se faltar memória.
 */
int clausula_adicionar_literal(Clausula *clausula, int literal);

/**
 * Avalia a cláusula sob uma interpretação parcial.
 *
 * @param clausula       Cláusula a avaliar. Não pode ser NULL.
 * @param interpretacao  Interpretação parcial. Não pode ser NULL.
 * @return               AVALIACAO_SATISFEITA se algum literal é verdadeiro;
 *                       AVALIACAO_FALSIFICADA se todos os literais são falsos;
 *                       AVALIACAO_INDEFINIDA caso contrário.
 */
ResultadoAvaliacao clausula_avaliar(const Clausula *clausula, const Interpretacao *interpretacao);

/**
 * Imprime a cláusula no formato DIMACS (literais separados por espaço, terminando em 0),
 * sem quebra de linha. Tem a assinatura de FuncaoImpressao para ser usada com vetor_imprimir.
 *
 * @param clausula  Ponteiro para uma Clausula. Não pode ser NULL.
 */
void clausula_imprimir(const void *clausula);

/**
 * Libera a cláusula e seus literais. Tem a assinatura de FuncaoLiberacao
 * para ser usada com vetor_destruir.
 *
 * @param clausula  Ponteiro para uma Clausula. Pode ser NULL (nada acontece).
 */
void clausula_destruir(void *clausula);

#endif
