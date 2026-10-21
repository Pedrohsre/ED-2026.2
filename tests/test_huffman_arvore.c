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

/* Nenhum código pode ser prefixo de outro */
static int livre_de_prefixo(const TabelaCodigos *tabela)
{
    for (int a = 0; a < 256; a++)
        for (int b = 0; b < 256; b++) {
            size_t n = strlen(tabela->codigo[a]);
            if (a != b && n > 0 && strncmp(tabela->codigo[a], tabela->codigo[b], n) == 0)
                return 0;
        }
    return 1;
}

static void teste_construir_arvore_do_slide(void)
{
    unsigned long long freq[256] = {0};
    freq['A'] = 6; freq['B'] = 5; freq['C'] = 4; freq['D'] = 3; freq['E'] = 2; freq['F'] = 1;

    NoHuffman *raiz = huffman_construir_arvore(freq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(raiz);
    CU_ASSERT_EQUAL(raiz->frequencia, 21);
    CU_ASSERT_EQUAL(huffman_tamanho_arvore(raiz), 11);   /* 6 folhas e 5 nós internos */

    char buffer[32];
    CU_ASSERT_EQUAL(gravar_em_buffer(raiz, buffer, sizeof buffer), 11);
    CU_ASSERT(memcmp(buffer, "**CB***FEDA", 11) == 0);   /* a mesma árvore do slide */

    TabelaCodigos tabela;
    huffman_montar_codigos(raiz, &tabela);
    CU_ASSERT_STRING_EQUAL(tabela.codigo['A'], "11");    /* a tabela do slide */
    CU_ASSERT_STRING_EQUAL(tabela.codigo['B'], "01");
    CU_ASSERT_STRING_EQUAL(tabela.codigo['C'], "00");
    CU_ASSERT_STRING_EQUAL(tabela.codigo['D'], "101");
    CU_ASSERT_STRING_EQUAL(tabela.codigo['E'], "1001");
    CU_ASSERT_STRING_EQUAL(tabela.codigo['F'], "1000");
    CU_ASSERT_STRING_EQUAL(tabela.codigo['Z'], "");

    unsigned long long total_bits = 0;
    for (int b = 0; b < 256; b++)
        total_bits += freq[b] * strlen(tabela.codigo[b]);
    CU_ASSERT_EQUAL(total_bits, 51);
    CU_ASSERT_TRUE(livre_de_prefixo(&tabela));
    huffman_destruir_arvore(raiz);
}

static void teste_construir_um_byte_so(void)
{
    unsigned long long freq[256] = {0};
    freq['A'] = 10;

    NoHuffman *raiz = huffman_construir_arvore(freq);
    CU_ASSERT_PTR_NOT_NULL_FATAL(raiz);
    CU_ASSERT_FALSE(huffman_eh_folha(raiz));
    CU_ASSERT_EQUAL(huffman_tamanho_arvore(raiz), 3);   /* raiz, a folha A e a folha extra */

    TabelaCodigos tabela;
    huffman_montar_codigos(raiz, &tabela);
    CU_ASSERT_EQUAL(strlen(tabela.codigo['A']), 1);
    huffman_destruir_arvore(raiz);
}

static void teste_construir_sem_bytes(void)
{
    unsigned long long freq[256] = {0};
    CU_ASSERT_PTR_NULL(huffman_construir_arvore(freq));
}

int registrar_testes_huffman_arvore(void)
{
    CU_pSuite suite = CU_add_suite("huffman: arvore", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "folha e no interno", teste_folha_e_no_interno) == NULL
        || CU_add_test(suite, "grava a arvore do slide", teste_gravar_arvore_do_slide) == NULL
        || CU_add_test(suite, "grava e le de volta", teste_ida_e_volta) == NULL
        || CU_add_test(suite, "escape de * e \\", teste_escape) == NULL
        || CU_add_test(suite, "arvore incompleta", teste_arvore_incompleta) == NULL
        || CU_add_test(suite, "monta a arvore do slide", teste_construir_arvore_do_slide) == NULL
        || CU_add_test(suite, "monta com um byte so", teste_construir_um_byte_so) == NULL
        || CU_add_test(suite, "monta sem bytes", teste_construir_sem_bytes) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
