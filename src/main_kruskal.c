#include <stdio.h>
#include "grafo.h"
#include "kruskal.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "uso: %s <arquivo-do-grafo>\n", argv[0]);
        return 2;
    }

    char erro[256];
    Grafo *grafo = grafo_ler(argv[1], erro, sizeof erro);
    if (grafo == NULL) {
        fprintf(stderr, "erro: %s\n", erro);
        return 1;
    }

    int peso_total;
    Lista *escolhidas = kruskal_calcular(grafo, &peso_total);
    if (escolhidas == NULL) {
        fprintf(stderr, "erro: memória insuficiente\n");
        grafo_destruir(grafo);
        return 1;
    }

    kruskal_imprimir(grafo, escolhidas, peso_total);

    lista_destruir(escolhidas, NULL);   /* as arestas são do grafo */
    grafo_destruir(grafo);
    return 0;
}
