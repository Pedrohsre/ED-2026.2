#ifndef SAT_H
#define SAT_H

#include "formula.h"
#include "interpretacao.h"

/*
 * Backtracking do slide (Aula 21):
 *
 *   sat(F, I):
 *     se I => F  retorna true
 *     se I => ¬F retorna false
 *     escolhe xi não atribuído em I
 *     se sat(F, I ∪ {xi = true})  retorna true
 *     se sat(F, I ∪ {xi = false}) retorna true
 *     retorna false
 *
 * Cada chamada recursiva corresponde a um nó da árvore binária do slide.
 */

/**
 * Decide se a fórmula é satisfatível.
 *
 * Em vez de copiar a interpretação a cada chamada (I ∪ {xi = ...}), a mesma
 * interpretação é alterada e desfeita ao voltar (backtrack).
 *
 * @param formula        Fórmula CNF. Não pode ser NULL e não é alterada.
 * @param interpretacao  Interpretação parcial inicial (normalmente vazia), criada com
 *                       o mesmo número de variáveis da fórmula. Não pode ser NULL.
 *                       O chamador continua dono dela. Em caso de SAT ela guarda a
 *                       atribuição encontrada (variáveis NAO_ATRIBUIDO aceitam qualquer
 *                       valor). Em caso de UNSAT volta ao estado inicial.
 * @return               1 se a fórmula é satisfatível, 0 se é insatisfatível.
 */
int sat_resolver(const Formula *formula, Interpretacao *interpretacao);

#endif
