#include <stdlib.h>
#include <CUnit/CUnit.h>
#include "testes.h"
#include "fp_heap.h"

static int menor_primeiro(const void *a, const void *b)
{
    int x = *(const int *) a;
    int y = *(const int *) b;
    return (x > y) - (x < y);
}

/* Os valores do exemplo de heapsort do slide, inseridos fora de ordem */
static void teste_sai_em_ordem(void)
{
    int valores[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    int esperado[] = {1, 2, 3, 4, 7, 8, 9, 10, 14, 16};
    FilaHeap *fila = fp_heap_criar(menor_primeiro);

    for (int i = 0; i < 10; i++)
        CU_ASSERT_TRUE(fp_heap_inserir(fila, &valores[i]));
    CU_ASSERT_EQUAL(fp_heap_tamanho(fila), 10);
    for (int i = 0; i < 10; i++)
        CU_ASSERT_EQUAL(*(int *) fp_heap_remover(fila), esperado[i]);
    CU_ASSERT_EQUAL(fp_heap_tamanho(fila), 0);
    fp_heap_destruir(fila, NULL);
}

static void teste_fila_vazia(void)
{
    FilaHeap *fila = fp_heap_criar(menor_primeiro);

    CU_ASSERT_EQUAL(fp_heap_tamanho(fila), 0);
    CU_ASSERT_PTR_NULL(fp_heap_remover(fila));
    fp_heap_destruir(fila, NULL);
    fp_heap_destruir(NULL, NULL);   /* não pode quebrar */
}

/* 1000 valores com repetidos: o vetor precisa crescer várias vezes */
static void teste_muitos_elementos(void)
{
    static int valores[1000];
    FilaHeap *fila = fp_heap_criar(menor_primeiro);

    for (int i = 0; i < 1000; i++) {
        valores[i] = (i * 37) % 101;
        CU_ASSERT_TRUE(fp_heap_inserir(fila, &valores[i]));
    }
    int anterior = -1, ordenado = 1;
    for (int i = 0; i < 1000; i++) {
        int atual = *(int *) fp_heap_remover(fila);
        if (atual < anterior)
            ordenado = 0;
        anterior = atual;
    }
    CU_ASSERT_TRUE(ordenado);
    fp_heap_destruir(fila, NULL);
}

/* O que sobra na fila é liberado pela função passada (o valgrind confere) */
static void teste_destruir_libera_os_dados(void)
{
    FilaHeap *fila = fp_heap_criar(menor_primeiro);

    for (int i = 0; i < 20; i++) {
        int *valor = malloc(sizeof(int));
        *valor = 20 - i;
        fp_heap_inserir(fila, valor);
    }
    free(fp_heap_remover(fila));   /* quem remove passa a ser o dono */
    fp_heap_destruir(fila, free);
}

int registrar_testes_fp_heap(void)
{
    CU_pSuite suite = CU_add_suite("fila de prioridade com heap", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "sai em ordem", teste_sai_em_ordem) == NULL
        || CU_add_test(suite, "fila vazia", teste_fila_vazia) == NULL
        || CU_add_test(suite, "muitos elementos", teste_muitos_elementos) == NULL
        || CU_add_test(suite, "destruir libera os dados", teste_destruir_libera_os_dados) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
