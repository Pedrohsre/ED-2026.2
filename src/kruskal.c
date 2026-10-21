#include <stdio.h>
#include "fp_heap.h"
#include "kruskal.h"
#include "union_find.h"

static int comparar_por_peso(const void *a, const void *b)
{
    const Aresta *x = a;
    const Aresta *y = b;
    /* Evita x->peso - y->peso, que pode estourar o int com pesos extremos */
    return (x->peso > y->peso) - (x->peso < y->peso);
}

static int preencher_fila(FilaHeap *fila, const Grafo *grafo)
{
    for (const No *no = grafo_arestas(grafo)->inicio; no != NULL; no = no->proximo) {
        if (!fp_heap_inserir(fila, no->dado))
            return 0;
    }
    return 1;
}

/* Escolhe até 'meta' arestas, em ordem crescente de peso. */
static int selecionar_arestas(FilaHeap *fila, UnionFind *uf, Lista *escolhidas,
                              int meta, int *peso_total)
{
    /* Para quando já há 'meta' arestas: uma árvore completa não aceita mais nenhuma,
     * então as restantes na fila só fechariam ciclos */
    while (fp_heap_tamanho(fila) > 0 && lista_tamanho(escolhidas) < meta) {
        Aresta *aresta = fp_heap_remover(fila);

        /* unir devolve 0 quando os extremos já estão conectados: a aresta fecharia um ciclo */
        if (!union_find_unir(uf, aresta->u, aresta->v))
            continue;
        if (!lista_inserir_fim(escolhidas, aresta))
            return 0;
        *peso_total += aresta->peso;
    }
    return 1;
}

Lista *kruskal_calcular(const Grafo *grafo, int *peso_total)
{
    int num_vertices = grafo_num_vertices(grafo);
    Lista *escolhidas = lista_criar();
    FilaHeap *fila = fp_heap_criar(comparar_por_peso);
    UnionFind *uf = union_find_criar(num_vertices);

    *peso_total = 0;
    int ok = escolhidas != NULL && fila != NULL && uf != NULL
             && preencher_fila(fila, grafo)
             && selecionar_arestas(fila, uf, escolhidas, num_vertices - 1, peso_total);

    fp_heap_destruir(fila, NULL);   /* as arestas pertencem ao grafo: a fila não as libera */
    union_find_destruir(uf);
    if (!ok) {
        lista_destruir(escolhidas, NULL);
        return NULL;
    }
    return escolhidas;
}

void kruskal_imprimir(const Grafo *grafo, const Lista *escolhidas, int peso_total)
{
    printf("Arestas da árvore geradora mínima:\n");
    for (const No *no = escolhidas->inicio; no != NULL; no = no->proximo) {
        const Aresta *aresta = no->dado;
        printf("  %d - %d (peso %d)\n", aresta->u, aresta->v, aresta->peso);
    }
    printf("Peso total: %d\n", peso_total);

    /* Uma floresta sem ciclos com k arestas sobre V vértices tem exatamente V - k componentes */
    int componentes = grafo_num_vertices(grafo) - lista_tamanho(escolhidas);
    if (componentes > 1) {
        printf("Aviso: o grafo é desconexo; o resultado é uma floresta geradora "
               "mínima com %d componentes.\n", componentes);
    }
}
