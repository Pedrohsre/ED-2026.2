#include <stdlib.h>
#include <CUnit/CUnit.h>
#include "testes.h"
#include "fp_heap.h"

/* Item com prioridade e um número para identificar quem saiu */
typedef struct {
    int prioridade;
    int id;
} Item;

static int menor_prioridade_primeiro(const void *a, const void *b)
{
    const Item *x = a;
    const Item *y = b;
    return (x->prioridade > y->prioridade) - (x->prioridade < y->prioridade);
}

/* O exemplo do slide de filas de prioridade: a(17), b(12), c(100), d(22) */
static void teste_ordem_de_saida(void)
{
    Item itens[] = {{17, 'a'}, {12, 'b'}, {100, 'c'}, {22, 'd'}};
    int esperado[] = {'b', 'a', 'd', 'c'};
    FilaHeap *fila = fp_heap_criar(menor_prioridade_primeiro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(fila);

    for (int i = 0; i < 4; i++)
        CU_ASSERT_TRUE(fp_heap_inserir(fila, &itens[i]));
    CU_ASSERT_EQUAL(fp_heap_tamanho(fila), 4);

    for (int i = 0; i < 4; i++)
        CU_ASSERT_EQUAL(((Item *) fp_heap_remover(fila))->id, esperado[i]);
    CU_ASSERT_EQUAL(fp_heap_tamanho(fila), 0);
    fp_heap_destruir(fila, NULL);
}

/* Com empates, todos saem uma vez, e a prioridade nunca diminui */
static void teste_empates(void)
{
    Item itens[8] = {{5, 0}, {1, 1}, {5, 2}, {1, 3}, {3, 4}, {5, 5}, {3, 6}, {1, 7}};
    int saiu[8] = {0};
    FilaHeap *fila = fp_heap_criar(menor_prioridade_primeiro);

    for (int i = 0; i < 8; i++)
        fp_heap_inserir(fila, &itens[i]);

    int anterior = 0, ordenado = 1;
    for (int i = 0; i < 8; i++) {
        Item *item = fp_heap_remover(fila);
        if (item->prioridade < anterior)
            ordenado = 0;
        anterior = item->prioridade;
        saiu[item->id]++;
    }
    CU_ASSERT_TRUE(ordenado);
    for (int i = 0; i < 8; i++)
        CU_ASSERT_EQUAL(saiu[i], 1);
    fp_heap_destruir(fila, NULL);
}

static void teste_fila_vazia(void)
{
    FilaHeap *fila = fp_heap_criar(menor_prioridade_primeiro);

    CU_ASSERT_EQUAL(fp_heap_tamanho(fila), 0);
    CU_ASSERT_PTR_NULL(fp_heap_remover(fila));
    fp_heap_destruir(fila, NULL);
    fp_heap_destruir(NULL, NULL);   /* não pode quebrar */
}

static void teste_destruir_libera_os_dados(void)
{
    FilaHeap *fila = fp_heap_criar(menor_prioridade_primeiro);

    for (int i = 0; i < 20; i++) {
        Item *item = malloc(sizeof(Item));
        item->prioridade = i % 4;
        item->id = i;
        fp_heap_inserir(fila, item);
    }
    free(fp_heap_remover(fila));
    fp_heap_destruir(fila, free);   /* o valgrind confere que nada vazou */
}

int registrar_testes_fp_heap(void)
{
    CU_pSuite suite = CU_add_suite("fila de prioridade com heap", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "ordem de saida", teste_ordem_de_saida) == NULL
        || CU_add_test(suite, "empates", teste_empates) == NULL
        || CU_add_test(suite, "fila vazia", teste_fila_vazia) == NULL
        || CU_add_test(suite, "destruir libera os dados", teste_destruir_libera_os_dados) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
