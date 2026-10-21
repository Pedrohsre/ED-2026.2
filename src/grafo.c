#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"

struct Grafo {
    int num_vertices;
    Lista *arestas;        /* Aresta*; esta lista é a dona das arestas */
    Lista **adjacencia;    /* adjacencia[v]: Aresta* incidentes a v; não são donas */
};

Grafo *grafo_criar(int num_vertices)
{
    if (num_vertices < 1)
        return NULL;

    Grafo *grafo = malloc(sizeof(Grafo));
    if (grafo == NULL)
        return NULL;

    grafo->num_vertices = num_vertices;
    grafo->arestas = lista_criar();
    /* calloc deixa todas as posições NULL: se uma criação falhar no meio,
     * grafo_destruir consegue limpar só o que já foi criado */
    grafo->adjacencia = calloc(num_vertices, sizeof(Lista *));
    if (grafo->arestas == NULL || grafo->adjacencia == NULL) {
        grafo_destruir(grafo);
        return NULL;
    }

    for (int v = 0; v < num_vertices; v++) {
        grafo->adjacencia[v] = lista_criar();
        if (grafo->adjacencia[v] == NULL) {
            grafo_destruir(grafo);
            return NULL;
        }
    }
    return grafo;
}

int grafo_adicionar_aresta(Grafo *grafo, int u, int v, int peso)
{
    if (u < 0 || u >= grafo->num_vertices || v < 0 || v >= grafo->num_vertices)
        return 0;

    Aresta *aresta = malloc(sizeof(Aresta));
    if (aresta == NULL)
        return 0;
    aresta->u = u;
    aresta->v = v;
    aresta->peso = peso;

    if (!lista_inserir_fim(grafo->arestas, aresta)) {
        free(aresta);
        return 0;
    }
    /* A partir daqui a aresta pertence à lista de arestas, que a libera no destruir */
    if (!lista_inserir_fim(grafo->adjacencia[u], aresta))
        return 0;
    if (u != v && !lista_inserir_fim(grafo->adjacencia[v], aresta))
        return 0;
    return 1;
}

static void registrar_erro(char *erro, size_t tam_erro, const char *formato, ...)
{
    va_list args;
    va_start(args, formato);
    vsnprintf(erro, tam_erro, formato, args);
    va_end(args);
}

static int ler_arestas(Grafo *grafo, FILE *arquivo, int num_arestas, char *erro, size_t tam_erro)
{
    for (int i = 1; i <= num_arestas; i++) {
        int u, v, peso;
        if (fscanf(arquivo, " %d %d %d", &u, &v, &peso) != 3) {
            registrar_erro(erro, tam_erro,
                           "aresta %d: esperado 'u v peso' (o arquivo declara %d arestas)",
                           i, num_arestas);
            return 0;
        }
        if (u < 0 || u >= grafo->num_vertices || v < 0 || v >= grafo->num_vertices) {
            registrar_erro(erro, tam_erro, "aresta %d: vértice fora do intervalo 0..%d",
                           i, grafo->num_vertices - 1);
            return 0;
        }
        if (!grafo_adicionar_aresta(grafo, u, v, peso)) {
            registrar_erro(erro, tam_erro, "memória insuficiente");
            return 0;
        }
    }
    return 1;
}

/* Depois das arestas declaradas só pode sobrar espaço em branco. */
static int conferir_fim(FILE *arquivo, int num_arestas, char *erro, size_t tam_erro)
{
    int sobra;
    if (fscanf(arquivo, " %d", &sobra) != EOF) {
        registrar_erro(erro, tam_erro, "conteúdo após as %d arestas declaradas", num_arestas);
        return 0;
    }
    return 1;
}

static Grafo *ler_grafo(FILE *arquivo, char *erro, size_t tam_erro)
{
    int num_vertices, num_arestas;
    if (fscanf(arquivo, "%d %d", &num_vertices, &num_arestas) != 2
        || num_vertices < 1 || num_arestas < 0) {
        registrar_erro(erro, tam_erro,
                       "cabeçalho inválido: esperado '<vértices> <arestas>' com vértices >= 1");
        return NULL;
    }

    Grafo *grafo = grafo_criar(num_vertices);
    if (grafo == NULL) {
        registrar_erro(erro, tam_erro, "memória insuficiente");
        return NULL;
    }

    if (!ler_arestas(grafo, arquivo, num_arestas, erro, tam_erro)
        || !conferir_fim(arquivo, num_arestas, erro, tam_erro)) {
        grafo_destruir(grafo);
        return NULL;
    }
    return grafo;
}

Grafo *grafo_ler(const char *caminho, char *erro, size_t tam_erro)
{
    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        registrar_erro(erro, tam_erro, "não foi possível abrir '%s'", caminho);
        return NULL;
    }

    Grafo *grafo = ler_grafo(arquivo, erro, tam_erro);
    fclose(arquivo);
    return grafo;
}

int grafo_num_vertices(const Grafo *grafo)
{
    return grafo->num_vertices;
}

int grafo_num_arestas(const Grafo *grafo)
{
    return lista_tamanho(grafo->arestas);
}

const Lista *grafo_arestas(const Grafo *grafo)
{
    return grafo->arestas;
}

const Lista *grafo_adjacentes(const Grafo *grafo, int vertice)
{
    return grafo->adjacencia[vertice];
}

int aresta_outro_extremo(const Aresta *aresta, int vertice)
{
    return aresta->u == vertice ? aresta->v : aresta->u;
}

void grafo_destruir(Grafo *grafo)
{
    if (grafo == NULL)
        return;

    if (grafo->adjacencia != NULL) {
        /* As listas de adjacência não são donas das arestas, então não liberam os dados */
        for (int v = 0; v < grafo->num_vertices; v++)
            lista_destruir(grafo->adjacencia[v], NULL);
    }
    free(grafo->adjacencia);
    lista_destruir(grafo->arestas, free);
    free(grafo);
}
