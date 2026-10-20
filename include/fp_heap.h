#ifndef FP_HEAP_H
#define FP_HEAP_H

/*
 * Fila de prioridade com heap (#8): contrato usado pelo Huffman, pelo
 * Dijkstra e pelo Kruskal. A implementação fica em src/fp_heap.c.
 *
 * A fila guarda void* e não copia os dados. Sai primeiro o elemento que o
 * comparador considera MENOR.
 *
 * A fila sem heap (#7) tem as mesmas operações com o prefixo fp_lista_
 * (FilaLista, fp_lista_criar, ...), para a contagem usar as duas no mesmo
 * programa.
 */

/**
 * Compara dois elementos da fila.
 *
 * @return  Negativo se a sai antes de b; 0 se empatam; positivo se b sai antes de a.
 */
typedef int (*FpComparador)(const void *a, const void *b);

typedef struct FilaHeap FilaHeap;

/**
 * Cria uma fila vazia.
 *
 * @param comparar  Função que decide a ordem de saída.
 * @return          A fila, ou NULL se faltar memória. Liberar com fp_heap_destruir.
 */
FilaHeap *fp_heap_criar(FpComparador comparar);

/**
 * Insere um elemento. A fila guarda só o ponteiro.
 *
 * @param fila  Fila criada por fp_heap_criar.
 * @param dado  Ponteiro para o dado; continua pertencendo a quem inseriu.
 * @return      1 se inseriu; 0 se faltou memória (a fila continua válida).
 */
int fp_heap_inserir(FilaHeap *fila, void *dado);

/**
 * Remove e devolve o menor elemento segundo o comparador.
 *
 * @param fila  Fila criada por fp_heap_criar.
 * @return      O elemento (quem chamou passa a ser o dono), ou NULL se a fila está vazia.
 */
void *fp_heap_remover(FilaHeap *fila);

/**
 * @param fila  Fila criada por fp_heap_criar.
 * @return      Quantidade de elementos na fila.
 */
int fp_heap_tamanho(const FilaHeap *fila);

/**
 * Libera a fila.
 *
 * @param fila     Fila criada por fp_heap_criar. Pode ser NULL.
 * @param liberar  Se não for NULL, é chamada para cada elemento que ainda
 *                 estiver na fila.
 */
void fp_heap_destruir(FilaHeap *fila, void (*liberar)(void *dado));

#endif
