#ifndef TESTES_H
#define TESTES_H

/*
 * Cada arquivo tests/test_<modulo>.c expõe uma função que registra as suas
 * suítes no CUnit e devolve CUE_SUCCESS (ou o código de erro do CUnit).
 * A função é declarada aqui e listada em tests/main_testes.c.
 */
int registrar_testes_exemplo(void);
/** Registra os testes da contagem; retorna CUE_SUCCESS ou um erro do CUnit. */
int registrar_testes_contagem(void);

#endif
