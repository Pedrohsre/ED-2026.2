#include "sat.h"

/* Devolve a primeira variável sem valor, ou 0 se todas já foram atribuídas. */
static int proxima_variavel_livre(const Interpretacao *interpretacao) {
    int total = interpretacao_num_variaveis(interpretacao);
    for (int variavel = 1; variavel <= total; variavel++) {
        if (interpretacao_obter(interpretacao, variavel) == VALOR_NAO_ATRIBUIDO)
            return variavel;
    }
    return 0;
}

int sat_resolver(const Formula *formula, Interpretacao *interpretacao) {
    ResultadoAvaliacao resultado = formula_avaliar(formula, interpretacao);
    if (resultado == AVALIACAO_SATISFEITA) return 1;    /* I => F  */
    if (resultado == AVALIACAO_FALSIFICADA) return 0;   /* I => ¬F */

    /* Fórmula indefinida implica que existe variável livre; o teste é só uma salvaguarda */
    int variavel = proxima_variavel_livre(interpretacao);
    if (variavel == 0) return 0;

    interpretacao_atribuir(interpretacao, variavel, VALOR_VERDADEIRO);
    if (sat_resolver(formula, interpretacao)) return 1;

    interpretacao_atribuir(interpretacao, variavel, VALOR_FALSO);
    if (sat_resolver(formula, interpretacao)) return 1;

    interpretacao_atribuir(interpretacao, variavel, VALOR_NAO_ATRIBUIDO);   /* desfaz antes de voltar */
    return 0;
}
