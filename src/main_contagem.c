#include "contagem.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

static int ler_numero(const char *texto, unsigned long *numero)
{
    char *fim;
    if (*texto < '0' || *texto > '9')
        return 0;
    errno = 0;
    *numero = strtoul(texto, &fim, 10);
    return !errno && *fim == '\0';
}

int main(int argc, char **argv)
{
    unsigned long semente = (unsigned int)time(NULL), quantidade = 5000;
    if (argc > 3 ||
        (argc > 1 && (!ler_numero(argv[1], &quantidade) || quantidade == 0)) ||
        (argc > 2 && (!ler_numero(argv[2], &semente) || semente > UINT_MAX))) {
        fprintf(stderr, "Uso: %s [quantidade_positiva [semente]]\n", argv[0]);
        return EXIT_FAILURE;
    }
    fprintf(stderr, "Semente: %lu; insercoes: %lu\n", semente, quantidade);
    if (!contagem_gerar(stdout, (size_t)quantidade, (unsigned int)semente)) {
        fprintf(stderr, "Erro de memoria ou de escrita do CSV.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
