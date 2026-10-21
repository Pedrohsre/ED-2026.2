#include <CUnit/CUnit.h>
#include "testes.h"
#include "dijkstra.h"

/* Confere distância e anterior de cada vértice */
static void conferir(const CaminhosMinimos *caminhos, const int *distancia, const int *anterior)
{
    for (int v = 0; v < caminhos->num_vertices; v++) {
        CU_ASSERT_EQUAL(caminhos->distancia[v], distancia[v]);
        CU_ASSERT_EQUAL(caminhos->anterior[v], anterior[v]);
    }
}

/* data/grafo_exemplo.txt a partir de 0: o vértice 4 é alcançado por 0 -> 1 -> 4 (2 + 5) */
static void teste_grafo_de_exemplo(void)
{
    char erro[256];
    Grafo *grafo = grafo_ler("data/grafo_exemplo.txt", erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(grafo);
    CaminhosMinimos *caminhos = dijkstra_calcular(grafo, 0, erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(caminhos);

    int distancia[] = {0, 2, 5, 6, 7};
    int anterior[] = {-1, 0, 1, 0, 1};
    conferir(caminhos, distancia, anterior);
    dijkstra_destruir(caminhos);
    grafo_destruir(grafo);
}

/*
 * data/grafo_dijkstra.txt: aqui o vértice 1 entra na fila com 4 e depois com 3,
 * então a entrada antiga (4, 1) sai da fila desatualizada e é ignorada.
 */
static void teste_entradas_desatualizadas(void)
{
    char erro[256];
    Grafo *grafo = grafo_ler("data/grafo_dijkstra.txt", erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(grafo);
    CaminhosMinimos *caminhos = dijkstra_calcular(grafo, 0, erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(caminhos);

    int distancia[] = {0, 3, 1, 4, 7};
    int anterior[] = {-1, 2, 0, 1, 3};
    conferir(caminhos, distancia, anterior);
    dijkstra_destruir(caminhos);
    grafo_destruir(grafo);
}

static void teste_grafo_desconexo(void)
{
    char erro[256];
    Grafo *grafo = grafo_ler("data/grafo_desconexo.txt", erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(grafo);
    CaminhosMinimos *caminhos = dijkstra_calcular(grafo, 0, erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(caminhos);

    int distancia[] = {0, 4, 5, DIJKSTRA_INALCANCAVEL, DIJKSTRA_INALCANCAVEL};
    int anterior[] = {-1, 0, 1, -1, -1};
    conferir(caminhos, distancia, anterior);
    dijkstra_destruir(caminhos);
    grafo_destruir(grafo);
}

/* Triângulo A-B (2), B-C (2), A-C (3): o menor caminho até C é a aresta direta */
static void teste_nao_e_a_arvore_geradora_minima(void)
{
    char erro[256];
    Grafo *grafo = grafo_criar(3);
    grafo_adicionar_aresta(grafo, 0, 1, 2);
    grafo_adicionar_aresta(grafo, 1, 2, 2);
    grafo_adicionar_aresta(grafo, 0, 2, 3);

    CaminhosMinimos *caminhos = dijkstra_calcular(grafo, 0, erro, sizeof erro);
    CU_ASSERT_EQUAL(caminhos->distancia[2], 3);
    CU_ASSERT_EQUAL(caminhos->anterior[2], 0);
    dijkstra_destruir(caminhos);
    grafo_destruir(grafo);
}

static void teste_entradas_invalidas(void)
{
    char erro[256];
    Grafo *grafo = grafo_criar(2);
    grafo_adicionar_aresta(grafo, 0, 1, 5);

    CU_ASSERT_PTR_NULL(dijkstra_calcular(grafo, 2, erro, sizeof erro));   /* origem fora */
    CU_ASSERT_PTR_NULL(dijkstra_calcular(grafo, -1, erro, sizeof erro));
    grafo_adicionar_aresta(grafo, 1, 0, -3);
    CU_ASSERT_PTR_NULL(dijkstra_calcular(grafo, 0, erro, sizeof erro));   /* peso negativo */
    grafo_destruir(grafo);
}

int registrar_testes_dijkstra(void)
{
    CU_pSuite suite = CU_add_suite("dijkstra", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "grafo de exemplo", teste_grafo_de_exemplo) == NULL
        || CU_add_test(suite, "entradas desatualizadas", teste_entradas_desatualizadas) == NULL
        || CU_add_test(suite, "grafo desconexo", teste_grafo_desconexo) == NULL
        || CU_add_test(suite, "nao e a arvore geradora minima", teste_nao_e_a_arvore_geradora_minima) == NULL
        || CU_add_test(suite, "entradas invalidas", teste_entradas_invalidas) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
