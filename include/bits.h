#ifndef BITS_H
#define BITS_H

#include <stdio.h>

/**
 * Diz se o bit i de um byte está ligado.
 * O bit 7 é o mais significativo (à esquerda) e o bit 0, o menos.
 *
 * @param byte  Byte a testar.
 * @param i     Posição do bit, de 0 a 7.
 * @return      Diferente de 0 se o bit está ligado; 0 se está desligado.
 */
int bits_esta_ligado(unsigned char byte, int i);

/**
 * Liga o bit i de um byte.
 *
 * @param byte  Byte original (não é alterado).
 * @param i     Posição do bit, de 0 a 7.
 * @return      Uma cópia do byte com o bit i ligado.
 */
unsigned char bits_ligar(unsigned char byte, int i);

/**
 * Junta bits soltos em bytes e grava cada byte completo em um arquivo.
 * Os bits entram do mais significativo (bit 7) para o menos (bit 0).
 * Não aloca memória: pode ser declarado como variável local.
 */
typedef struct {
    FILE *arquivo;
    unsigned char byte;   /* byte em montagem */
    int usados;           /* quantos bits do byte já foram preenchidos (0 a 7) */
} EscritorBits;

/**
 * Prepara o escritor para gravar em um arquivo.
 *
 * @param escritor  Escritor a preparar.
 * @param arquivo   Arquivo aberto pelo chamador em modo binário ("wb").
 */
void bits_iniciar_escritor(EscritorBits *escritor, FILE *arquivo);

/**
 * Acrescenta um bit. Quando o byte completa 8 bits, ele é gravado no arquivo.
 *
 * @param escritor  Escritor já iniciado.
 * @param bit       0 ou 1.
 */
void bits_gravar(EscritorBits *escritor, int bit);

/**
 * Grava o último byte, se estiver incompleto, com os bits que faltam em 0.
 *
 * @param escritor  Escritor já iniciado.
 * @return          Quantidade de bits de lixo do último byte (0 a 7).
 */
int bits_finalizar(EscritorBits *escritor);

#endif
