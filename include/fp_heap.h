#ifndef FP_HEAP_H
#define FP_HEAP_H

/*
 * Fila de prioridade com heap (Aula 19). Guarda void* sem copiar os dados e
 * tira primeiro o elemento que o comparador considera menor.
 * A prioridade é a menor frequência: o comparador faz a fila tirar primeiro o nó menos frequente. 
 Juntando sempre os dois menores, os bytes raros ficam no fundo da árvore, com códigos longos, e os
 frequentes perto da raiz, com códigos curtos. Isso minimiza o total de bits. No empate, o nó interno
 sai antes, e por isso a nossa árvore é igual à do slide.
 */

/** Negativo se a sai antes de b; 0 se empatam; positivo se b sai antes de a. */
typedef int (*FpComparador)(const void *a, const void *b);

typedef struct FilaHeap FilaHeap;

/** Cria uma fila vazia. Devolve NULL se faltar memória. */
FilaHeap *fp_heap_criar(FpComparador comparar);

/** Insere o ponteiro; o dado continua sendo de quem inseriu. Devolve 1, ou 0 se faltar memória. */
int fp_heap_inserir(FilaHeap *fila, void *dado);

/** Remove e devolve o menor elemento (quem chamou passa a ser o dono), ou NULL se a fila está vazia. */
void *fp_heap_remover(FilaHeap *fila);

/** Devolve quantos elementos há na fila. */
int fp_heap_tamanho(const FilaHeap *fila);

/** Libera a fila. Se liberar não for NULL, ela é chamada para cada elemento que sobrou. */
void fp_heap_destruir(FilaHeap *fila, void (*liberar)(void *dado));

#endif
