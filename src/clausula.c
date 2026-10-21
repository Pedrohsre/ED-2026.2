#include <stdio.h>
#include <stdlib.h>
#include "clausula.h"
#include "vetor.h"

/* Cada literal é um int* alocado individualmente (a estrutura só guarda void*). */
struct Clausula {
    Vetor *literais;
};

Clausula *clausula_criar(void) {
    Clausula *clausula = malloc(sizeof *clausula);
    if (clausula == NULL) return NULL;

    clausula->literais = vetor_criar();
    if (clausula->literais == NULL) {
        free(clausula);
        return NULL;
    }
    return clausula;
}

int clausula_adicionar_literal(Clausula *clausula, int literal) {
    int *copia = malloc(sizeof *copia);
    if (copia == NULL) return 0;

    *copia = literal;
    if (!vetor_adicionar(clausula->literais, copia)) {
        free(copia);
        return 0;
    }
    return 1;
}

ResultadoAvaliacao clausula_avaliar(const Clausula *clausula, const Interpretacao *interpretacao) {
    int ha_literal_livre = 0;
    int total = vetor_tamanho(clausula->literais);

    for (int i = 0; i < total; i++) {
        int literal = *(int *) vetor_obter(clausula->literais, i);
        Valor valor = interpretacao_valor_literal(interpretacao, literal);

        /* Basta um literal verdadeiro: o resto da cláusula não importa */
        if (valor == VALOR_VERDADEIRO) return AVALIACAO_SATISFEITA;
        if (valor == VALOR_NAO_ATRIBUIDO) ha_literal_livre = 1;
    }
    /* Sem literal verdadeiro: ainda pode virar verdadeira se houver literal livre */
    return ha_literal_livre ? AVALIACAO_INDEFINIDA : AVALIACAO_FALSIFICADA;
}

static void imprimir_literal(const void *literal) {
    printf("%d", *(const int *) literal);
}

void clausula_imprimir(const void *clausula) {
    const Clausula *c = clausula;
    vetor_imprimir(c->literais, imprimir_literal, " ");
    printf(vetor_tamanho(c->literais) > 0 ? " 0" : "0");
}

void clausula_destruir(void *clausula) {
    Clausula *c = clausula;
    if (c == NULL) return;
    vetor_destruir(c->literais, free);
    free(c);
}
