#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>

/**
 * Conta quantas vezes cada byte (0 a 255) aparece no arquivo e volta ao
 * início do arquivo no final.
 *
 * @param entrada  Arquivo aberto em modo binário ("rb").
 * @param freq     Vetor de 256 posições; é zerado antes da contagem.
 */
void huffman_contar_frequencias(FILE *entrada, unsigned long long freq[256]);

/**
 * Grava os 2 bytes do cabeçalho: os 3 bits mais altos do 1º byte guardam o
 * lixo; os 13 bits restantes (5 do 1º byte e os 8 do 2º) guardam o tamanho
 * da árvore.
 *
 * @param saida           Arquivo aberto em modo binário.
 * @param lixo            Bits de lixo do último byte (0 a 7).
 * @param tamanho_arvore  Bytes da árvore em pré-ordem, contando os escapes (0 a 8191).
 */
void huffman_gravar_cabecalho(FILE *saida, int lixo, int tamanho_arvore);

/**
 * Lê os 2 bytes do cabeçalho.
 *
 * @param entrada         Arquivo .huff posicionado no início.
 * @param lixo            Recebe os bits de lixo do último byte.
 * @param tamanho_arvore  Recebe o tamanho da árvore em bytes.
 * @return                1 se leu; 0 se o arquivo tem menos de 2 bytes.
 */
int huffman_ler_cabecalho(FILE *entrada, int *lixo, int *tamanho_arvore);

/**
 * Compacta um arquivo: cabeçalho, árvore em pré-ordem e os códigos de cada
 * byte, bit a bit. Um arquivo vazio vira um .huff só com o cabeçalho zerado.
 *
 * @param entrada  Arquivo original aberto em "rb", posicionado no início.
 * @param saida    Arquivo .huff aberto em "wb".
 * @return         1 se deu certo; 0 se faltou memória.
 */
int huffman_compactar(FILE *entrada, FILE *saida);

/**
 * Descompacta um arquivo .huff.
 *
 * Um .huff com tamanho de árvore 0 e nada depois do cabeçalho representa um
 * arquivo vazio, e a saída fica vazia.
 *
 * @param entrada  Arquivo .huff aberto em "rb", posicionado no início.
 * @param saida    Arquivo de saída aberto em "wb".
 * @return         1 se deu certo; 0 se o .huff é inválido ou faltou memória.
 */
int huffman_descompactar(FILE *entrada, FILE *saida);

#endif
