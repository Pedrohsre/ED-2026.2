#include <CUnit/CUnit.h>
#include "testes.h"
#include "dimacs.h"
#include "sat.h"

/* Lê o .cnf e resolve. Devolve 1 (SAT), 0 (UNSAT) ou -1 (arquivo inválido). */
static int resolver_arquivo(const char *caminho)
{
    char erro[256];
    Formula *formula = dimacs_ler(caminho, erro, sizeof erro);
    if (formula == NULL)
        return -1;

    Interpretacao *interpretacao = interpretacao_criar(formula_num_variaveis(formula));
    int resultado = sat_resolver(formula, interpretacao);
    interpretacao_destruir(interpretacao);
    formula_destruir(formula);
    return resultado;
}

/* Slide: { ¬x1 ∨ ¬x2, x1 ∨ ¬x2, ¬x1 ∨ ¬x3 } dá SAT com x1 = V, x2 = F, x3 = F */
static void teste_exemplo_do_slide(void)
{
    char erro[256];
    Formula *formula = dimacs_ler("data/sat_slide.cnf", erro, sizeof erro);
    CU_ASSERT_PTR_NOT_NULL_FATAL(formula);
    Interpretacao *interpretacao = interpretacao_criar(formula_num_variaveis(formula));

    CU_ASSERT_TRUE(sat_resolver(formula, interpretacao));
    CU_ASSERT_EQUAL(interpretacao_obter(interpretacao, 1), VALOR_VERDADEIRO);
    CU_ASSERT_EQUAL(interpretacao_obter(interpretacao, 2), VALOR_FALSO);
    CU_ASSERT_EQUAL(interpretacao_obter(interpretacao, 3), VALOR_FALSO);
    CU_ASSERT_EQUAL(formula_avaliar(formula, interpretacao), AVALIACAO_SATISFEITA);

    interpretacao_destruir(interpretacao);
    formula_destruir(formula);
}

static void teste_satisfativeis_e_insatisfativeis(void)
{
    CU_ASSERT_EQUAL(resolver_arquivo("data/sat_multilinha.cnf"), 1);
    CU_ASSERT_EQUAL(resolver_arquivo("data/sat_satlib.cnf"), 1);
    CU_ASSERT_EQUAL(resolver_arquivo("data/unsat_simples.cnf"), 0);
    CU_ASSERT_EQUAL(resolver_arquivo("data/unsat_duas_vars.cnf"), 0);
}

static void teste_arquivos_invalidos(void)
{
    CU_ASSERT_EQUAL(resolver_arquivo("data/invalidos/clausulas_diferentes.cnf"), -1);
    CU_ASSERT_EQUAL(resolver_arquivo("data/invalidos/literal_fora.cnf"), -1);
    CU_ASSERT_EQUAL(resolver_arquivo("data/invalidos/sem_header.cnf"), -1);
    CU_ASSERT_EQUAL(resolver_arquivo("data/invalidos/sem_zero_final.cnf"), -1);
    CU_ASSERT_EQUAL(resolver_arquivo("data/invalidos/token_invalido.cnf"), -1);
    CU_ASSERT_EQUAL(resolver_arquivo("data/nao_existe.cnf"), -1);
}

/* Monta uma cláusula a partir de literais terminados em 0, como no DIMACS */
static Clausula *clausula(const int *literais)
{
    Clausula *c = clausula_criar();
    for (int i = 0; literais[i] != 0; i++)
        clausula_adicionar_literal(c, literais[i]);
    return c;
}

/* Os três casos do slide "Partial Interpretation", com x = 1, y = 2, z = 3 e I = {x = V, z = F} */
static void teste_interpretacao_parcial(void)
{
    int f1a[] = {-1, 2, 0}, f1b[] = {-1, 3, 0};          /* { ¬x ∨ y, ¬x ∨ z } */
    int f2a[] = {1, 2, 0}, f2b[] = {-1, -2, 3, 0};       /* { x ∨ y, ¬x ∨ ¬y ∨ z } */
    int f3a[] = {1, 2, 3, 0}, f3b[] = {-2, -3, 0};       /* { x ∨ y ∨ z, ¬y ∨ ¬z } */
    Formula *f1 = formula_criar(3), *f2 = formula_criar(3), *f3 = formula_criar(3);
    formula_adicionar_clausula(f1, clausula(f1a));
    formula_adicionar_clausula(f1, clausula(f1b));
    formula_adicionar_clausula(f2, clausula(f2a));
    formula_adicionar_clausula(f2, clausula(f2b));
    formula_adicionar_clausula(f3, clausula(f3a));
    formula_adicionar_clausula(f3, clausula(f3b));

    Interpretacao *interpretacao = interpretacao_criar(3);
    interpretacao_atribuir(interpretacao, 1, VALOR_VERDADEIRO);
    interpretacao_atribuir(interpretacao, 3, VALOR_FALSO);

    CU_ASSERT_EQUAL(formula_avaliar(f1, interpretacao), AVALIACAO_FALSIFICADA);
    CU_ASSERT_EQUAL(formula_avaliar(f2, interpretacao), AVALIACAO_INDEFINIDA);
    CU_ASSERT_EQUAL(formula_avaliar(f3, interpretacao), AVALIACAO_SATISFEITA);

    interpretacao_destruir(interpretacao);
    formula_destruir(f1);
    formula_destruir(f2);
    formula_destruir(f3);
}

int registrar_testes_sat(void)
{
    CU_pSuite suite = CU_add_suite("sat", NULL, NULL);
    if (suite == NULL
        || CU_add_test(suite, "exemplo do slide", teste_exemplo_do_slide) == NULL
        || CU_add_test(suite, "SAT e UNSAT", teste_satisfativeis_e_insatisfativeis) == NULL
        || CU_add_test(suite, "arquivos invalidos", teste_arquivos_invalidos) == NULL
        || CU_add_test(suite, "interpretacao parcial", teste_interpretacao_parcial) == NULL)
        return CU_get_error();
    return CUE_SUCCESS;
}
