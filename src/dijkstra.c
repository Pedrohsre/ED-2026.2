#include <stdio.h>
#include <stdlib.h>
#include "dijkstra.h"
#include "fp_heap.h"

/* Entrada da fila: um vértice e a distância com que ele foi inserido */
typedef struct {
    int distancia;
    int vertice;
} Entrada;

/* A fila tira primeiro a menor distância */
static int comparar_distancia(const void *a, const void *b)
{
    const Entrada *x = a;
    const Entrada *y = b;
    return (x->distancia > y->distancia) - (x->distancia < y->distancia);
}

static int inserir(FilaHeap *fila, int distancia, int vertice)
{
    Entrada *entrada = malloc(sizeof(Entrada));
    if (entrada == NULL)
        return 0;
    entrada->distancia = distancia;
    entrada->vertice = vertice;
    if (!fp_heap_inserir(fila, entrada)) {
        free(entrada);
        return 0;
    }
    return 1;
}

static int tem_peso_negativo(const Grafo *grafo)
{
    for (const No *no = grafo_arestas(grafo)->inicio; no != NULL; no = no->proximo) {
        const Aresta *aresta = no->dado;
        if (aresta->peso < 0)
            return 1;
    }
    return 0;
}

static CaminhosMinimos *criar_caminhos(int num_vertices, int origem)
{
    CaminhosMinimos *caminhos = malloc(sizeof(CaminhosMinimos));
    if (caminhos == NULL)
        return NULL;
    caminhos->num_vertices = num_vertices;
    caminhos->origem = origem;
    caminhos->distancia = malloc(num_vertices * sizeof(int));
    caminhos->anterior = malloc(num_vertices * sizeof(int));
    if (caminhos->distancia == NULL || caminhos->anterior == NULL) {
        dijkstra_destruir(caminhos);
        return NULL;
    }
    for (int v = 0; v < num_vertices; v++) {
        caminhos->distancia[v] = DIJKSTRA_INALCANCAVEL;
        caminhos->anterior[v] = -1;
    }
    caminhos->distancia[origem] = 0;
    return caminhos;
}

/* Relaxa as arestas de u: se passar por u encurta o caminho até v, atualiza v */
static int relaxar_vizinhos(const Grafo *grafo, CaminhosMinimos *caminhos, FilaHeap *fila, int u)
{
    for (const No *no = grafo_adjacentes(grafo, u)->inicio; no != NULL; no = no->proximo) {
        const Aresta *aresta = no->dado;
        int v = aresta_outro_extremo(aresta, u);
        int nova = caminhos->distancia[u] + aresta->peso;

        if (caminhos->distancia[v] == DIJKSTRA_INALCANCAVEL || nova < caminhos->distancia[v]) {
            caminhos->distancia[v] = nova;
            caminhos->anterior[v] = u;
            if (!inserir(fila, nova, v))   /* reinsere em vez de diminuir a prioridade */
                return 0;
        }
    }
    return 1;
}

static int percorrer(const Grafo *grafo, CaminhosMinimos *caminhos, FilaHeap *fila)
{
    int ok = inserir(fila, 0, caminhos->origem);

    while (ok && fp_heap_tamanho(fila) > 0) {
        Entrada *entrada = fp_heap_remover(fila);
        /* uma entrada com distância maior que a atual está desatualizada: é ignorada */
        if (entrada->distancia == caminhos->distancia[entrada->vertice])
            ok = relaxar_vizinhos(grafo, caminhos, fila, entrada->vertice);
        free(entrada);
    }
    return ok;
}

CaminhosMinimos *dijkstra_calcular(const Grafo *grafo, int origem, char *erro, size_t tam_erro)
{
    int num_vertices = grafo_num_vertices(grafo);

    if (origem < 0 || origem >= num_vertices) {
        snprintf(erro, tam_erro, "origem %d fora do intervalo 0..%d", origem, num_vertices - 1);
        return NULL;
    }
    if (tem_peso_negativo(grafo)) {
        snprintf(erro, tam_erro, "o grafo tem aresta com peso negativo, e o Dijkstra só funciona com pesos >= 0");
        return NULL;
    }

    CaminhosMinimos *caminhos = criar_caminhos(num_vertices, origem);
    FilaHeap *fila = fp_heap_criar(comparar_distancia);
    int ok = caminhos != NULL && fila != NULL && percorrer(grafo, caminhos, fila);

    fp_heap_destruir(fila, free);   /* só sobram entradas se faltou memória no meio */
    if (!ok) {
        snprintf(erro, tam_erro, "memória insuficiente");
        dijkstra_destruir(caminhos);
        return NULL;
    }
    return caminhos;
}

/* Imprime o caminho da origem até v, seguindo o anterior de trás para a frente */
static void imprimir_caminho(const CaminhosMinimos *caminhos, int v)
{
    if (caminhos->anterior[v] != -1) {
        imprimir_caminho(caminhos, caminhos->anterior[v]);
        printf(" -> ");
    }
    printf("%d", v);
}

void dijkstra_imprimir(const CaminhosMinimos *caminhos)
{
    printf("Menores distâncias a partir do vértice %d:\n", caminhos->origem);
    for (int v = 0; v < caminhos->num_vertices; v++) {
        if (caminhos->distancia[v] == DIJKSTRA_INALCANCAVEL) {
            printf("  %d: inalcançável\n", v);
        } else {
            printf("  %d: %d (caminho: ", v, caminhos->distancia[v]);
            imprimir_caminho(caminhos, v);
            printf(")\n");
        }
    }
}

void dijkstra_destruir(CaminhosMinimos *caminhos)
{
    if (caminhos == NULL)
        return;
    free(caminhos->distancia);
    free(caminhos->anterior);
    free(caminhos);
}
