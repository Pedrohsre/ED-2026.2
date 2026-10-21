#ifndef TESTES_H
#define TESTES_H

/*
 * Cada arquivo tests/test_<modulo>.c expõe uma função que registra as suas
 * suítes no CUnit e devolve CUE_SUCCESS (ou o código de erro do CUnit).
 * A função é declarada aqui e listada em tests/main_testes.c.
 */
int registrar_testes_exemplo(void);
int registrar_testes_fp_heap(void);
int registrar_testes_bits(void);
int registrar_testes_huffman_arvore(void);
int registrar_testes_huffman(void);
int registrar_testes_union_find(void);

int registrar_testes_sat(void);
int registrar_testes_vetor(void);
int registrar_testes_lista(void);
int registrar_testes_grafo(void);
int registrar_testes_kruskal(void);
int registrar_testes_dijkstra(void);
/** Registra os testes da contagem; retorna CUE_SUCCESS ou um erro do CUnit. */
int registrar_testes_contagem(void);
int registrar_testes_fila_lista(void);
#endif
