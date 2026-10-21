#ifndef FILAS_H
#define FILAS_H

typedef struct FilaLista FilaLista;
/** Retorna negativo se a vem antes de b, zero no empate e positivo caso contrario. */
typedef int (*Comparador)(const void *, const void *);

/** Cria lista ordenada crescente; comparar deve ser valido. Retorna NULL sem memoria.
 * O chamador deve liberar a fila com fila_lista_destruir.
 */
FilaLista *fila_lista_criar(Comparador comparar);
/** Guarda dado (nao NULL) na fila valida, sem copia. Retorna 1 ou 0 sem memoria. */
int fila_lista_inserir(FilaLista *fila, void *dado);
/** Libera nos e fila; aceita NULL. Os dados continuam pertencendo ao chamador. */
void fila_lista_destruir(FilaLista *fila);

/** Remove e devolve o primeiro (o menor); quem chamou passa a ser o dono. NULL se vazia. */
void *fila_lista_remover(FilaLista *fila);
/** Devolve quantos elementos há na fila. */
int fila_lista_tamanho(const FilaLista *fila);

#endif
