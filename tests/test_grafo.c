#include <stdio.h>
#include <CUnit/CUnit.h>
#include "testes.h"
#include "grafo.h"

static void teste_ler_grafo_de_exemplo(void)
{
    char erro[256];
    Grafo *grafo = grafo_ler("data/grafo_exemplo.txt", erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(grafo);

    CU_ASSERT_EQUAL(grafo_num_vertices(grafo), 5);
    CU_ASSERT_EQUAL(grafo_num_arestas(grafo), 7);
    CU_ASSERT_EQUAL(lista_tamanho(grafo_adjacentes(grafo, 1)), 4);   /* 1 liga em 0, 2, 3 e 4 */
    CU_ASSERT_EQUAL(lista_tamanho(grafo_adjacentes(grafo, 0)), 2);

    const Aresta *primeira = grafo_arestas(grafo)->inicio->dado;     /* "0 1 2" */
    CU_ASSERT_EQUAL(primeira->peso, 2);
    CU_ASSERT_EQUAL(aresta_outro_extremo(primeira, 0), 1);
    CU_ASSERT_EQUAL(aresta_outro_extremo(primeira, 1), 0);
    grafo_destruir(grafo);
}

static void teste_grafo_desconexo(void)
{
    char erro[256];
    Grafo *grafo = grafo_ler("data/grafo_desconexo.txt", erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(grafo);
    CU_ASSERT_EQUAL(grafo_num_vertices(grafo), 5);
    CU_ASSERT_EQUAL(grafo_num_arestas(grafo), 3);
    grafo_destruir(grafo);
}

/* Grava um arquivo temporário e tenta lê-lo como grafo */
static Grafo *ler_texto(const char *texto)
{
    char erro[256];
    const char *caminho = "build/grafo_teste.txt";
    FILE *arquivo = fopen(caminho, "w");
    if (arquivo == NULL)
        return NULL;
    fputs(texto, arquivo);
    fclose(arquivo);
    Grafo *grafo = grafo_ler(caminho, erro, sizeof erro);
    remove(caminho);
    return grafo;
}

static void teste_arquivos_invalidos(void)
{
    char erro[256];
    CU_ASSERT_PTR_NULL(grafo_ler("data/nao_existe.txt", erro, sizeof erro));
    CU_ASSERT_PTR_NULL(ler_texto("0 0\n"));               /* nenhum vértice */
    CU_ASSERT_PTR_NULL(ler_texto("3 2\n0 1 4\n"));        /* falta uma aresta */
    CU_ASSERT_PTR_NULL(ler_texto("3 1\n0 5 4\n"));        /* vértice fora de 0..2 */
    CU_ASSERT_PTR_NULL(ler_texto("3 1\n0 1 4\n1 2 7\n")); /* aresta a mais */
}

int registrar_testes_grafo(void)
{
    CU_pSuite suite = CU_add_suite("grafo", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "le o grafo de exemplo", teste_ler_grafo_de_exemplo) == NULL
        || CU_add_test(suite, "grafo desconexo", teste_grafo_desconexo) == NULL
        || CU_add_test(suite, "arquivos invalidos", teste_arquivos_invalidos) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
