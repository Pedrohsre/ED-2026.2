#include <stdlib.h>
#include "huffman_arvore.h"

#define NO_INTERNO '*'
#define ESCAPE '\\'

NoHuffman *huffman_criar_folha(unsigned char byte, unsigned long long frequencia)
{
    NoHuffman *folha = malloc(sizeof(NoHuffman));
    unsigned char *copia = malloc(sizeof(unsigned char));
    if (folha == NULL || copia == NULL) {
        free(folha);
        free(copia);
        return NULL;
    }
    *copia = byte;
    folha->byte = copia;
    folha->frequencia = frequencia;
    folha->esquerda = NULL;
    folha->direita = NULL;
    return folha;
}

NoHuffman *huffman_criar_no_interno(NoHuffman *esquerda, NoHuffman *direita)
{
    NoHuffman *no = malloc(sizeof(NoHuffman));
    if (no == NULL)
        return NULL;
    no->byte = NULL;
    no->frequencia = esquerda->frequencia + direita->frequencia;
    no->esquerda = esquerda;
    no->direita = direita;
    return no;
}

int huffman_eh_folha(const NoHuffman *no)
{
    return no->esquerda == NULL && no->direita == NULL;
}

unsigned char huffman_byte_da_folha(const NoHuffman *folha)
{
    return *(unsigned char *) folha->byte;
}

void huffman_destruir_arvore(NoHuffman *raiz)
{
    if (raiz == NULL)
        return;
    huffman_destruir_arvore(raiz->esquerda);
    huffman_destruir_arvore(raiz->direita);
    free(raiz->byte);   /* nos nós internos é NULL, e free(NULL) não faz nada */
    free(raiz);
}

/* Uma folha com "*" ou "\" precisa do escape, senão seria lida como nó interno */
static int precisa_escape(unsigned char byte)
{
    return byte == NO_INTERNO || byte == ESCAPE;
}

int huffman_tamanho_arvore(const NoHuffman *raiz)
{
    if (raiz == NULL)
        return 0;
    if (huffman_eh_folha(raiz))
        return precisa_escape(huffman_byte_da_folha(raiz)) ? 2 : 1;
    return 1 + huffman_tamanho_arvore(raiz->esquerda) + huffman_tamanho_arvore(raiz->direita);
}

void huffman_gravar_arvore(const NoHuffman *raiz, FILE *saida)
{
    if (huffman_eh_folha(raiz)) {
        unsigned char byte = huffman_byte_da_folha(raiz);
        if (precisa_escape(byte))
            fputc(ESCAPE, saida);
        fputc(byte, saida);
    } else {
        /* pré-ordem: o nó, depois a subárvore esquerda, depois a direita */
        fputc(NO_INTERNO, saida);
        huffman_gravar_arvore(raiz->esquerda, saida);
        huffman_gravar_arvore(raiz->direita, saida);
    }
}

/* Lê o próximo byte da árvore sem passar do tamanho informado no cabeçalho */
static int ler_byte_da_arvore(FILE *entrada, int *restantes)
{
    if (*restantes <= 0)
        return EOF;
    (*restantes)--;
    return fgetc(entrada);
}

/* Depois de um "*" vêm exatamente duas subárvores: a esquerda e a direita */
static NoHuffman *ler_no_interno(FILE *entrada, int *restantes)
{
    NoHuffman *esquerda = huffman_ler_arvore(entrada, restantes);
    if (esquerda == NULL)
        return NULL;

    NoHuffman *direita = huffman_ler_arvore(entrada, restantes);
    if (direita == NULL) {
        huffman_destruir_arvore(esquerda);
        return NULL;
    }

    NoHuffman *no = huffman_criar_no_interno(esquerda, direita);
    if (no == NULL) {
        huffman_destruir_arvore(esquerda);
        huffman_destruir_arvore(direita);
    }
    return no;
}

NoHuffman *huffman_ler_arvore(FILE *entrada, int *restantes)
{
    int c = ler_byte_da_arvore(entrada, restantes);
    if (c == EOF)
        return NULL;
    if (c == NO_INTERNO)
        return ler_no_interno(entrada, restantes);

    if (c == ESCAPE) {
        c = ler_byte_da_arvore(entrada, restantes);   /* o byte depois do escape é uma folha */
        if (c == EOF)
            return NULL;
    }
    return huffman_criar_folha((unsigned char) c, 0);   /* a frequência não é gravada no .huff */
}
