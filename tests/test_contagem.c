#include "contagem.h"
#include "filas.h"
#include "fp_heap.h"
#include "testes.h"
#include <CUnit/Basic.h>
#include <limits.h>

static void testar_sequencia(const int *valores, size_t quantidade,
                            const size_t *lista_esperada, const size_t *heap_esperado)
{
    size_t lista_contador = 0, heap_contador = 0;
    ElementoContagem lista_dados[64], heap_dados[64];
    FilaLista *lista = fila_lista_criar(contagem_comparar);
    FilaHeap *heap = fp_heap_criar(contagem_comparar);
    CU_ASSERT_PTR_NOT_NULL(lista);
    CU_ASSERT_PTR_NOT_NULL(heap);
    if (lista && heap) {
        for (size_t i = 0; i < quantidade; i++) {
            lista_dados[i] = (ElementoContagem){valores[i], &lista_contador};
            heap_dados[i] = (ElementoContagem){valores[i], &heap_contador};
            lista_contador = heap_contador = 0;
            CU_ASSERT(fila_lista_inserir(lista, &lista_dados[i]));
            CU_ASSERT(fp_heap_inserir(heap, &heap_dados[i]));
            CU_ASSERT_EQUAL(lista_contador, lista_esperada[i]);
            CU_ASSERT_EQUAL(heap_contador, heap_esperado[i]);
        }
    }
    fila_lista_destruir(lista);
    fp_heap_destruir(heap, NULL);
}

static void testar_contagens(void)
{
    int valores[] = {3, 1, 2, 2, 4};
    size_t lista[] = {0, 1, 2, 3, 4}, heap[] = {0, 1, 1, 2, 1};
    testar_sequencia(valores, 5, lista, heap);
    for (size_t i = 0; i < 5; i++) {
        valores[i] = 7;
        lista[i] = i;
        heap[i] = i ? 1 : 0;
    }
    testar_sequencia(valores, 5, lista, heap);
}

static void testar_crescimento(void)
{
    int valores[64];
    size_t lista[64], heap[64];
    for (size_t i = 0; i < 64; i++) {
        size_t tamanho = i + 1, profundidade = 0;
        while ((tamanho /= 2) > 0)
            profundidade++;
        valores[i] = 64 - (int)i;
        lista[i] = i ? 1 : 0;
        heap[i] = profundidade;
    }
    testar_sequencia(valores, 64, lista, heap);
    for (size_t i = 0; i < 64; i++) {
        valores[i] = (int)i;
        lista[i] = i;
        heap[i] = i ? 1 : 0;
    }
    testar_sequencia(valores, 64, lista, heap);
}

static void testar_comparador(void)
{
    size_t contador = 0;
    ElementoContagem a = {INT_MIN, &contador}, b = {INT_MAX, &contador};
    CU_ASSERT(contagem_comparar(&a, &b) < 0);
    CU_ASSERT(contagem_comparar(&b, &a) > 0);
    CU_ASSERT_EQUAL(contagem_comparar(&a, &a), 0);
    CU_ASSERT_EQUAL(contador, 3);
}

static void testar_csv(void)
{
    FILE *a = tmpfile(), *b = tmpfile();
    char linha[128];
    CU_ASSERT_PTR_NOT_NULL(a);
    CU_ASSERT_PTR_NOT_NULL(b);
    if (a && b) {
        CU_ASSERT(contagem_gerar(a, 64, 42));
        CU_ASSERT(contagem_gerar(b, 64, 42));
        rewind(a);
        rewind(b);
        CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof linha, a));
        CU_ASSERT_STRING_EQUAL(linha, "n,comparacoes_sem_heap,comparacoes_com_heap\n");
        CU_ASSERT_PTR_NOT_NULL(fgets(linha, sizeof linha, a));
        CU_ASSERT_STRING_EQUAL(linha, "0,0,0\n");
        rewind(a);
        do {
            int primeiro = fgetc(a), segundo = fgetc(b);
            CU_ASSERT_EQUAL(primeiro, segundo);
            if (primeiro == EOF || segundo == EOF)
                break;
        } while (1);
    }
    if (a) fclose(a);
    if (b) fclose(b);
}

int registrar_testes_contagem(void)
{
    CU_pSuite suite = CU_add_suite("Contagem", NULL, NULL);
    if (!suite || !CU_add_test(suite, "contagens e empates", testar_contagens) ||
        !CU_add_test(suite, "crescimento e limites", testar_crescimento) ||
        !CU_add_test(suite, "comparador", testar_comparador) ||
        !CU_add_test(suite, "CSV e semente", testar_csv)) {
        return CU_get_error();
    }
    return CUE_SUCCESS;
}
