#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dimacs.h"

/* Estado da leitura. Fica em uma struct (e não em variáveis globais) para que
 * as funções auxiliares compartilhem o progresso sem muitos parâmetros. */
typedef struct {
    FILE *arquivo;
    Formula *formula;            /* NULL até a linha 'p cnf' ser lida */
    int num_variaveis;
    int clausulas_declaradas;
    Clausula *clausula_atual;    /* cláusula em construção; NULL se nenhuma */
    char *erro;
    size_t tam_erro;
} Leitor;

static void registrar_erro(Leitor *leitor, const char *formato, ...) {
    va_list args;
    va_start(args, formato);
    vsnprintf(leitor->erro, leitor->tam_erro, formato, args);
    va_end(args);
}

static void pular_linha(FILE *arquivo) {
    int ch;
    while ((ch = fgetc(arquivo)) != EOF && ch != '\n')
        ;
}

/* Lê o que vem depois do 'p': "cnf <variáveis> <cláusulas>". */
static int ler_cabecalho(Leitor *leitor) {
    char formato[16];
    int variaveis, clausulas;

    if (leitor->formula != NULL) {
        registrar_erro(leitor, "linha 'p' repetida");
        return 0;
    }
    if (fscanf(leitor->arquivo, " %15s %d %d", formato, &variaveis, &clausulas) != 3
        || strcmp(formato, "cnf") != 0 || variaveis < 0 || clausulas < 0) {
        registrar_erro(leitor, "linha 'p cnf <variáveis> <cláusulas>' inválida");
        return 0;
    }

    leitor->formula = formula_criar(variaveis);
    if (leitor->formula == NULL) {
        registrar_erro(leitor, "memória insuficiente");
        return 0;
    }
    leitor->num_variaveis = variaveis;
    leitor->clausulas_declaradas = clausulas;
    return 1;
}

static int adicionar_literal(Leitor *leitor, long literal) {
    if (literal < -leitor->num_variaveis || literal > leitor->num_variaveis) {
        registrar_erro(leitor, "literal %ld fora do intervalo 1..%d", literal, leitor->num_variaveis);
        return 0;
    }
    /* A cláusula só é criada no primeiro literal, pois ela pode ocupar várias linhas */
    if (leitor->clausula_atual == NULL) {
        leitor->clausula_atual = clausula_criar();
        if (leitor->clausula_atual == NULL) {
            registrar_erro(leitor, "memória insuficiente");
            return 0;
        }
    }
    if (!clausula_adicionar_literal(leitor->clausula_atual, (int) literal)) {
        registrar_erro(leitor, "memória insuficiente");
        return 0;
    }
    return 1;
}

/* O 0 fecha a cláusula atual e a entrega para a fórmula. */
static int encerrar_clausula(Leitor *leitor) {
    /* Um 0 sozinho é uma cláusula vazia (sempre falsa, logo a fórmula é UNSAT) */
    if (leitor->clausula_atual == NULL) {
        leitor->clausula_atual = clausula_criar();
        if (leitor->clausula_atual == NULL) {
            registrar_erro(leitor, "memória insuficiente");
            return 0;
        }
    }
    if (!formula_adicionar_clausula(leitor->formula, leitor->clausula_atual)) {
        registrar_erro(leitor, "memória insuficiente");
        return 0;
    }
    leitor->clausula_atual = NULL;   /* agora pertence à fórmula */
    return 1;
}

static int processar_numero(Leitor *leitor, const char *token) {
    if (leitor->formula == NULL) {
        registrar_erro(leitor, "cláusula antes da linha 'p cnf'");
        return 0;
    }

    char *fim;
    long literal = strtol(token, &fim, 10);
    if (*fim != '\0') {
        registrar_erro(leitor, "token inválido '%s'", token);
        return 0;
    }
    return literal == 0 ? encerrar_clausula(leitor) : adicionar_literal(leitor, literal);
}

static int ler_tokens(Leitor *leitor) {
    char token[64];

    while (fscanf(leitor->arquivo, "%63s", token) == 1) {
        if (token[0] == 'c') { pular_linha(leitor->arquivo); continue; }
        if (token[0] == '%') break;   /* o "0" que vem depois do % não faz parte da fórmula */

        int ok = strcmp(token, "p") == 0 ? ler_cabecalho(leitor) : processar_numero(leitor, token);
        if (!ok) return 0;
    }
    return 1;
}

/* Confere o que só dá para saber depois de ler o arquivo inteiro. */
static int validar_final(Leitor *leitor) {
    if (leitor->formula == NULL) {
        registrar_erro(leitor, "linha 'p cnf' ausente");
        return 0;
    }
    if (leitor->clausula_atual != NULL) {
        registrar_erro(leitor, "última cláusula não termina com 0");
        return 0;
    }
    int lidas = formula_num_clausulas(leitor->formula);
    if (lidas != leitor->clausulas_declaradas) {
        registrar_erro(leitor, "esperava %d cláusulas, encontrou %d",
                       leitor->clausulas_declaradas, lidas);
        return 0;
    }
    return 1;
}

Formula *dimacs_ler(const char *caminho, char *erro, size_t tam_erro) {
    Leitor leitor = {0};
    leitor.arquivo = fopen(caminho, "r");
    if (leitor.arquivo == NULL) {
        snprintf(erro, tam_erro, "não foi possível abrir '%s'", caminho);
        return NULL;
    }
    leitor.erro = erro;
    leitor.tam_erro = tam_erro;

    int ok = ler_tokens(&leitor) && validar_final(&leitor);

    fclose(leitor.arquivo);
    clausula_destruir(leitor.clausula_atual);   /* só sobra algo aqui se a leitura falhou */
    if (!ok) {
        formula_destruir(leitor.formula);
        return NULL;
    }
    return leitor.formula;
}
