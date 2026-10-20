#include <stdlib.h>
#include "heap.h"

#define CAPACIDADE_INICIAL 16

struct Heap {
    void **dados;      /* dados[1..tamanho]; a posição 0 não é usada */
    int tamanho;
    int capacidade;    /* quantos elementos cabem; o vetor tem capacidade + 1 posições */
    HeapComparador comparar;
};

static int pai(int i)      { return i / 2; }
static int esquerdo(int i) { return 2 * i; }
static int direito(int i)  { return 2 * i + 1; }

/* O elemento da posição i deve ficar acima do da posição j? */
static int vem_antes(const Heap *heap, int i, int j)
{
    return heap->comparar(heap->dados[i], heap->dados[j]) < 0;
}

static void trocar(Heap *heap, int i, int j)
{
    void *aux = heap->dados[i];
    heap->dados[i] = heap->dados[j];
    heap->dados[j] = aux;
}

Heap *heap_criar(HeapComparador comparar)
{
    Heap *heap = malloc(sizeof(Heap));
    if (heap == NULL)
        return NULL;

    heap->dados = malloc((CAPACIDADE_INICIAL + 1) * sizeof(void *));
    if (heap->dados == NULL) {
        free(heap);
        return NULL;
    }
    heap->tamanho = 0;
    heap->capacidade = CAPACIDADE_INICIAL;
    heap->comparar = comparar;
    return heap;
}

/* Dobra a capacidade. Se o realloc falhar, o vetor antigo continua válido. */
static int crescer(Heap *heap)
{
    int nova = 2 * heap->capacidade;
    void **novos = realloc(heap->dados, (nova + 1) * sizeof(void *));
    if (novos == NULL)
        return 0;
    heap->dados = novos;
    heap->capacidade = nova;
    return 1;
}

int heap_inserir(Heap *heap, void *dado)
{
    if (heap->tamanho == heap->capacidade && !crescer(heap))
        return 0;

    heap->tamanho++;
    heap->dados[heap->tamanho] = dado;   /* entra na primeira posição livre */

    int i = heap->tamanho;
    while (i > 1 && vem_antes(heap, i, pai(i))) {   /* sobe enquanto vier antes do pai */
        trocar(heap, i, pai(i));
        i = pai(i);
    }
    return 1;
}

/* O heapify do slide: desce o elemento da posição i, trocando com o filho que vem antes */
static void descer(Heap *heap, int i)
{
    int primeiro = i;
    int e = esquerdo(i);
    int d = direito(i);

    if (e <= heap->tamanho && vem_antes(heap, e, primeiro))
        primeiro = e;
    if (d <= heap->tamanho && vem_antes(heap, d, primeiro))
        primeiro = d;

    if (primeiro != i) {
        trocar(heap, i, primeiro);
        descer(heap, primeiro);
    }
}

void *heap_remover(Heap *heap)
{
    if (heap->tamanho == 0)
        return NULL;

    void *topo = heap->dados[1];
    heap->dados[1] = heap->dados[heap->tamanho];   /* o último vai para a raiz */
    heap->tamanho--;
    descer(heap, 1);
    return topo;
}

void *heap_topo(const Heap *heap)
{
    if (heap->tamanho == 0)
        return NULL;
    return heap->dados[1];
}

int heap_tamanho(const Heap *heap)
{
    return heap->tamanho;
}

int heap_vazio(const Heap *heap)
{
    return heap->tamanho == 0;
}

void heap_destruir(Heap *heap, void (*liberar)(void *dado))
{
    if (heap == NULL)
        return;
    if (liberar != NULL)
        for (int i = 1; i <= heap->tamanho; i++)
            liberar(heap->dados[i]);
    free(heap->dados);
    free(heap);
}
