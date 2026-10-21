#include <stdio.h>
#include <stdlib.h>
#include "formula.h"
#include "vetor.h"

struct Formula {
    int num_variaveis;
    Vetor *clausulas;   /* elementos do tipo Clausula* */
};

Formula *formula_criar(int num_variaveis) {
    Formula *formula = malloc(sizeof *formula);
    if (formula == NULL) return NULL;

    formula->num_variaveis = num_variaveis;
    formula->clausulas = vetor_criar();
    if (formula->clausulas == NULL) {
        free(formula);
        return NULL;
    }
    return formula;
}

int formula_adicionar_clausula(Formula *formula, Clausula *clausula) {
    return vetor_adicionar(formula->clausulas, clausula);
}

int formula_num_variaveis(const Formula *formula) {
    return formula->num_variaveis;
}

int formula_num_clausulas(const Formula *formula) {
    return vetor_tamanho(formula->clausulas);
}

ResultadoAvaliacao formula_avaliar(const Formula *formula, const Interpretacao *interpretacao) {
    int todas_satisfeitas = 1;
    int total = vetor_tamanho(formula->clausulas);

    for (int i = 0; i < total; i++) {
        const Clausula *clausula = vetor_obter(formula->clausulas, i);
        ResultadoAvaliacao resultado = clausula_avaliar(clausula, interpretacao);

        /* Uma cláusula falsificada já basta para I => ¬F, sem olhar as demais */
        if (resultado == AVALIACAO_FALSIFICADA) return AVALIACAO_FALSIFICADA;
        if (resultado == AVALIACAO_INDEFINIDA) todas_satisfeitas = 0;
    }
    return todas_satisfeitas ? AVALIACAO_SATISFEITA : AVALIACAO_INDEFINIDA;
}

void formula_imprimir(const Formula *formula) {
    printf("p cnf %d %d\n", formula->num_variaveis, vetor_tamanho(formula->clausulas));
    vetor_imprimir(formula->clausulas, clausula_imprimir, "\n");
    printf("\n");
}

void formula_destruir(Formula *formula) {
    if (formula == NULL) return;
    vetor_destruir(formula->clausulas, clausula_destruir);
    free(formula);
}
