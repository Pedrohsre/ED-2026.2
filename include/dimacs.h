#ifndef DIMACS_H
#define DIMACS_H

#include <stddef.h>
#include "formula.h"

/*
 * Leitor de arquivos DIMACS (.cnf).
 *
 * Formato aceito:
 *   - linhas começando com 'c' são comentários e são ignoradas;
 *   - uma linha "p cnf <variáveis> <cláusulas>" antes de qualquer cláusula;
 *   - cláusulas são sequências de inteiros terminadas em 0, lidas token a token
 *     (uma cláusula pode ocupar várias linhas, e várias cláusulas podem estar na mesma linha);
 *   - um '%' encerra a leitura (final dos benchmarks do SATLIB: "%" seguido de "0").
 */

/**
 * Lê um arquivo DIMACS e constrói a fórmula correspondente.
 *
 * Entradas inválidas NÃO derrubam o programa: arquivo inexistente, token que não é
 * número, literal fora de 1..variáveis, cláusula sem 0 final, quantidade de cláusulas
 * diferente da declarada ou falta da linha 'p cnf' resultam em NULL e em uma
 * mensagem em erro.
 *
 * @param caminho   Caminho do arquivo .cnf. Não pode ser NULL.
 * @param erro      Buffer onde a mensagem de erro é escrita. Não pode ser NULL.
 * @param tam_erro  Tamanho do buffer erro, em bytes.
 * @return          Ponteiro para a fórmula, ou NULL em caso de erro (arquivo ou
 *                  memória). O chamador é responsável por liberá-la com formula_destruir.
 */
Formula *dimacs_ler(const char *caminho, char *erro, size_t tam_erro);

#endif
