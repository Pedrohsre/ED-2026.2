#include <string.h>
#include <CUnit/CUnit.h>
#include "testes.h"
#include "huffman.h"

/* O .huff do slide: cabeçalho 101 00000 00001011, árvore e 51 bits de dados */
static const unsigned char HUFF_DO_SLIDE[] = {
    0xA0, 0x0B,
    '*', '*', 'C', 'B', '*', '*', '*', 'F', 'E', 'D', 'A',
    0xFF, 0xF5, 0x54, 0x02, 0xDB, 0x33, 0x00
};
static const char TEXTO_DO_SLIDE[] = "AAAAAABBBBBCCCCDDDEEF";

/* Cria um arquivo temporário com os bytes dados, posicionado no início */
static FILE *arquivo_com(const void *dados, size_t tamanho)
{
    FILE *arquivo = tmpfile();
    if (arquivo != NULL) {
        if (tamanho > 0)
            fwrite(dados, 1, tamanho, arquivo);
        rewind(arquivo);
    }
    return arquivo;
}

static size_t ler_tudo(FILE *arquivo, unsigned char *buffer, size_t max)
{
    rewind(arquivo);
    return fread(buffer, 1, max, arquivo);
}

static void teste_frequencias(void)
{
    unsigned char dados[] = {'A', 'A', 'B', 0x00, 0xFF};
    FILE *arquivo = arquivo_com(dados, sizeof dados);
    unsigned long long freq[256];

    huffman_contar_frequencias(arquivo, freq);
    CU_ASSERT_EQUAL(freq['A'], 2);
    CU_ASSERT_EQUAL(freq['B'], 1);
    CU_ASSERT_EQUAL(freq[0x00], 1);
    CU_ASSERT_EQUAL(freq[0xFF], 1);
    CU_ASSERT_EQUAL(freq['C'], 0);
    CU_ASSERT_EQUAL(ftell(arquivo), 0);   /* voltou ao início */
    fclose(arquivo);
}

static void teste_frequencias_arquivo_vazio(void)
{
    FILE *arquivo = tmpfile();
    unsigned long long freq[256];

    huffman_contar_frequencias(arquivo, freq);
    for (int b = 0; b < 256; b++)
        CU_ASSERT_EQUAL(freq[b], 0);
    fclose(arquivo);
}

static void teste_cabecalho(void)
{
    FILE *arquivo = tmpfile();
    int lixo, tamanho;

    huffman_gravar_cabecalho(arquivo, 5, 11);   /* exemplo do slide */
    huffman_gravar_cabecalho(arquivo, 7, 8191); /* maiores valores possíveis */
    rewind(arquivo);
    CU_ASSERT_EQUAL(fgetc(arquivo), 0xA0);
    CU_ASSERT_EQUAL(fgetc(arquivo), 0x0B);
    CU_ASSERT_EQUAL(fgetc(arquivo), 0xFF);
    CU_ASSERT_EQUAL(fgetc(arquivo), 0xFF);

    rewind(arquivo);
    CU_ASSERT_TRUE(huffman_ler_cabecalho(arquivo, &lixo, &tamanho));
    CU_ASSERT_EQUAL(lixo, 5);
    CU_ASSERT_EQUAL(tamanho, 11);
    CU_ASSERT_TRUE(huffman_ler_cabecalho(arquivo, &lixo, &tamanho));
    CU_ASSERT_EQUAL(lixo, 7);
    CU_ASSERT_EQUAL(tamanho, 8191);
    CU_ASSERT_FALSE(huffman_ler_cabecalho(arquivo, &lixo, &tamanho));   /* acabou o arquivo */
    fclose(arquivo);
}

static void teste_descompactar_exemplo_do_slide(void)
{
    FILE *huff = arquivo_com(HUFF_DO_SLIDE, sizeof HUFF_DO_SLIDE);
    FILE *saida = tmpfile();
    unsigned char buffer[64];

    CU_ASSERT_TRUE(huffman_descompactar(huff, saida));
    size_t lidos = ler_tudo(saida, buffer, sizeof buffer);
    CU_ASSERT_EQUAL(lidos, strlen(TEXTO_DO_SLIDE));
    CU_ASSERT(memcmp(buffer, TEXTO_DO_SLIDE, strlen(TEXTO_DO_SLIDE)) == 0);
    fclose(huff);
    fclose(saida);
}

static void teste_descompactar_arquivo_vazio(void)
{
    unsigned char so_cabecalho[] = {0x00, 0x00};
    FILE *huff = arquivo_com(so_cabecalho, sizeof so_cabecalho);
    FILE *saida = tmpfile();
    unsigned char buffer[4];

    CU_ASSERT_TRUE(huffman_descompactar(huff, saida));
    CU_ASSERT_EQUAL(ler_tudo(saida, buffer, sizeof buffer), 0);
    fclose(huff);
    fclose(saida);
}

/* Descompacta bytes inválidos e confere que a função recusa */
static int descompacta(const unsigned char *dados, size_t tamanho)
{
    FILE *huff = arquivo_com(dados, tamanho);
    FILE *saida = tmpfile();
    int ok = huffman_descompactar(huff, saida);
    fclose(huff);
    fclose(saida);
    return ok;
}

static void teste_descompactar_invalidos(void)
{
    unsigned char um_byte[] = {0xA0};
    unsigned char arvore_maior_que_arquivo[] = {0xA0, 0x0B, '*', '*', 'C'};
    unsigned char so_uma_folha[] = {0x00, 0x01, 'A', 0x00};
    unsigned char lixo_errado[sizeof HUFF_DO_SLIDE];

    /* com lixo 4 no lugar de 5, sobra um bit e a leitura termina no meio de um código */
    memcpy(lixo_errado, HUFF_DO_SLIDE, sizeof HUFF_DO_SLIDE);
    lixo_errado[0] = 0x80;

    CU_ASSERT_FALSE(descompacta(um_byte, sizeof um_byte));
    CU_ASSERT_FALSE(descompacta(arvore_maior_que_arquivo, sizeof arvore_maior_que_arquivo));
    CU_ASSERT_FALSE(descompacta(so_uma_folha, sizeof so_uma_folha));
    CU_ASSERT_FALSE(descompacta(lixo_errado, sizeof lixo_errado));
}

