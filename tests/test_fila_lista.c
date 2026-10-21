#include <CUnit/CUnit.h>
#include "testes.h"
#include "filas.h"

static int menor_primeiro(const void *a, const void *b)
{
    int x = *(const int *) a;
    int y = *(const int *) b;
    return (x > y) - (x < y);
}

/* O exemplo do slide de filas de prioridade (a17, b12, c100, d22), menor primeiro */
static void teste_sai_em_ordem(void)
{
    int valores[] = {17, 12, 100, 22};
    int esperado[] = {12, 17, 22, 100};
    FilaLista *fila = fila_lista_criar(menor_primeiro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(fila);

    for (int i = 0; i < 4; i++)
        CU_ASSERT_TRUE(fila_lista_inserir(fila, &valores[i]));
    CU_ASSERT_EQUAL(fila_lista_tamanho(fila), 4);
    for (int i = 0; i < 4; i++)
        CU_ASSERT_EQUAL(*(int *) fila_lista_remover(fila), esperado[i]);
    CU_ASSERT_EQUAL(fila_lista_tamanho(fila), 0);
    fila_lista_destruir(fila);
}

/* Empates saem na ordem de chegada */
static void teste_empates(void)
{
    int a = 5, b = 5, c = 1;
    FilaLista *fila = fila_lista_criar(menor_primeiro);

    fila_lista_inserir(fila, &a);
    fila_lista_inserir(fila, &b);
    fila_lista_inserir(fila, &c);
    CU_ASSERT_PTR_EQUAL(fila_lista_remover(fila), &c);
    CU_ASSERT_PTR_EQUAL(fila_lista_remover(fila), &a);
    CU_ASSERT_PTR_EQUAL(fila_lista_remover(fila), &b);
    fila_lista_destruir(fila);
}

static void teste_fila_vazia(void)
{
    FilaLista *fila = fila_lista_criar(menor_primeiro);

    CU_ASSERT_EQUAL(fila_lista_tamanho(fila), 0);
    CU_ASSERT_PTR_NULL(fila_lista_remover(fila));
    fila_lista_destruir(fila);
    fila_lista_destruir(NULL);   /* não pode quebrar */
}

int registrar_testes_fila_lista(void)
{
    CU_pSuite suite = CU_add_suite("fila de prioridade sem heap", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "sai em ordem", teste_sai_em_ordem) == NULL
        || CU_add_test(suite, "empates", teste_empates) == NULL
        || CU_add_test(suite, "fila vazia", teste_fila_vazia) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
