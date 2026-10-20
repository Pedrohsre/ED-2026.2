#include "contagem.h"
#include "filas.h"
#include <stdint.h>
#include <stdlib.h>

int contagem_comparar(const void *a, const void *b)
{
    const ElementoContagem *primeiro = a;
    const ElementoContagem *segundo = b;
    (*primeiro->contador)++;
    return (primeiro->valor > segundo->valor) - (primeiro->valor < segundo->valor);
}

static int inserir_dados(FILE *saida, size_t quantidade, FilaLista *lista,
                         FilaHeap *heap, ElementoContagem *dados)
{
    size_t sem_heap = 0, com_heap = 0;
    for (size_t n = 0; n < quantidade; n++) {
        int valor = rand();
        dados[n] = (ElementoContagem){valor, &sem_heap};
        dados[quantidade + n] = (ElementoContagem){valor, &com_heap};
        sem_heap = com_heap = 0;
        if (!fila_lista_inserir(lista, &dados[n]) ||
            !fila_heap_inserir(heap, &dados[quantidade + n]))
            return 0;
        if (fprintf(saida, "%zu,%zu,%zu\n", n, sem_heap, com_heap) < 0)
            return 0;
    }
    return 1;
}

int contagem_gerar(FILE *saida, size_t quantidade, unsigned int semente)
{
    FilaLista *lista;
    FilaHeap *heap;
    ElementoContagem *dados;
    int sucesso = 0;
    if (quantidade > SIZE_MAX / sizeof *dados / 2)
        return 0;
    lista = fila_lista_criar(contagem_comparar);
    heap = fila_heap_criar(contagem_comparar);
    dados = quantidade ? malloc(2 * quantidade * sizeof *dados) : NULL;
    if (!lista || !heap || (quantidade && !dados))
        goto liberar;
    srand(semente);
    if (fprintf(saida, "n,comparacoes_sem_heap,comparacoes_com_heap\n") < 0)
        goto liberar;
    sucesso = inserir_dados(saida, quantidade, lista, heap, dados);
liberar:
    /* As filas guardam ponteiros para dados, mas nunca liberam os elementos. */
    fila_lista_destruir(lista);
    fila_heap_destruir(heap);
    free(dados);
    return sucesso && fflush(saida) == 0 && !ferror(saida);
}
