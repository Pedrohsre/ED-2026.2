#ifndef KRUSKAL_H
#define KRUSKAL_H

#include "grafo.h"
#include "lista.h"

/*
 * Kruskal (#25): árvore geradora mínima.
 *
 * As arestas entram em uma fila de prioridade com heap, ordenadas por peso. Para cada
 * aresta, em ordem crescente de peso, ela é escolhida se seus extremos ainda estão em
 * conjuntos diferentes no Union-Find (senão fecharia um ciclo).
 *
 * Se o grafo for desconexo, o resultado é uma floresta geradora mínima (uma árvore
 * por componente conexo).
 */

/**
 * Calcula a árvore (ou floresta) geradora mínima.
 *
 * @param grafo       Grafo de entrada. Não pode ser NULL e não é alterado.
 * @param peso_total  Recebe a soma dos pesos das arestas escolhidas. Não pode ser NULL.
 * @return            Lista de Aresta* escolhidas, em ordem crescente de peso, ou NULL
 *                    se faltar memória. As arestas pertencem ao GRAFO: a lista deve
 *                    ser liberada com lista_destruir(lista, NULL) (sem callback) e
 *                    só é válida enquanto o grafo existir. Há V-1 arestas se o grafo
 *                    for conexo; menos que isso indica floresta.
 */
Lista *kruskal_calcular(const Grafo *grafo, int *peso_total);

/**
 * Imprime as arestas escolhidas, o peso total e, se o grafo for desconexo, um aviso
 * com a quantidade de componentes.
 *
 * @param grafo       Grafo usado no cálculo. Não pode ser NULL.
 * @param escolhidas  Lista devolvida por kruskal_calcular. Não pode ser NULL.
 * @param peso_total  Peso total devolvido por kruskal_calcular.
 */
void kruskal_imprimir(const Grafo *grafo, const Lista *escolhidas, int peso_total);

#endif
