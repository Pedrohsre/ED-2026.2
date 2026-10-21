#include <string.h>
#include "huffman.h"
#include "huffman_arvore.h"
#include "bits.h"

void huffman_contar_frequencias(FILE *entrada, unsigned long long freq[256])
{
    int c;   /* int, e não char: fgetc devolve EOF (-1) no fim do arquivo */

    memset(freq, 0, 256 * sizeof(unsigned long long));
    while ((c = fgetc(entrada)) != EOF)
        freq[c]++;
    rewind(entrada);
}

void huffman_gravar_cabecalho(FILE *saida, int lixo, int tamanho_arvore)
{
    /* 1º byte: o lixo nos 3 bits mais altos e os 5 bits mais altos do tamanho */
    unsigned char byte1 = (unsigned char) ((lixo << 5) | (tamanho_arvore >> 8));
    /* 2º byte: os 8 bits mais baixos do tamanho */
    unsigned char byte2 = (unsigned char) (tamanho_arvore & 0xFF);

    fputc(byte1, saida);
    fputc(byte2, saida);
}

int huffman_ler_cabecalho(FILE *entrada, int *lixo, int *tamanho_arvore)
{
    int byte1 = fgetc(entrada);
    int byte2 = fgetc(entrada);
    if (byte1 == EOF || byte2 == EOF)
        return 0;

    *lixo = byte1 >> 5;
    *tamanho_arvore = ((byte1 & 0x1F) << 8) | byte2;   /* 0x1F = 00011111 */
    return 1;
}

static long tamanho_do_arquivo(FILE *arquivo)
{
    long posicao = ftell(arquivo);
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fseek(arquivo, posicao, SEEK_SET);
    return tamanho;
}

/*
 * Percorre a árvore bit a bit: 0 desce à esquerda, 1 à direita. Ao chegar a
 * uma folha, grava o byte e volta para a raiz. No último byte, os "lixo" bits
 * mais baixos não são dados e ficam de fora.
 */
static int decodificar(FILE *entrada, FILE *saida, const NoHuffman *raiz,
                       long bytes_dados, int lixo)
{
    const NoHuffman *atual = raiz;

    for (long i = 0; i < bytes_dados; i++) {
        int byte = fgetc(entrada);
        if (byte == EOF)
            return 0;

        int menor_bit = (i == bytes_dados - 1) ? lixo : 0;
        for (int bit = 7; bit >= menor_bit; bit--) {
            if (bits_esta_ligado((unsigned char) byte, bit))
                atual = atual->direita;
            else
                atual = atual->esquerda;

            if (huffman_eh_folha(atual)) {
                fputc(huffman_byte_da_folha(atual), saida);
                atual = raiz;
            }
        }
    }
    return atual == raiz;   /* se parou no meio de um código, o arquivo está corrompido */
}

int huffman_descompactar(FILE *entrada, FILE *saida)
{
    int lixo, tamanho_arvore;
    long total = tamanho_do_arquivo(entrada);

    if (!huffman_ler_cabecalho(entrada, &lixo, &tamanho_arvore))
        return 0;
    if (tamanho_arvore == 0)
        return total == 2 && lixo == 0;   /* arquivo original vazio: a saída fica vazia */

    long bytes_dados = total - 2 - tamanho_arvore;
    if (bytes_dados <= 0)
        return 0;

    int restantes = tamanho_arvore;
    NoHuffman *raiz = huffman_ler_arvore(entrada, &restantes);
    int ok = 0;
    /* a árvore tem que ocupar exatamente o tamanho do cabeçalho e ter ao menos um nó interno */
    if (raiz != NULL && restantes == 0 && !huffman_eh_folha(raiz))
        ok = decodificar(entrada, saida, raiz, bytes_dados, lixo);

    huffman_destruir_arvore(raiz);
    return ok;
}
