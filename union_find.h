#ifndef UNION_FIND_H
#define UNION_FIND_H

/*
 * Union-Find (conjuntos disjuntos) com compressão de caminho e união por rank.
 *
 * Os elementos são os inteiros 0..num_elementos-1 (por exemplo, os vértices de um
 * grafo). Como a estrutura só relaciona índices e não armazena dados do usuário,
 * ela usa vetores de int internamente em vez de void*.
 *
 * Inicialmente cada elemento está sozinho no seu próprio conjunto.
 */

typedef struct UnionFind UnionFind;

/**
 * Cria uma estrutura com cada elemento em um conjunto separado.
 *
 * @param num_elementos  Quantidade de elementos (0..num_elementos-1). Deve ser >= 1.
 * @return               Ponteiro para a estrutura, ou NULL se num_elementos < 1
 *                       ou se faltar memória. O chamador deve liberá-la com
 *                       union_find_destruir.
 */
UnionFind *union_find_criar(int num_elementos);

/**
 * Encontra o representante (raiz) do conjunto de um elemento.
 *
 * Dois elementos estão no mesmo conjunto se, e somente se, têm o mesmo representante.
 * Como efeito colateral, comprime o caminho percorrido para acelerar buscas futuras.
 *
 * @param uf        Estrutura consultada. Não pode ser NULL.
 * @param elemento  Elemento, entre 0 e num_elementos-1.
 * @return          Representante do conjunto do elemento.
 */
int union_find_encontrar(UnionFind *uf, int elemento);

/**
 * Une os conjuntos de dois elementos.
 *
 * @param uf  Estrutura a alterar. Não pode ser NULL.
 * @param a   Primeiro elemento, entre 0 e num_elementos-1.
 * @param b   Segundo elemento, entre 0 e num_elementos-1.
 * @return    1 se os elementos estavam em conjuntos diferentes e foram unidos;
 *            0 se já estavam no mesmo conjunto (nada muda). Nenhum dos dois
 *            casos é erro: o retorno diz ao chamador se a união aconteceu.
 */
int union_find_unir(UnionFind *uf, int a, int b);

/**
 * Libera a estrutura.
 *
 * @param uf  Estrutura a destruir. Pode ser NULL (nada acontece).
 */
void union_find_destruir(UnionFind *uf);

#endif
