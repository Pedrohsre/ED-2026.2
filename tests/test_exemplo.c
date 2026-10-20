/*
 * Modelo de arquivo de teste. Para um módulo novo:
 *   1. copie este arquivo para tests/test_<modulo>.c e escreva os testes;
 *   2. declare registrar_testes_<modulo> em tests/testes.h;
 *   3. inclua a função na lista de tests/main_testes.c.
 */
#include <CUnit/CUnit.h>
#include "testes.h"

static void teste_soma(void)
{
    CU_ASSERT_EQUAL(2 + 2, 4);
}

int registrar_testes_exemplo(void)
{
    CU_pSuite suite = CU_add_suite("exemplo", NULL, NULL);
    if (suite == NULL || CU_add_test(suite, "soma", teste_soma) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
