#include <stdio.h>
#include <string.h>
#include "huffman.h"

#define EXTENSAO ".huff"

static void mostrar_uso(const char *programa)
{
    fprintf(stderr, "Uso:\n"
                    "  %s c <arquivo>               compacta em <arquivo>.huff\n"
                    "  %s d <arquivo.huff> [saida]  descompacta; sem [saida], tira o .huff do nome\n",
            programa, programa);
}

/* Abre os dois arquivos, chama a função e apaga a saída se algo der errado */
static int processar(const char *entrada_nome, const char *saida_nome,
                     int (*funcao)(FILE *, FILE *), const char *erro)
{
    FILE *entrada = fopen(entrada_nome, "rb");
    if (entrada == NULL) {
        perror(entrada_nome);
        return 1;
    }
    FILE *saida = fopen(saida_nome, "wb");
    if (saida == NULL) {
        perror(saida_nome);
        fclose(entrada);
        return 1;
    }

    int ok = funcao(entrada, saida);
    fclose(entrada);
    fclose(saida);
    if (!ok) {
        fprintf(stderr, "Erro: %s\n", erro);
        remove(saida_nome);
        return 1;
    }
    printf("%s -> %s\n", entrada_nome, saida_nome);
    return 0;
}

static int compactar(const char *nome)
{
    char saida[FILENAME_MAX];

    if (snprintf(saida, sizeof saida, "%s%s", nome, EXTENSAO) >= (int) sizeof saida) {
        fprintf(stderr, "Erro: nome de arquivo grande demais.\n");
        return 1;
    }
    return processar(nome, saida, huffman_compactar, "memória insuficiente.");
}

static int existe(const char *nome)
{
    FILE *arquivo = fopen(nome, "rb");
    if (arquivo == NULL)
        return 0;
    fclose(arquivo);
    return 1;
}

static int descompactar(const char *nome, const char *saida_dada)
{
    char saida[FILENAME_MAX];
    size_t n = strlen(nome);
    size_t ext = strlen(EXTENSAO);

    if (saida_dada != NULL) {
        snprintf(saida, sizeof saida, "%s", saida_dada);
    } else {
        if (n <= ext || strcmp(nome + n - ext, EXTENSAO) != 0) {
            fprintf(stderr, "Erro: %s não termina em %s.\n", nome, EXTENSAO);
            return 1;
        }
        snprintf(saida, sizeof saida, "%.*s", (int) (n - ext), nome);
        /* não apaga um arquivo que já existe, como o original ao lado do .huff */
        if (existe(saida)) {
            fprintf(stderr, "Erro: %s já existe. Informe outro nome: d %s <saida>\n", saida, nome);
            return 1;
        }
    }
    return processar(nome, saida, huffman_descompactar, "arquivo .huff inválido.");
}

int main(int argc, char *argv[])
{
    if (argc == 3 && strcmp(argv[1], "c") == 0)
        return compactar(argv[2]);
    if ((argc == 3 || argc == 4) && strcmp(argv[1], "d") == 0)
        return descompactar(argv[2], argc == 4 ? argv[3] : NULL);

    mostrar_uso(argv[0]);
    return 1;
}
