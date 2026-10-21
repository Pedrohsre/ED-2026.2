#include <CUnit/CUnit.h>
#include "testes.h"
#include "bits.h"

/* Exemplo do slide: 11000111 tem os bits 7, 6, 2, 1 e 0 ligados */
static void teste_esta_ligado(void)
{
    unsigned char byte = 0xC7;
    int esperado[8] = {1, 1, 1, 0, 0, 0, 1, 1};   /* do bit 0 ao bit 7 */

    for (int i = 0; i < 8; i++)
        CU_ASSERT_EQUAL(bits_esta_ligado(byte, i) != 0, esperado[i]);
}

/* Exemplo do slide: ligar o bit 4 de 11000111 dá 11010111 */
static void teste_ligar(void)
{
    CU_ASSERT_EQUAL(bits_ligar(0xC7, 4), 0xD7);
    CU_ASSERT_EQUAL(bits_ligar(0xC7, 7), 0xC7);   /* já estava ligado */
    CU_ASSERT_EQUAL(bits_ligar(0x00, 0), 0x01);
}

static void teste_escritor_byte_incompleto(void)
{
    FILE *arquivo = tmpfile();
    CU_ASSERT_PTR_NOT_NULL_FATAL(arquivo);
    EscritorBits escritor;
    bits_iniciar_escritor(&escritor, arquivo);

    bits_gravar(&escritor, 1);
    bits_gravar(&escritor, 0);
    bits_gravar(&escritor, 1);
    CU_ASSERT_EQUAL(bits_finalizar(&escritor), 5);   /* 101 + 5 bits de lixo */

    rewind(arquivo);
    CU_ASSERT_EQUAL(fgetc(arquivo), 0xA0);           /* 10100000 */
    CU_ASSERT_EQUAL(fgetc(arquivo), EOF);
    fclose(arquivo);
}

static void teste_escritor_byte_cheio(void)
{
    FILE *arquivo = tmpfile();
    CU_ASSERT_PTR_NOT_NULL_FATAL(arquivo);
    EscritorBits escritor;
    bits_iniciar_escritor(&escritor, arquivo);

    int bits[8] = {1, 1, 1, 1, 0, 0, 0, 0};
    for (int i = 0; i < 8; i++)
        bits_gravar(&escritor, bits[i]);
    CU_ASSERT_EQUAL(bits_finalizar(&escritor), 0);   /* sem lixo e sem byte extra */

    rewind(arquivo);
    CU_ASSERT_EQUAL(fgetc(arquivo), 0xF0);
    CU_ASSERT_EQUAL(fgetc(arquivo), EOF);
    fclose(arquivo);
}

int registrar_testes_bits(void)
{
    CU_pSuite suite = CU_add_suite("bits", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "testar bit", teste_esta_ligado) == NULL
        || CU_add_test(suite, "ligar bit", teste_ligar) == NULL
        || CU_add_test(suite, "escritor com byte incompleto", teste_escritor_byte_incompleto) == NULL
        || CU_add_test(suite, "escritor com byte cheio", teste_escritor_byte_cheio) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
