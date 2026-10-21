#include <stdlib.h>
#include <CUnit/CUnit.h>
#include "testes.h"
#include "lista.h"

static int comparar_inteiros(const void *a, const void *b)
{
    int x = *(const int *) a;
    int y = *(const int *) b;
    return (x > y) - (x < y);
}

static void teste_inserir_inicio_e_fim(void)
{
    int a = 1, b = 2, c = 3;
    Lista *lista = lista_criar();
    CU_ASSERT_PTR_NOT_NULL_FATAL(lista);
    CU_ASSERT_TRUE(lista_vazia(lista));

    lista_inserir_fim(lista, &b);
    lista_inserir_inicio(lista, &a);
    lista_inserir_fim(lista, &c);
    CU_ASSERT_EQUAL(lista_tamanho(lista), 3);
    CU_ASSERT_PTR_EQUAL(lista_remover_inicio(lista), &a);
    CU_ASSERT_PTR_EQUAL(lista_remover_inicio(lista), &b);
    CU_ASSERT_PTR_EQUAL(lista_remover_inicio(lista), &c);
    CU_ASSERT_PTR_NULL(lista_remover_inicio(lista));   /* lista vazia */
    lista_destruir(lista, NULL);
}

/* Em ordem crescente; empates saem na ordem de chegada */
static void teste_inserir_ordenado(void)
{
    int valores[] = {5, 1, 3, 1, 4};
    int esperado[] = {1, 1, 3, 4, 5};
    Lista *lista = lista_criar();

    for (int i = 0; i < 5; i++)
        lista_inserir_ordenado(lista, &valores[i], comparar_inteiros);
    CU_ASSERT_PTR_EQUAL(lista->inicio->dado, &valores[1]);   /* o primeiro 1 continua antes */
    for (int i = 0; i < 5; i++)
        CU_ASSERT_EQUAL(*(int *) lista_remover_inicio(lista), esperado[i]);
    lista_destruir(lista, NULL);
}

static void teste_buscar(void)
{
    int valores[] = {10, 20, 30}, procurado = 20, ausente = 99;
    Lista *lista = lista_criar();

    for (int i = 0; i < 3; i++)
        lista_inserir_fim(lista, &valores[i]);
    CU_ASSERT_PTR_EQUAL(lista_buscar(lista, &procurado, comparar_inteiros), &valores[1]);
    CU_ASSERT_PTR_NULL(lista_buscar(lista, &ausente, comparar_inteiros));
    lista_destruir(lista, NULL);
}

/* Os dados que sobram são liberados pela função passada (o valgrind confere) */
static void teste_destruir_libera_os_dados(void)
{
    Lista *lista = lista_criar();

    for (int i = 0; i < 10; i++) {
        int *valor = malloc(sizeof(int));
        *valor = i;
        lista_inserir_fim(lista, valor);
    }
    free(lista_remover_inicio(lista));
    lista_destruir(lista, free);
    lista_destruir(NULL, NULL);   /* não pode quebrar */
}

int registrar_testes_lista(void)
{
    CU_pSuite suite = CU_add_suite("lista", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "inserir no inicio e no fim", teste_inserir_inicio_e_fim) == NULL
        || CU_add_test(suite, "inserir ordenado com empates", teste_inserir_ordenado) == NULL
        || CU_add_test(suite, "buscar", teste_buscar) == NULL
        || CU_add_test(suite, "destruir libera os dados", teste_destruir_libera_os_dados) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
