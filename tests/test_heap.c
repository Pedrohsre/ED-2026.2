#include <stdlib.h>
#include <CUnit/CUnit.h>
#include "testes.h"
#include "heap.h"

static int menor_primeiro(const void *a, const void *b)
{
    int x = *(const int *) a;
    int y = *(const int *) b;
    return (x > y) - (x < y);
}

static int maior_primeiro(const void *a, const void *b)
{
    return menor_primeiro(b, a);   /* comparador invertido: max-heap, como no slide */
}

/* Valores do exemplo de heapsort do slide, inseridos fora de ordem */
static void teste_sai_em_ordem(void)
{
    int valores[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    int esperado[] = {1, 2, 3, 4, 7, 8, 9, 10, 14, 16};
    Heap *heap = heap_criar(menor_primeiro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(heap);

    for (int i = 0; i < 10; i++)
        CU_ASSERT_TRUE(heap_inserir(heap, &valores[i]));
    CU_ASSERT_EQUAL(heap_tamanho(heap), 10);
    CU_ASSERT_EQUAL(*(int *) heap_topo(heap), 1);

    for (int i = 0; i < 10; i++)
        CU_ASSERT_EQUAL(*(int *) heap_remover(heap), esperado[i]);
    CU_ASSERT_TRUE(heap_vazio(heap));
    heap_destruir(heap, NULL);
}

static void teste_max_heap(void)
{
    int valores[] = {20, 10, 15, 8, 7, 13, 14, 2, 5, 6};
    Heap *heap = heap_criar(maior_primeiro);

    for (int i = 0; i < 10; i++)
        heap_inserir(heap, &valores[i]);
    CU_ASSERT_EQUAL(*(int *) heap_remover(heap), 20);
    CU_ASSERT_EQUAL(*(int *) heap_remover(heap), 15);
    CU_ASSERT_EQUAL(*(int *) heap_topo(heap), 14);
    heap_destruir(heap, NULL);
}

static void teste_heap_vazio(void)
{
    Heap *heap = heap_criar(menor_primeiro);

    CU_ASSERT_TRUE(heap_vazio(heap));
    CU_ASSERT_EQUAL(heap_tamanho(heap), 0);
    CU_ASSERT_PTR_NULL(heap_topo(heap));
    CU_ASSERT_PTR_NULL(heap_remover(heap));
    heap_destruir(heap, NULL);
    heap_destruir(NULL, NULL);   /* não pode quebrar */
}

/* 1000 elementos, com repetidos: força o realloc (a capacidade inicial é 16) */
static void teste_muitos_elementos(void)
{
    static int valores[1000];
    unsigned int semente = 7;
    Heap *heap = heap_criar(menor_primeiro);

    for (int i = 0; i < 1000; i++) {
        semente = semente * 1103515245u + 12345u;
        valores[i] = (int) ((semente >> 16) % 300);
        CU_ASSERT_TRUE(heap_inserir(heap, &valores[i]));
    }
    CU_ASSERT_EQUAL(heap_tamanho(heap), 1000);

    int anterior = -1, removidos = 0, ordenado = 1;
    while (!heap_vazio(heap)) {
        int atual = *(int *) heap_remover(heap);
        if (atual < anterior)
            ordenado = 0;
        anterior = atual;
        removidos++;
    }
    CU_ASSERT_TRUE(ordenado);
    CU_ASSERT_EQUAL(removidos, 1000);
    heap_destruir(heap, NULL);
}

/* Os elementos que sobram são liberados pela função passada (o valgrind confere) */
static void teste_destruir_libera_os_dados(void)
{
    Heap *heap = heap_criar(menor_primeiro);

    for (int i = 0; i < 50; i++) {
        int *valor = malloc(sizeof(int));
        *valor = 50 - i;
        heap_inserir(heap, valor);
    }
    free(heap_remover(heap));   /* quem remove passa a ser o dono */
    heap_destruir(heap, free);
}

/* Elemento que carrega o contador de comparações (sem variável global) */
typedef struct {
    int valor;
    int *comparacoes;
} Contado;

static int comparar_contando(const void *a, const void *b)
{
    const Contado *x = a;
    const Contado *y = b;
    (*x->comparacoes)++;
    return (x->valor > y->valor) - (x->valor < y->valor);
}

static int piso_log2(int n)
{
    int resultado = 0;
    while (n > 1) {
        n /= 2;
        resultado++;
    }
    return resultado;
}

/* Inserir com n elementos no heap faz no máximo piso(log2(n + 1)) comparações */
static void teste_comparacoes_por_insercao(void)
{
    static Contado elementos[2000];
    int comparacoes = 0, dentro_do_limite = 1;
    unsigned int semente = 99;
    Heap *heap = heap_criar(comparar_contando);

    for (int n = 0; n < 2000; n++) {
        semente = semente * 1103515245u + 12345u;
        elementos[n].valor = (int) (semente >> 16);
        elementos[n].comparacoes = &comparacoes;
        comparacoes = 0;
        heap_inserir(heap, &elementos[n]);
        if (comparacoes > piso_log2(n + 1))
            dentro_do_limite = 0;
    }
    CU_ASSERT_TRUE(dentro_do_limite);
    heap_destruir(heap, NULL);
}

int registrar_testes_heap(void)
{
    CU_pSuite suite = CU_add_suite("heap", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "insercoes fora de ordem saem em ordem", teste_sai_em_ordem) == NULL
        || CU_add_test(suite, "max-heap com comparador invertido", teste_max_heap) == NULL
        || CU_add_test(suite, "heap vazio", teste_heap_vazio) == NULL
        || CU_add_test(suite, "muitos elementos (realloc)", teste_muitos_elementos) == NULL
        || CU_add_test(suite, "destruir libera os dados", teste_destruir_libera_os_dados) == NULL
        || CU_add_test(suite, "comparacoes por insercao", teste_comparacoes_por_insercao) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
