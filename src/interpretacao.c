#include <stdlib.h>
#include "interpretacao.h"
#include "vetor.h"

/* Os valores ficam em um Vetor de Valor* (posição v-1 guarda a variável v)
 * porque todas as estruturas do projeto armazenam void*. */
struct Interpretacao {
    int num_variaveis;
    Vetor *valores;
};

static Valor *valor_criar(void) {
    Valor *valor = malloc(sizeof *valor);
    if (valor != NULL) *valor = VALOR_NAO_ATRIBUIDO;
    return valor;
}

Interpretacao *interpretacao_criar(int num_variaveis) {
    Interpretacao *interpretacao = malloc(sizeof *interpretacao);
    if (interpretacao == NULL) return NULL;

    interpretacao->num_variaveis = num_variaveis;
    interpretacao->valores = vetor_criar();
    if (interpretacao->valores == NULL) {
        free(interpretacao);
        return NULL;
    }

    for (int i = 0; i < num_variaveis; i++) {
        Valor *valor = valor_criar();
        if (valor == NULL || !vetor_adicionar(interpretacao->valores, valor)) {
            free(valor);   /* free(NULL) é permitido */
            interpretacao_destruir(interpretacao);
            return NULL;
        }
    }
    return interpretacao;
}

int interpretacao_num_variaveis(const Interpretacao *interpretacao) {
    return interpretacao->num_variaveis;
}

Valor interpretacao_obter(const Interpretacao *interpretacao, int variavel) {
    return *(Valor *) vetor_obter(interpretacao->valores, variavel - 1);
}

void interpretacao_atribuir(Interpretacao *interpretacao, int variavel, Valor valor) {
    *(Valor *) vetor_obter(interpretacao->valores, variavel - 1) = valor;
}

Valor interpretacao_valor_literal(const Interpretacao *interpretacao, int literal) {
    int variavel = literal > 0 ? literal : -literal;
    Valor valor = interpretacao_obter(interpretacao, variavel);
    /* Trocar o sinal inverte VERDADEIRO/FALSO e mantém NAO_ATRIBUIDO (0) */
    return literal > 0 ? valor : (Valor) -valor;
}

void interpretacao_destruir(Interpretacao *interpretacao) {
    if (interpretacao == NULL) return;
    vetor_destruir(interpretacao->valores, free);
    free(interpretacao);
}
