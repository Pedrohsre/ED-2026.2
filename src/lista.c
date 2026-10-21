#include <stdlib.h>
#include "lista.h"

Lista *lista_criar(void)
{
    Lista *lista = malloc(sizeof(Lista));
    if (lista == NULL)
        return NULL;

    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
    return lista;
}

static No *no_criar(void *dado, No *proximo)
{
    No *no = malloc(sizeof(No));
    if (no == NULL)
        return NULL;

    no->dado = dado;
    no->proximo = proximo;
    return no;
}

int lista_inserir_inicio(Lista *lista, void *dado)
{
    No *no = no_criar(dado, lista->inicio);
    if (no == NULL)
        return 0;

    lista->inicio = no;
    if (lista->fim == NULL)   /* a lista estava vazia: o novo nó também é o último */
        lista->fim = no;
    lista->tamanho++;
    return 1;
}

int lista_inserir_fim(Lista *lista, void *dado)
{
    No *no = no_criar(dado, NULL);
    if (no == NULL)
        return 0;

    if (lista->fim == NULL)
        lista->inicio = no;
    else
        lista->fim->proximo = no;
    lista->fim = no;
    lista->tamanho++;
    return 1;
}

int lista_inserir_ordenado(Lista *lista, void *dado, ListaComparador comparar)
{
    if (lista->inicio == NULL || comparar(dado, lista->inicio->dado) < 0)
        return lista_inserir_inicio(lista, dado);

    /* Avança enquanto o próximo não vier depois do novo. Com ">= 0", empates
     * ficam à frente do novo elemento, o que mantém a ordem de chegada. */
    No *anterior = lista->inicio;
    while (anterior->proximo != NULL && comparar(dado, anterior->proximo->dado) >= 0)
        anterior = anterior->proximo;

    No *no = no_criar(dado, anterior->proximo);
    if (no == NULL)
        return 0;

    anterior->proximo = no;
    if (anterior == lista->fim)
        lista->fim = no;
    lista->tamanho++;
    return 1;
}

void *lista_remover_inicio(Lista *lista)
{
    if (lista->inicio == NULL)
        return NULL;

    No *removido = lista->inicio;
    void *dado = removido->dado;

    lista->inicio = removido->proximo;
    if (lista->inicio == NULL)   /* removeu o único nó: sem isso, fim ficaria apontando para memória liberada */
        lista->fim = NULL;
    lista->tamanho--;
    free(removido);
    return dado;
}

void *lista_buscar(const Lista *lista, const void *chave, ListaComparador comparar)
{
    for (const No *no = lista->inicio; no != NULL; no = no->proximo) {
        if (comparar(chave, no->dado) == 0)
            return no->dado;
    }
    return NULL;
}

int lista_tamanho(const Lista *lista)
{
    return lista->tamanho;
}

int lista_vazia(const Lista *lista)
{
    return lista->tamanho == 0;
}

void lista_destruir(Lista *lista, void (*liberar)(void *dado))
{
    if (lista == NULL)
        return;

    No *no = lista->inicio;
    while (no != NULL) {
        No *proximo = no->proximo;   /* guarda antes de liberar o nó */
        if (liberar != NULL)
            liberar(no->dado);
        free(no);
        no = proximo;
    }
    free(lista);
}