/*
 * Compacta os dados, descompacta o resultado e confere que voltou igual.
 * Devolve o tamanho do .huff (ou -1 se algo falhou).
 */
static long ida_e_volta(const unsigned char *dados, size_t tamanho)
{
    static unsigned char volta[300000];
    FILE *original = arquivo_com(dados, tamanho);
    FILE *huff = tmpfile();
    FILE *saida = tmpfile();
    long tamanho_huff = -1;

    if (huffman_compactar(original, huff)) {
        tamanho_huff = ftell(huff);
        rewind(huff);
        if (!huffman_descompactar(huff, saida)
            || ler_tudo(saida, volta, sizeof volta) != tamanho
            || (tamanho > 0 && memcmp(volta, dados, tamanho) != 0))
            tamanho_huff = -1;
    }
    fclose(original);
    fclose(huff);
    fclose(saida);
    return tamanho_huff;
}

static void teste_compactar_exemplo_do_slide(void)
{
    FILE *original = arquivo_com(TEXTO_DO_SLIDE, strlen(TEXTO_DO_SLIDE));
    FILE *huff = tmpfile();
    int lixo, tamanho_arvore;

    unsigned char buffer[64];

    CU_ASSERT_TRUE(huffman_compactar(original, huff));
    CU_ASSERT_EQUAL(ftell(huff), 2 + 11 + 7);   /* cabeçalho + árvore + 51 bits em 7 bytes */
    rewind(huff);
    CU_ASSERT_TRUE(huffman_ler_cabecalho(huff, &lixo, &tamanho_arvore));
    CU_ASSERT_EQUAL(lixo, 5);
    CU_ASSERT_EQUAL(tamanho_arvore, 11);
    /* o .huff gerado é, byte a byte, o do slide */
    CU_ASSERT_EQUAL(ler_tudo(huff, buffer, sizeof buffer), sizeof HUFF_DO_SLIDE);
    CU_ASSERT(memcmp(buffer, HUFF_DO_SLIDE, sizeof HUFF_DO_SLIDE) == 0);
    fclose(original);
    fclose(huff);

    CU_ASSERT_EQUAL(ida_e_volta((const unsigned char *) TEXTO_DO_SLIDE, strlen(TEXTO_DO_SLIDE)), 20);
}

static void teste_ida_e_volta_casos_de_borda(void)
{
    unsigned char um_byte_so[1000];
    unsigned char todos_os_bytes[256];
    unsigned char sem_lixo[] = "ABABABAB";   /* 8 bytes de 1 bit cada: lixo 0 */
    unsigned char especiais[] = "**\\\\*\\ texto com * e \\";

    memset(um_byte_so, 'A', sizeof um_byte_so);
    for (int b = 0; b < 256; b++)
        todos_os_bytes[b] = (unsigned char) b;

    CU_ASSERT_EQUAL(ida_e_volta(NULL, 0), 2);   /* arquivo vazio: só o cabeçalho */
    CU_ASSERT_NOT_EQUAL(ida_e_volta(um_byte_so, sizeof um_byte_so), -1);
    CU_ASSERT_NOT_EQUAL(ida_e_volta(todos_os_bytes, sizeof todos_os_bytes), -1);
    CU_ASSERT_EQUAL(ida_e_volta(sem_lixo, 8), 2 + 3 + 1);
    CU_ASSERT_NOT_EQUAL(ida_e_volta(especiais, sizeof especiais - 1), -1);
}

/* Bytes pseudoaleatórios com distribuição desigual, como num arquivo binário */
static void teste_ida_e_volta_arquivo_grande(void)
{
    static unsigned char dados[250000];
    unsigned int semente = 12345;

    for (size_t i = 0; i < sizeof dados; i++) {
        semente = semente * 1103515245u + 12345u;
        dados[i] = (unsigned char) ((semente >> 16) % ((i % 7 == 0) ? 256 : 40));
    }
    long tamanho_huff = ida_e_volta(dados, sizeof dados);
    CU_ASSERT_NOT_EQUAL(tamanho_huff, -1);
    CU_ASSERT(tamanho_huff < (long) sizeof dados);   /* com bytes desiguais, o arquivo diminui */
}

int registrar_testes_huffman(void)
{
    CU_pSuite suite = CU_add_suite("huffman", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "frequencias", teste_frequencias) == NULL
        || CU_add_test(suite, "frequencias de arquivo vazio", teste_frequencias_arquivo_vazio) == NULL
        || CU_add_test(suite, "cabecalho", teste_cabecalho) == NULL
        || CU_add_test(suite, "descompactar o exemplo do slide", teste_descompactar_exemplo_do_slide) == NULL
        || CU_add_test(suite, "descompactar arquivo vazio", teste_descompactar_arquivo_vazio) == NULL
        || CU_add_test(suite, "recusar .huff invalido", teste_descompactar_invalidos) == NULL
        || CU_add_test(suite, "compactar o exemplo do slide", teste_compactar_exemplo_do_slide) == NULL
        || CU_add_test(suite, "ida e volta: casos de borda", teste_ida_e_volta_casos_de_borda) == NULL
        || CU_add_test(suite, "ida e volta: arquivo grande", teste_ida_e_volta_arquivo_grande) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
