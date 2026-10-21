#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stddef.h>
#include "grafo.h"

/*
 * Dijkstra (#22): menor distância de um vértice de origem até todos os outros,
 * em um grafo com pesos não negativos.
 *
 * Usa a fila de prioridade com heap com pares (distância, vértice). A fila não tem
 * "diminuir prioridade": quando a distância de um vértice melhora, ele é inserido
 * de novo, e a entrada antiga é ignorada quando sai da fila.
 */

#define DIJKSTRA_INALCANCAVEL (-1)

typedef struct {
    int num_vertices;
    int origem;
    int *distancia;   /* distancia[v]: menor distância até v, ou DIJKSTRA_INALCANCAVEL */
    int *anterior;    /* anterior[v]: vértice antes de v no caminho; -1 na origem e nos inalcançáveis */
} CaminhosMinimos;

/**
 * Calcula os caminhos mínimos a partir da origem.
 *
 * @param grafo     Grafo de entrada. Não pode ser NULL e não é alterado.
 * @param origem    Vértice de partida, entre 0 e V-1.
 * @param erro      Buffer onde a mensagem de erro é escrita. Não pode ser NULL.
 * @param tam_erro  Tamanho do buffer erro, em bytes.
 * @return          O resultado, ou NULL se a origem for inválida, se alguma aresta
 *                  tiver peso negativo ou se faltar memória (com a mensagem em erro).
 *                  Liberar com dijkstra_destruir.
 */
CaminhosMinimos *dijkstra_calcular(const Grafo *grafo, int origem, char *erro, size_t tam_erro);

/**
 * Imprime a distância e o caminho até cada vértice.
 *
 * @param caminhos  Resultado de dijkstra_calcular. Não pode ser NULL.
 */
void dijkstra_imprimir(const CaminhosMinimos *caminhos);

/**
 * Libera o resultado.
 *
 * @param caminhos  Resultado a destruir. Pode ser NULL (nada acontece).
 */
void dijkstra_destruir(CaminhosMinimos *caminhos);

#endif
