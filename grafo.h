#ifndef GRAFO_H
#define GRAFO_H

#include <stddef.h>
#include "lista.h"

/*
 * Grafo não direcionado e ponderado (#24), com vértices numerados de 0 a V-1.
 * Usado pelo Kruskal (#25) e pelo Dijkstra (#22).
 *
 * Duas representações são mantidas ao mesmo tempo:
 *   - lista de arestas (Kruskal): grafo_arestas;
 *   - lista de adjacência (Dijkstra): grafo_adjacentes, uma lista por vértice.
 *
 * Cada aresta é UM ÚNICO objeto Aresta, compartilhado entre as duas representações:
 * a lista de arestas é a dona dele, e as listas de adjacência só guardam ponteiros
 * para o mesmo objeto. Uma aresta u-v aparece na lista de adjacência de u e na de v;
 * um laço (u == v) aparece uma só vez, na lista de u. Para saber o vizinho a partir
 * de um vértice, use aresta_outro_extremo.
 *
 * Formato do arquivo de entrada (grafo_ler), em texto, separado por espaços ou quebras de linha:
 *
 *     <vértices> <arestas>
 *     u v peso        (uma linha por aresta, com u e v entre 0 e vértices-1)
 *
 * Exemplo:  3 2 / 0 1 4 / 1 2 7
 */

/* Aresta entre os vértices u e v. O grafo é não direcionado: u-v e v-u são a mesma aresta.
 * O peso é um inteiro e pode ser negativo (algoritmos como o Dijkstra devem rejeitar pesos negativos). */
typedef struct {
    int u;
    int v;
    int peso;
} Aresta;

typedef struct Grafo Grafo;

/**
 * Cria um grafo sem arestas.
 *
 * @param num_vertices  Quantidade de vértices (numerados de 0 a num_vertices-1). Deve ser >= 1.
 * @return              O grafo, ou NULL se num_vertices < 1 ou se faltar memória.
 *                      Liberar com grafo_destruir.
 */
Grafo *grafo_criar(int num_vertices);

/**
 * Adiciona uma aresta ao grafo (nas duas representações).
 *
 * @param grafo  Grafo a alterar. Não pode ser NULL.
 * @param u      Primeiro extremo, entre 0 e V-1.
 * @param v      Segundo extremo, entre 0 e V-1.
 * @param peso   Peso da aresta.
 * @return       1 em caso de sucesso; 0 se u ou v estiverem fora do intervalo ou se
 *               faltar memória. Se faltar memória, o grafo não deve mais ser usado:
 *               descarte-o com grafo_destruir.
 */
int grafo_adicionar_aresta(Grafo *grafo, int u, int v, int peso);

/**
 * Lê um grafo de um arquivo de texto no formato descrito acima.
 *
 * Entradas inválidas NÃO derrubam o programa: arquivo inexistente, cabeçalho inválido
 * (vértices < 1 ou arestas < 0), aresta incompleta ou faltando, vértice fora de
 * 0..V-1 e arestas a mais que o declarado resultam em NULL e em uma mensagem em erro.
 *
 * @param caminho   Caminho do arquivo. Não pode ser NULL.
 * @param erro      Buffer onde a mensagem de erro é escrita. Não pode ser NULL.
 * @param tam_erro  Tamanho do buffer erro, em bytes.
 * @return          O grafo, ou NULL em caso de erro (arquivo ou memória). O chamador
 *                  é responsável por liberá-lo com grafo_destruir.
 */
Grafo *grafo_ler(const char *caminho, char *erro, size_t tam_erro);

/**
 * @param grafo  Grafo consultado. Não pode ser NULL.
 * @return       Quantidade de vértices (V).
 */
int grafo_num_vertices(const Grafo *grafo);

/**
 * @param grafo  Grafo consultado. Não pode ser NULL.
 * @return       Quantidade de arestas.
 */
int grafo_num_arestas(const Grafo *grafo);

/**
 * Devolve a lista de arestas, na ordem em que foram adicionadas (representação do Kruskal).
 *
 * @param grafo  Grafo consultado. Não pode ser NULL.
 * @return       Lista de Aresta*. É do grafo: não altere nem destrua, e ela só é
 *               válida enquanto o grafo existir.
 */
const Lista *grafo_arestas(const Grafo *grafo);

/**
 * Devolve as arestas que tocam um vértice (representação do Dijkstra).
 *
 * @param grafo    Grafo consultado. Não pode ser NULL.
 * @param vertice  Vértice, entre 0 e V-1.
 * @return         Lista de Aresta* incidentes a vertice. É do grafo: não altere nem
 *                 destrua, e ela só é válida enquanto o grafo existir.
 */
const Lista *grafo_adjacentes(const Grafo *grafo, int vertice);

/**
 * Dado um extremo de uma aresta, devolve o outro. Usado para andar pela lista de adjacência.
 *
 * @param aresta   Aresta consultada. Não pode ser NULL.
 * @param vertice  Um dos extremos da aresta (u ou v).
 * @return         O outro extremo (num laço, o próprio vértice).
 */
int aresta_outro_extremo(const Aresta *aresta, int vertice);

/**
 * Libera o grafo e todas as suas arestas.
 *
 * @param grafo  Grafo a destruir. Pode ser NULL (nada acontece).
 */
void grafo_destruir(Grafo *grafo);

#endif
