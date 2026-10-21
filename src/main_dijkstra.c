#include <stdio.h>
#include "dijkstra.h"
#include "grafo.h"

int main(int argc, char **argv)
{
    int origem = 0;
    if ((argc != 2 && argc != 3) || (argc == 3 && sscanf(argv[2], "%d", &origem) != 1)) {
        fprintf(stderr, "uso: %s <arquivo-do-grafo> [origem]\n", argv[0]);
        return 2;
    }

    char erro[256];
    Grafo *grafo = grafo_ler(argv[1], erro, sizeof erro);
    if (grafo == NULL) {
        fprintf(stderr, "erro: %s\n", erro);
        return 1;
    }

    CaminhosMinimos *caminhos = dijkstra_calcular(grafo, origem, erro, sizeof erro);
    if (caminhos == NULL) {
        fprintf(stderr, "erro: %s\n", erro);
        grafo_destruir(grafo);
        return 1;
    }

    dijkstra_imprimir(caminhos);

    dijkstra_destruir(caminhos);
    grafo_destruir(grafo);
    return 0;
}
