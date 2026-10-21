#include <CUnit/CUnit.h>
#include "testes.h"
#include "kruskal.h"

/* Roda o Kruskal no arquivo e confere o número de arestas e o peso total */
static void conferir(const char *caminho, int arestas_esperadas, int peso_esperado)
{
    char erro[256];
    Grafo *grafo = grafo_ler(caminho, erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(grafo);

    int peso_total;
    Lista *escolhidas = kruskal_calcular(grafo, &peso_total);
    CU_ASSERT_PTR_NOT_NULL_FATAL(escolhidas);
    CU_ASSERT_EQUAL(lista_tamanho(escolhidas), arestas_esperadas);
    CU_ASSERT_EQUAL(peso_total, peso_esperado);

    lista_destruir(escolhidas, NULL);   /* as arestas são do grafo */
    grafo_destruir(grafo);
}

/* Arestas 0-1 (2), 1-2 (3), 1-4 (5) e 0-3 (6): V - 1 = 4 arestas, peso 16 */
static void teste_grafo_conexo(void)
{
    conferir("data/grafo_exemplo.txt", 4, 16);
}

/* Duas componentes: floresta com 1-2 (1), 3-4 (2) e 0-1 (4) */
static void teste_grafo_desconexo(void)
{
    conferir("data/grafo_desconexo.txt", 3, 7);
}

/* Triângulo A-B (2), B-C (2), A-C (3): a aresta mais pesada fecharia um ciclo */
static void teste_nao_forma_ciclo(void)
{
    Grafo *grafo = grafo_criar(3);
    grafo_adicionar_aresta(grafo, 0, 1, 2);
    grafo_adicionar_aresta(grafo, 1, 2, 2);
    grafo_adicionar_aresta(grafo, 0, 2, 3);

    int peso_total;
    Lista *escolhidas = kruskal_calcular(grafo, &peso_total);
    CU_ASSERT_EQUAL(lista_tamanho(escolhidas), 2);
    CU_ASSERT_EQUAL(peso_total, 4);
    lista_destruir(escolhidas, NULL);
    grafo_destruir(grafo);
}

int registrar_testes_kruskal(void)
{
    CU_pSuite suite = CU_add_suite("kruskal", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "grafo conexo", teste_grafo_conexo) == NULL
        || CU_add_test(suite, "grafo desconexo", teste_grafo_desconexo) == NULL
        || CU_add_test(suite, "nao forma ciclo", teste_nao_forma_ciclo) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
