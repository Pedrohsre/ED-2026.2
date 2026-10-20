#include <CUnit/Basic.h>
#include "union_find.h"

static void testar_criar_invalido(void) {
    CU_ASSERT_PTR_NULL(union_find_criar(0));
    CU_ASSERT_PTR_NULL(union_find_criar(-3));
}

static void testar_elementos_comecam_separados(void) {
    UnionFind *uf = union_find_criar(5);
    CU_ASSERT_PTR_NOT_NULL_FATAL(uf);

    for (int i = 0; i < 5; i++)
        CU_ASSERT_EQUAL(union_find_encontrar(uf, i), i);

    union_find_destruir(uf);
}

static void testar_unir_dois_elementos(void) {
    UnionFind *uf = union_find_criar(4);
    CU_ASSERT_PTR_NOT_NULL_FATAL(uf);

    CU_ASSERT_EQUAL(union_find_unir(uf, 0, 1), 1);
    CU_ASSERT_EQUAL(union_find_encontrar(uf, 0), union_find_encontrar(uf, 1));
    CU_ASSERT_NOT_EQUAL(union_find_encontrar(uf, 0), union_find_encontrar(uf, 2));

    union_find_destruir(uf);
}

static void testar_unir_repetido_retorna_zero(void) {
    UnionFind *uf = union_find_criar(4);
    CU_ASSERT_PTR_NOT_NULL_FATAL(uf);

    CU_ASSERT_EQUAL(union_find_unir(uf, 0, 1), 1);
    CU_ASSERT_EQUAL(union_find_unir(uf, 1, 0), 0);   /* ordem invertida: mesmo conjunto */
    CU_ASSERT_EQUAL(union_find_unir(uf, 2, 2), 0);   /* elemento com ele mesmo */

    union_find_destruir(uf);
}

static void testar_transitividade(void) {
    UnionFind *uf = union_find_criar(5);
    CU_ASSERT_PTR_NOT_NULL_FATAL(uf);

    union_find_unir(uf, 0, 1);
    union_find_unir(uf, 1, 2);
    CU_ASSERT_EQUAL(union_find_encontrar(uf, 0), union_find_encontrar(uf, 2));
    CU_ASSERT_NOT_EQUAL(union_find_encontrar(uf, 0), union_find_encontrar(uf, 3));

    union_find_destruir(uf);
}

static void testar_unir_dois_grupos(void) {
    UnionFind *uf = union_find_criar(6);
    CU_ASSERT_PTR_NOT_NULL_FATAL(uf);

    union_find_unir(uf, 0, 1);
    union_find_unir(uf, 2, 3);
    union_find_unir(uf, 4, 5);
    CU_ASSERT_EQUAL(union_find_unir(uf, 1, 2), 1);   /* junta {0,1} com {2,3} */
    CU_ASSERT_EQUAL(union_find_encontrar(uf, 0), union_find_encontrar(uf, 3));
    CU_ASSERT_NOT_EQUAL(union_find_encontrar(uf, 0), union_find_encontrar(uf, 4));

    union_find_destruir(uf);
}

static void testar_cadeia_grande(void) {
    int n = 1000;
    UnionFind *uf = union_find_criar(n);
    CU_ASSERT_PTR_NOT_NULL_FATAL(uf);

    for (int i = 0; i + 1 < n; i++)
        CU_ASSERT_EQUAL(union_find_unir(uf, i, i + 1), 1);

    int raiz = union_find_encontrar(uf, 0);
    for (int i = 0; i < n; i++)
        CU_ASSERT_EQUAL(union_find_encontrar(uf, i), raiz);

    union_find_destruir(uf);
}

static void testar_destruir_null(void) {
    union_find_destruir(NULL);   /* não deve travar */
    CU_PASS("destruir(NULL) não trava");
}

int main(void) {
    if (CU_initialize_registry() != CUE_SUCCESS) return CU_get_error();

    CU_pSuite suite = CU_add_suite("union_find", NULL, NULL);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(suite, "criar com tamanho invalido", testar_criar_invalido);
    CU_add_test(suite, "elementos comecam separados", testar_elementos_comecam_separados);
    CU_add_test(suite, "unir dois elementos", testar_unir_dois_elementos);
    CU_add_test(suite, "unir repetido retorna 0", testar_unir_repetido_retorna_zero);
    CU_add_test(suite, "transitividade", testar_transitividade);
    CU_add_test(suite, "unir dois grupos", testar_unir_dois_grupos);
    CU_add_test(suite, "cadeia grande (1000)", testar_cadeia_grande);
    CU_add_test(suite, "destruir NULL", testar_destruir_null);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    unsigned falhas = CU_get_number_of_failures();
    CU_cleanup_registry();
    return falhas == 0 ? 0 : 1;
}
