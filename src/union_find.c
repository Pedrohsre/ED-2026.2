#include <stdlib.h>
#include "union_find.h"

struct UnionFind {
    int *pai;     /* pai[i] == i quando i é a raiz do seu conjunto */
    int *rank;    /* limite superior da altura da árvore cuja raiz é i */
};

UnionFind *union_find_criar(int num_elementos) {
    if (num_elementos < 1) return NULL;

    UnionFind *uf = malloc(sizeof *uf);
    if (uf == NULL) return NULL;

    uf->pai = malloc(num_elementos * sizeof *uf->pai);
    uf->rank = calloc(num_elementos, sizeof *uf->rank);   /* calloc: todos começam com rank 0 */
    if (uf->pai == NULL || uf->rank == NULL) {
        free(uf->pai);
        free(uf->rank);
        free(uf);
        return NULL;
    }

    for (int i = 0; i < num_elementos; i++)
        uf->pai[i] = i;
    return uf;
}

int union_find_encontrar(UnionFind *uf, int elemento) {
    if (uf->pai[elemento] != elemento) {
        /* Compressão de caminho: ligar direto à raiz deixa as próximas buscas quase O(1) */
        uf->pai[elemento] = union_find_encontrar(uf, uf->pai[elemento]);
    }
    return uf->pai[elemento];
}

int union_find_unir(UnionFind *uf, int a, int b) {
    int raiz_a = union_find_encontrar(uf, a);
    int raiz_b = union_find_encontrar(uf, b);
    if (raiz_a == raiz_b) return 0;

    /* União por rank: a árvore mais baixa vira filha da mais alta, para que a
     * altura só aumente quando as duas têm a mesma altura */
    if (uf->rank[raiz_a] < uf->rank[raiz_b]) {
        int troca = raiz_a;
        raiz_a = raiz_b;
        raiz_b = troca;
    }
    uf->pai[raiz_b] = raiz_a;
    if (uf->rank[raiz_a] == uf->rank[raiz_b])
        uf->rank[raiz_a]++;
    return 1;
}

void union_find_destruir(UnionFind *uf) {
    if (uf == NULL) return;
    free(uf->pai);
    free(uf->rank);
    free(uf);
}
