#include <stdlib.h>
#include "fp_heap.h"
#include "heap.h"

/* A fila com heap usa o heap da #6: o topo do heap é o próximo a sair da fila */
struct FilaHeap {
    Heap *heap;
};

FilaHeap *fp_heap_criar(FpComparador comparar)
{
    FilaHeap *fila = malloc(sizeof(FilaHeap));
    if (fila == NULL)
        return NULL;

    fila->heap = heap_criar(comparar);
    if (fila->heap == NULL) {
        free(fila);
        return NULL;
    }
    return fila;
}

int fp_heap_inserir(FilaHeap *fila, void *dado)
{
    return heap_inserir(fila->heap, dado);
}

void *fp_heap_remover(FilaHeap *fila)
{
    return heap_remover(fila->heap);
}

int fp_heap_tamanho(const FilaHeap *fila)
{
    return heap_tamanho(fila->heap);
}

void fp_heap_destruir(FilaHeap *fila, void (*liberar)(void *dado))
{
    if (fila == NULL)
        return;
    heap_destruir(fila->heap, liberar);
    free(fila);
}
