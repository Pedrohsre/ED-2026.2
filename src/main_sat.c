#include <stdio.h>
#include "dimacs.h"
#include "formula.h"
#include "interpretacao.h"
#include "sat.h"

static void imprimir_resultado(const Interpretacao *interpretacao, int satisfativel) {
    if (!satisfativel) {
        printf("UNSAT\n");
        return;
    }

    printf("SAT\n");
    int total = interpretacao_num_variaveis(interpretacao);
    for (int variavel = 1; variavel <= total; variavel++) {
        /* Variável livre sai como 0: a fórmula já está satisfeita, qualquer valor serve */
        int valor = interpretacao_obter(interpretacao, variavel) == VALOR_VERDADEIRO;
        printf("%d = %d\n", variavel, valor);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "uso: %s <arquivo.cnf>\n", argv[0]);
        return 2;
    }

    char erro[256];
    Formula *formula = dimacs_ler(argv[1], erro, sizeof erro);
    if (formula == NULL) {
        fprintf(stderr, "erro: %s\n", erro);
        return 1;
    }

    Interpretacao *interpretacao = interpretacao_criar(formula_num_variaveis(formula));
    if (interpretacao == NULL) {
        fprintf(stderr, "erro: memória insuficiente\n");
        formula_destruir(formula);
        return 1;
    }

    imprimir_resultado(interpretacao, sat_resolver(formula, interpretacao));

    interpretacao_destruir(interpretacao);
    formula_destruir(formula);
    return 0;
}
