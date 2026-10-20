#ifndef CONTAGEM_H
#define CONTAGEM_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
    int valor;
    size_t *contador;
} ElementoContagem;

/** Compara valores e incrementa uma vez o contador de a. Ambos devem ser validos. */
int contagem_comparar(const void *a, const void *b);
/** Gera quantidade insercoes em saida aberta, usando semente. Retorna 1 ou 0
 * em erro de memoria/escrita. Libera tudo que aloca; nao fecha saida.
 */
int contagem_gerar(FILE *saida, size_t quantidade, unsigned int semente);

#endif
