#include <stddef.h>
#include <CUnit/Basic.h>
#include "testes.h"

/* Uma função de registro por arquivo de teste (declaradas em testes.h) */
static int (*const registros[])(void) = {
    registrar_testes_exemplo,
};

int main(void)
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return 1;

    int erro = CUE_SUCCESS;
    for (size_t i = 0; i < sizeof registros / sizeof registros[0] && erro == CUE_SUCCESS; i++)
        erro = registros[i]();

    unsigned int falhas = 0;
    if (erro == CUE_SUCCESS) {
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
        falhas = CU_get_number_of_failures();
    }
    CU_cleanup_registry();

    /* Diferente de 0 quando um teste falha: assim o make test acusa o erro */
    return (erro != CUE_SUCCESS || falhas > 0) ? 1 : 0;
}
