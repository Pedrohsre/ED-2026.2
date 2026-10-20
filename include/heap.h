#ifndef HEAP_H
#define HEAP_H

/*
 * Heap binário genérico (Aula 19), guardado em um vetor de void* que cresce
 * com realloc. A raiz fica no índice 1, como no slide:
 *   pai(i) = i / 2      esquerdo(i) = 2i      direito(i) = 2i + 1
 *
 * No topo fica o elemento que o comparador considera MENOR (min-heap).
 * Para um max-heap, como o do slide, basta inverter o comparador.
 */

/**
 * Compara dois elementos do heap.
 *
 * @return  Negativo se a fica acima de b; 0 se empatam; positivo se b fica acima de a.
 */
typedef int (*HeapComparador)(const void *a, const void *b);

typedef struct Heap Heap;

/**
 * Cria um heap vazio.
 *
 * @param comparar  Função que decide quem fica mais perto do topo.
 * @return          O heap, ou NULL se faltar memória. Liberar com heap_destruir.
 */
Heap *heap_criar(HeapComparador comparar);

/**
 * Insere um elemento: ele entra na primeira posição livre e sobe enquanto
 * vier antes do pai. O heap guarda só o ponteiro, não copia o dado.
 *
 * @param heap  Heap criado por heap_criar.
 * @param dado  Ponteiro para o dado; continua pertencendo a quem inseriu.
 * @return      1 se inseriu; 0 se faltou memória (o heap continua válido).
 */
int heap_inserir(Heap *heap, void *dado);

/**
 * Remove o topo: o último elemento vai para a raiz e desce até o lugar certo.
 *
 * @param heap  Heap criado por heap_criar.
 * @return      O elemento do topo (quem chamou passa a ser o dono), ou NULL se o heap está vazio.
 */
void *heap_remover(Heap *heap);

/**
 * @param heap  Heap criado por heap_criar.
 * @return      O elemento do topo, sem remover, ou NULL se o heap está vazio.
 */
void *heap_topo(const Heap *heap);

/**
 * @param heap  Heap criado por heap_criar.
 * @return      Quantidade de elementos no heap.
 */
int heap_tamanho(const Heap *heap);

/**
 * @param heap  Heap criado por heap_criar.
 * @return      1 se o heap está vazio; 0 caso contrário.
 */
int heap_vazio(const Heap *heap);

/**
 * Libera o heap.
 *
 * @param heap     Heap criado por heap_criar. Pode ser NULL.
 * @param liberar  Se não for NULL, é chamada para cada elemento que ainda
 *                 estiver no heap.
 */
void heap_destruir(Heap *heap, void (*liberar)(void *dado));

#endif
