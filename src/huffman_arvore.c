#include <stdlib.h>
#include <string.h>
#include "huffman_arvore.h"
#include "fp_heap.h"

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

/*
 * A fila tira primeiro o nó de menor frequência. No empate, o nó interno sai
 * antes da folha, como no slide (*3 antes de D3, *6 antes de A6), e a árvore
 * do exemplo fica igual à do slide: **CB***FEDA.
 */
static int comparar_frequencia(const void *a, const void *b)
{
    const NoHuffman *x = a;
    const NoHuffman *y = b;

    if (x->frequencia != y->frequencia)
        return x->frequencia < y->frequencia ? -1 : 1;
    return huffman_eh_folha(x) - huffman_eh_folha(y);
}

/* Adaptador para a fila liberar os nós que sobrarem nela */
static void liberar_no(void *no)
{
    huffman_destruir_arvore(no);
}

static int inserir_folha(FilaHeap *fila, unsigned char byte, unsigned long long frequencia)
{
    NoHuffman *folha = huffman_criar_folha(byte, frequencia);
    if (folha == NULL)
        return 0;
    if (!fp_heap_inserir(fila, folha)) {
        huffman_destruir_arvore(folha);
        return 0;
    }
    return 1;
}

/* Um byte diferente do único que aparece no arquivo, para a folha extra */
static unsigned char byte_extra(const unsigned long long freq[256])
{
    for (int b = 0; b < 256; b++)
        if (freq[b] > 0)
            return (unsigned char) (b + 1);   /* 255 + 1 vira 0 no unsigned char */
    return 0;
}

/* Passo 1 do slide: uma folha na fila para cada byte que aparece */
static FilaHeap *fila_de_folhas(const unsigned long long freq[256])
{
    FilaHeap *fila = fp_heap_criar(comparar_frequencia);
    if (fila == NULL)
        return NULL;

    int ok = 1;
    for (int b = 0; b < 256 && ok; b++)
        if (freq[b] > 0)
            ok = inserir_folha(fila, (unsigned char) b, freq[b]);
    if (ok && fp_heap_tamanho(fila) == 1)
        ok = inserir_folha(fila, byte_extra(freq), 0);

    if (!ok) {
        fp_heap_destruir(fila, liberar_no);
        return NULL;
    }
    return fila;
}

NoHuffman *huffman_construir_arvore(const unsigned long long freq[256])
{
    FilaHeap *fila = fila_de_folhas(freq);
    if (fila == NULL)
        return NULL;

    /* Passo 2 do slide: remove as duas menores e insere o pai, com a soma */
    while (fp_heap_tamanho(fila) > 1) {
        NoHuffman *esquerda = fp_heap_remover(fila);
        NoHuffman *direita = fp_heap_remover(fila);
        NoHuffman *pai = huffman_criar_no_interno(esquerda, direita);

        if (pai == NULL) {
            huffman_destruir_arvore(esquerda);
            huffman_destruir_arvore(direita);
        } else if (!fp_heap_inserir(fila, pai)) {
            huffman_destruir_arvore(pai);
            pai = NULL;
        }
        if (pai == NULL) {
            fp_heap_destruir(fila, liberar_no);
            return NULL;
        }
    }

    NoHuffman *raiz = fp_heap_remover(fila);   /* NULL se a fila ficou vazia (arquivo vazio) */
    fp_heap_destruir(fila, NULL);
    return raiz;
}

static void montar(const NoHuffman *no, char *caminho, int profundidade, TabelaCodigos *tabela)
{
    if (huffman_eh_folha(no)) {
        caminho[profundidade] = '\0';
        strcpy(tabela->codigo[huffman_byte_da_folha(no)], caminho);
        return;
    }
    caminho[profundidade] = '0';   /* esquerda = 0 */
    montar(no->esquerda, caminho, profundidade + 1, tabela);
    caminho[profundidade] = '1';   /* direita = 1 */
    montar(no->direita, caminho, profundidade + 1, tabela);
}

void huffman_montar_codigos(const NoHuffman *raiz, TabelaCodigos *tabela)
{
    char caminho[257];

    memset(tabela, 0, sizeof(TabelaCodigos));
    montar(raiz, caminho, 0, tabela);
}
