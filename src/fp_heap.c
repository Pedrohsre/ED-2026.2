#include <stdlib.h>
#include "fp_heap.h"

/* Heap em vetor com a raiz no índice 1, como no slide: pai = i/2, filhos = 2i e 2i+1 */
struct FilaHeap {
    void **dados;      /* dados[1..tamanho]; a posição 0 não é usada */
    int tamanho;
    int capacidade;
    FpComparador comparar;
};

/* O elemento da posição i deve ficar acima do da posição j? */
static int vem_antes(const FilaHeap *fila, int i, int j)
{
    return fila->comparar(fila->dados[i], fila->dados[j]) < 0;
}

static void trocar(FilaHeap *fila, int i, int j)
{
    void *aux = fila->dados[i];
    fila->dados[i] = fila->dados[j];
    fila->dados[j] = aux;
}

FilaHeap *fp_heap_criar(FpComparador comparar)
{
    FilaHeap *fila = malloc(sizeof(FilaHeap));
    if (fila == NULL)
        return NULL;
    fila->dados = NULL;
    fila->tamanho = 0;
    fila->capacidade = 0;
    fila->comparar = comparar;
    return fila;
}

int fp_heap_inserir(FilaHeap *fila, void *dado)
{
    if (fila->tamanho == fila->capacidade) {   /* vetor cheio: dobra o tamanho */
        int nova = fila->capacidade == 0 ? 16 : 2 * fila->capacidade;
        void **dados = realloc(fila->dados, (nova + 1) * sizeof(void *));
        if (dados == NULL)
            return 0;
        fila->dados = dados;
        fila->capacidade = nova;
    }

    fila->tamanho++;
    int i = fila->tamanho;
    fila->dados[i] = dado;                      /* entra no fim e sobe enquanto vier antes do pai */
    while (i > 1 && vem_antes(fila, i, i / 2)) {
        trocar(fila, i, i / 2);
        i = i / 2;
    }
    return 1;
}

/* O heapify do slide: desce o elemento da posição i, trocando com o filho que vem antes */
static void descer(FilaHeap *fila, int i)
{
    int menor = i;
    if (2 * i <= fila->tamanho && vem_antes(fila, 2 * i, menor))
        menor = 2 * i;
    if (2 * i + 1 <= fila->tamanho && vem_antes(fila, 2 * i + 1, menor))
        menor = 2 * i + 1;
    if (menor != i) {
        trocar(fila, i, menor);
        descer(fila, menor);
    }
}

void *fp_heap_remover(FilaHeap *fila)
{
    if (fila->tamanho == 0)
        return NULL;
    void *topo = fila->dados[1];
    fila->dados[1] = fila->dados[fila->tamanho];   /* o último vai para a raiz e desce */
    fila->tamanho--;
    descer(fila, 1);
    return topo;
}

int fp_heap_tamanho(const FilaHeap *fila)
{
    return fila->tamanho;
}

void fp_heap_destruir(FilaHeap *fila, void (*liberar)(void *dado))
{
    if (fila == NULL)
        return;
    for (int i = 1; liberar != NULL && i <= fila->tamanho; i++)
        liberar(fila->dados[i]);
    free(fila->dados);
    free(fila);
}
