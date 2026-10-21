#include <stdlib.h>
#include <CUnit/CUnit.h>
#include "testes.h"
#include "vetor.h"

/* 100 elementos: o vetor precisa crescer e manter a ordem de inserção */
static void teste_adicionar_e_obter(void)
{
    int valores[100];
    Vetor *vetor = vetor_criar();
    CU_ASSERT_PTR_NOT_NULL_FATAL(vetor);

    for (int i = 0; i < 100; i++) {
        valores[i] = i * 10;
        CU_ASSERT_TRUE(vetor_adicionar(vetor, &valores[i]));
    }
    CU_ASSERT_EQUAL(vetor_tamanho(vetor), 100);
    CU_ASSERT_PTR_EQUAL(vetor_obter(vetor, 0), &valores[0]);
    CU_ASSERT_EQUAL(*(int *) vetor_obter(vetor, 99), 990);
    vetor_destruir(vetor, NULL);   /* os dados são do vetor local valores */
}

/* O que está no vetor é liberado pela função passada (o valgrind confere) */
static void teste_destruir_libera_os_dados(void)
{
    Vetor *vetor = vetor_criar();

    for (int i = 0; i < 10; i++) {
        int *valor = malloc(sizeof(int));
        *valor = i;
        vetor_adicionar(vetor, valor);
    }
    vetor_destruir(vetor, free);
    vetor_destruir(NULL, NULL);   /* não pode quebrar */
}

int registrar_testes_vetor(void)
{
    CU_pSuite suite = CU_add_suite("vetor", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "adicionar e obter", teste_adicionar_e_obter) == NULL
        || CU_add_test(suite, "destruir libera os dados", teste_destruir_libera_os_dados) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
