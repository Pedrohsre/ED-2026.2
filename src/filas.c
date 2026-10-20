#include "filas.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct No {
    void *dado;
    struct No *proximo;
} No;

struct FilaLista {
    No *inicio;
    Comparador comparar;
};

struct FilaHeap {
    void **dados;
    size_t tamanho;
    size_t capacidade;
    Comparador comparar;
};

FilaLista *fila_lista_criar(Comparador comparar)
{
    FilaLista *fila = malloc(sizeof *fila);
    if (fila) {
        fila->inicio = NULL;
        fila->comparar = comparar;
    }
    return fila;
}

int fila_lista_inserir(FilaLista *fila, void *dado)
{
    No *novo = malloc(sizeof *novo);
    No **posicao = &fila->inicio;
    if (!novo)
        return 0;
    while (*posicao && fila->comparar((*posicao)->dado, dado) <= 0)
        posicao = &(*posicao)->proximo;
    novo->dado = dado;
    novo->proximo = *posicao;
    *posicao = novo;
    return 1;
}

void fila_lista_destruir(FilaLista *fila)
{
    if (!fila)
        return;
    while (fila->inicio) {
        No *removido = fila->inicio;
        fila->inicio = removido->proximo;
        free(removido);
    }
    free(fila);
}

FilaHeap *fila_heap_criar(Comparador comparar)
{
    FilaHeap *fila = malloc(sizeof *fila);
    if (fila) {
        fila->dados = NULL;
        fila->tamanho = 0;
        fila->capacidade = 0;
        fila->comparar = comparar;
    }
    return fila;
}

static int heap_crescer(FilaHeap *fila)
{
    size_t capacidade = fila->capacidade ? fila->capacidade * 2 : 16;
    void **dados;
    if (capacidade < fila->capacidade || capacidade > SIZE_MAX / sizeof *dados)
        return 0;
    dados = realloc(fila->dados, capacidade * sizeof *dados);
    if (!dados)
        return 0;
    fila->dados = dados;
    fila->capacidade = capacidade;
    return 1;
}

int fila_heap_inserir(FilaHeap *fila, void *dado)
{
    size_t posicao = fila->tamanho;
    if (fila->tamanho == fila->capacidade && !heap_crescer(fila))
        return 0;
    while (posicao > 0) {
        size_t pai = (posicao - 1) / 2;
        if (fila->comparar(dado, fila->dados[pai]) >= 0)
            break;
        fila->dados[posicao] = fila->dados[pai];
        posicao = pai;
    }
    fila->dados[posicao] = dado;
    fila->tamanho++;
    return 1;
}

void fila_heap_destruir(FilaHeap *fila)
{
    if (fila) {
        free(fila->dados);
        free(fila);
    }
}
