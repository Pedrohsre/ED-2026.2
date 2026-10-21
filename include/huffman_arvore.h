#ifndef HUFFMAN_ARVORE_H
#define HUFFMAN_ARVORE_H

#include <stdio.h>

/**
 * Nó da árvore de Huffman.
 *
 * Nas folhas, byte aponta para um unsigned char alocado com malloc (todas as
 * estruturas guardam void*) e os dois filhos são NULL.
 * Nos nós internos, byte é NULL e os dois filhos existem.
 */
typedef struct NoHuffman NoHuffman;
struct NoHuffman {
    void *byte;
    unsigned long long frequencia;
    NoHuffman *esquerda;
    NoHuffman *direita;
};

/**
 * Cria uma folha. O byte é copiado para um bloco alocado, apontado por void*.
 *
 * @param byte        Byte que a folha representa.
 * @param frequencia  Quantas vezes o byte aparece no arquivo.
 * @return            A folha, ou NULL se faltar memória. Liberar com huffman_destruir_arvore.
 */
NoHuffman *huffman_criar_folha(unsigned char byte, unsigned long long frequencia);

/**
 * Cria um nó interno (o "*" do slide) com a soma das frequências dos filhos.
 *
 * @param esquerda  Filho esquerdo (caminho 0). Não pode ser NULL.
 * @param direita   Filho direito (caminho 1). Não pode ser NULL.
 * @return          O nó, ou NULL se faltar memória. Os filhos passam a pertencer ao nó.
 */
NoHuffman *huffman_criar_no_interno(NoHuffman *esquerda, NoHuffman *direita);

/**
 * @param no  Nó a testar. Não pode ser NULL.
 * @return    1 se o nó é folha; 0 se é nó interno.
 */
int huffman_eh_folha(const NoHuffman *no);

/**
 * @param folha  Uma folha. Não pode ser nó interno.
 * @return       O byte guardado na folha.
 */
unsigned char huffman_byte_da_folha(const NoHuffman *folha);

/**
 * Libera a árvore inteira em pós-ordem: os filhos antes do pai.
 *
 * @param raiz  Raiz da árvore. Pode ser NULL.
 */
void huffman_destruir_arvore(NoHuffman *raiz);

/**
 * Conta quantos bytes a árvore ocupa em pré-ordem, incluindo os bytes de
 * escape ("\") das folhas "*" e "\". É o tamanho gravado no cabeçalho.
 *
 * @param raiz  Raiz da árvore. Pode ser NULL (tamanho 0).
 * @return      Tamanho da árvore em bytes.
 */
int huffman_tamanho_arvore(const NoHuffman *raiz);

/**
 * Grava a árvore em pré-ordem: "*" para nó interno; a folha "*" ou "\" é
 * gravada com um "\" antes.
 *
 * @param raiz   Raiz da árvore. Não pode ser NULL.
 * @param saida  Arquivo aberto em modo binário.
 */
void huffman_gravar_arvore(const NoHuffman *raiz, FILE *saida);

/**
 * Reconstrói a árvore a partir dos bytes gravados em pré-ordem.
 *
 * @param entrada    Arquivo posicionado no início da árvore.
 * @param restantes  Bytes da árvore que ainda podem ser lidos (o tamanho do
 *                   cabeçalho); é decrementado a cada byte lido.
 * @return           A raiz, ou NULL se os bytes acabarem antes da árvore
 *                   terminar ou faltar memória. Liberar com huffman_destruir_arvore.
 */
NoHuffman *huffman_ler_arvore(FILE *entrada, int *restantes);

/**
 * Monta a árvore de Huffman com a fila de prioridade com heap: uma folha por
 * byte que aparece; depois junta as duas de menor frequência até sobrar uma.
 *
 * Com um só byte distinto, a árvore seria uma folha só e o código teria 0
 * bits. Nesse caso é criada uma folha extra, de frequência 0, para o byte
 * ganhar um código de 1 bit.
 *
 * @param freq  Frequência de cada byte (256 posições).
 * @return      A raiz, ou NULL se nenhum byte aparece (arquivo vazio) ou faltar
 *              memória. Liberar com huffman_destruir_arvore.
 */
NoHuffman *huffman_construir_arvore(const unsigned long long freq[256]);

/**
 * Tabela com o código de cada byte, como texto de '0' e '1'.
 * Um código tem no máximo 255 bits (a profundidade máxima com 256 folhas).
 */
typedef struct {
    char codigo[256][257];   /* codigo[b] é "" se o byte b não aparece no arquivo */
} TabelaCodigos;

/**
 * Percorre a árvore e anota o caminho até cada folha: esquerda = '0', direita = '1'.
 *
 * @param raiz    Raiz da árvore, com pelo menos um nó interno.
 * @param tabela  Tabela a preencher (é zerada antes).
 */
void huffman_montar_codigos(const NoHuffman *raiz, TabelaCodigos *tabela);

#endif
