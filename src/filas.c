#include "filas.h"
#include <stdlib.h>

typedef struct No {
    void *dado;
    struct No *proximo;
} No;

struct FilaLista {
    No *inicio;
    int tamanho;
    Comparador comparar;
};

FilaLista *fila_lista_criar(Comparador comparar)
{
    FilaLista *fila = malloc(sizeof *fila);
    if (fila) {
        fila->inicio = NULL;
        fila->tamanho = 0;
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
    fila->tamanho++;
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

void *fila_lista_remover(FilaLista *fila)
{
    No *primeiro = fila->inicio;
    void *dado;
    if (!primeiro)
        return NULL;
    dado = primeiro->dado;
    fila->inicio = primeiro->proximo;
    free(primeiro);
    fila->tamanho--;
    return dado;
}

int fila_lista_tamanho(const FilaLista *fila)
{
    return fila->tamanho;
}
