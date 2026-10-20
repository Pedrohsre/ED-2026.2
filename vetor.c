#include <stdio.h>
#include <stdlib.h>
#include "vetor.h"

#define CAPACIDADE_INICIAL 8

struct Vetor {
    void **dados;
    int tamanho;
    int capacidade;
};

Vetor *vetor_criar(void) {
    Vetor *vetor = malloc(sizeof *vetor);
    if (vetor == NULL) return NULL;

    vetor->dados = malloc(CAPACIDADE_INICIAL * sizeof *vetor->dados);
    if (vetor->dados == NULL) {
        free(vetor);
        return NULL;
    }
    vetor->tamanho = 0;
    vetor->capacidade = CAPACIDADE_INICIAL;
    return vetor;
}

int vetor_adicionar(Vetor *vetor, void *dado) {
    if (vetor->tamanho == vetor->capacidade) {
        int nova_capacidade = vetor->capacidade * 2;
        void **novos = realloc(vetor->dados, nova_capacidade * sizeof *novos);
        if (novos == NULL) return 0;   /* o buffer antigo continua válido */
        vetor->dados = novos;
        vetor->capacidade = nova_capacidade;
    }
    vetor->dados[vetor->tamanho++] = dado;
    return 1;
}

void *vetor_obter(const Vetor *vetor, int indice) {
    return vetor->dados[indice];
}

int vetor_tamanho(const Vetor *vetor) {
    return vetor->tamanho;
}

void vetor_imprimir(const Vetor *vetor, FuncaoImpressao imprimir, const char *separador) {
    for (int i = 0; i < vetor->tamanho; i++) {
        if (i > 0) printf("%s", separador);
        imprimir(vetor->dados[i]);
    }
}

void vetor_destruir(Vetor *vetor, FuncaoLiberacao liberar) {
    if (vetor == NULL) return;
    if (liberar != NULL) {
        for (int i = 0; i < vetor->tamanho; i++)
            liberar(vetor->dados[i]);
    }
    free(vetor->dados);
    free(vetor);
}
