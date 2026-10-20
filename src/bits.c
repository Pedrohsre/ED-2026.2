#include "bits.h"

int bits_esta_ligado(unsigned char byte, int i)
{
    unsigned char mascara = 1 << i;
    return mascara & byte;
}

unsigned char bits_ligar(unsigned char byte, int i)
{
    unsigned char mascara = 1 << i;
    return mascara | byte;
}

void bits_iniciar_escritor(EscritorBits *escritor, FILE *arquivo)
{
    escritor->arquivo = arquivo;
    escritor->byte = 0;
    escritor->usados = 0;
}

void bits_gravar(EscritorBits *escritor, int bit)
{
    /* o 1º bit vai para a posição 7, o 2º para a 6, e assim por diante */
    if (bit)
        escritor->byte = bits_ligar(escritor->byte, 7 - escritor->usados);
    escritor->usados++;

    if (escritor->usados == 8) {
        fputc(escritor->byte, escritor->arquivo);
        escritor->byte = 0;
        escritor->usados = 0;
    }
}

int bits_finalizar(EscritorBits *escritor)
{
    if (escritor->usados == 0)
        return 0;   /* o último byte já saiu cheio: não há lixo */

    int lixo = 8 - escritor->usados;
    fputc(escritor->byte, escritor->arquivo);   /* os bits que sobraram já estão em 0 */
    escritor->byte = 0;
    escritor->usados = 0;
    return lixo;
}
