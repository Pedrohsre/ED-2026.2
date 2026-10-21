#include <string.h>
#include <CUnit/CUnit.h>
#include "testes.h"
#include "huffman_arvore.h"

/* A árvore do slide para AAAAAABBBBBCCCCDDDEEF, montada à mão: **CB***FEDA */
static NoHuffman *arvore_do_slide(void)
{
    NoHuffman *c = huffman_criar_folha('C', 4);
    NoHuffman *b = huffman_criar_folha('B', 5);
    NoHuffman *f = huffman_criar_folha('F', 1);
    NoHuffman *e = huffman_criar_folha('E', 2);
    NoHuffman *d = huffman_criar_folha('D', 3);
    NoHuffman *a = huffman_criar_folha('A', 6);

    NoHuffman *no3 = huffman_criar_no_interno(f, e);
    NoHuffman *no6 = huffman_criar_no_interno(no3, d);
    NoHuffman *no9 = huffman_criar_no_interno(c, b);
    NoHuffman *no12 = huffman_criar_no_interno(no6, a);
    return huffman_criar_no_interno(no9, no12);
}

/* Grava a árvore em um arquivo temporário e devolve os bytes em buffer */
static size_t gravar_em_buffer(const NoHuffman *raiz, char *buffer, size_t max)
{
    FILE *arquivo = tmpfile();
    huffman_gravar_arvore(raiz, arquivo);
    rewind(arquivo);
    size_t lidos = fread(buffer, 1, max, arquivo);
    fclose(arquivo);
    return lidos;
}

static void teste_folha_e_no_interno(void)
{
    NoHuffman *raiz = arvore_do_slide();
    CU_ASSERT_PTR_NOT_NULL_FATAL(raiz);
    CU_ASSERT_EQUAL(raiz->frequencia, 21);
    CU_ASSERT_FALSE(huffman_eh_folha(raiz));
    CU_ASSERT_TRUE(huffman_eh_folha(raiz->esquerda->esquerda));
    CU_ASSERT_EQUAL(huffman_byte_da_folha(raiz->esquerda->esquerda), 'C');
    huffman_destruir_arvore(raiz);
}

static void teste_gravar_arvore_do_slide(void)
{
    NoHuffman *raiz = arvore_do_slide();
    char buffer[32];

    CU_ASSERT_EQUAL(huffman_tamanho_arvore(raiz), 11);
    CU_ASSERT_EQUAL(gravar_em_buffer(raiz, buffer, sizeof buffer), 11);
    CU_ASSERT(memcmp(buffer, "**CB***FEDA", 11) == 0);
    huffman_destruir_arvore(raiz);
}

static void teste_ida_e_volta(void)
{
    NoHuffman *original = arvore_do_slide();
    FILE *arquivo = tmpfile();
    huffman_gravar_arvore(original, arquivo);
    rewind(arquivo);

    int restantes = 11;
    NoHuffman *lida = huffman_ler_arvore(arquivo, &restantes);
    CU_ASSERT_PTR_NOT_NULL_FATAL(lida);
    CU_ASSERT_EQUAL(restantes, 0);

    char buffer[32];
    CU_ASSERT_EQUAL(gravar_em_buffer(lida, buffer, sizeof buffer), 11);
    CU_ASSERT(memcmp(buffer, "**CB***FEDA", 11) == 0);

    fclose(arquivo);
    huffman_destruir_arvore(original);
    huffman_destruir_arvore(lida);
}

/* Folhas "*" e "\" ganham o escape, e o escape conta no tamanho */
static void teste_escape(void)
{
    NoHuffman *raiz = huffman_criar_no_interno(huffman_criar_folha('*', 1),
                                               huffman_criar_folha('\\', 1));
    char buffer[16];

    CU_ASSERT_EQUAL(huffman_tamanho_arvore(raiz), 5);
    CU_ASSERT_EQUAL(gravar_em_buffer(raiz, buffer, sizeof buffer), 5);
    CU_ASSERT(memcmp(buffer, "*\\*\\\\", 5) == 0);

    FILE *arquivo = tmpfile();
    huffman_gravar_arvore(raiz, arquivo);
    rewind(arquivo);
    int restantes = 5;
    NoHuffman *lida = huffman_ler_arvore(arquivo, &restantes);
    CU_ASSERT_PTR_NOT_NULL_FATAL(lida);
    CU_ASSERT_EQUAL(huffman_byte_da_folha(lida->esquerda), '*');
    CU_ASSERT_EQUAL(huffman_byte_da_folha(lida->direita), '\\');

    fclose(arquivo);
    huffman_destruir_arvore(raiz);
    huffman_destruir_arvore(lida);
}

/* Árvore cortada no meio: devolve NULL sem vazar memória (o valgrind confere) */
static void teste_arvore_incompleta(void)
{
    FILE *arquivo = tmpfile();
    fputs("**CB*", arquivo);
    rewind(arquivo);

    int restantes = 5;
    CU_ASSERT_PTR_NULL(huffman_ler_arvore(arquivo, &restantes));
    fclose(arquivo);
}

int registrar_testes_huffman_arvore(void)
{
    CU_pSuite suite = CU_add_suite("huffman: arvore", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "folha e no interno", teste_folha_e_no_interno) == NULL
        || CU_add_test(suite, "grava a arvore do slide", teste_gravar_arvore_do_slide) == NULL
        || CU_add_test(suite, "grava e le de volta", teste_ida_e_volta) == NULL
        || CU_add_test(suite, "escape de * e \\", teste_escape) == NULL
        || CU_add_test(suite, "arvore incompleta", teste_arvore_incompleta) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
