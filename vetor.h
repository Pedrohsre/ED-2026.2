#ifndef VETOR_H
#define VETOR_H

/*
 * Vetor dinâmico genérico: armazena ponteiros void*.
 * É a base das demais estruturas do módulo SAT (fórmula, cláusula, interpretação).
 */

typedef struct Vetor Vetor;

/**
 * Função que libera um dado armazenado em uma estrutura.
 *
 * @param dado  Ponteiro para o dado a ser liberado.
 */
typedef void (*FuncaoLiberacao)(void *dado);

/**
 * Função que imprime um dado armazenado em uma estrutura (sem quebra de linha).
 *
 * @param dado  Ponteiro para o dado a ser impresso.
 */
typedef void (*FuncaoImpressao)(const void *dado);

/**
 * Cria um vetor vazio.
 *
 * @return  Ponteiro para o vetor, ou NULL se faltar memória.
 *          O chamador é responsável por liberá-lo com vetor_destruir.
 */
Vetor *vetor_criar(void);

/**
 * Adiciona um elemento ao final do vetor.
 *
 * @param vetor  Vetor onde o elemento será inserido. Não pode ser NULL.
 * @param dado   Ponteiro para o dado. O vetor NÃO copia o dado,
 *               apenas armazena o ponteiro.
 * @return       1 em caso de sucesso, 0 se faltar memória
 *               (nesse caso o vetor permanece inalterado).
 */
int vetor_adicionar(Vetor *vetor, void *dado);

/**
 * Devolve o elemento de uma posição.
 *
 * @param vetor   Vetor consultado. Não pode ser NULL.
 * @param indice  Posição, entre 0 e vetor_tamanho(vetor) - 1.
 * @return        O ponteiro armazenado. O vetor continua dono da posição,
 *                mas a memória apontada continua sendo de quem inseriu o dado.
 */
void *vetor_obter(const Vetor *vetor, int indice);

/**
 * Devolve a quantidade de elementos do vetor.
 *
 * @param vetor  Vetor consultado. Não pode ser NULL.
 * @return       Número de elementos.
 */
int vetor_tamanho(const Vetor *vetor);

/**
 * Imprime todos os elementos, na ordem, separados por um texto.
 *
 * @param vetor      Vetor a imprimir. Não pode ser NULL.
 * @param imprimir   Função que imprime um único elemento. Não pode ser NULL.
 * @param separador  Texto impresso entre dois elementos consecutivos (ex.: " " ou "\n").
 */
void vetor_imprimir(const Vetor *vetor, FuncaoImpressao imprimir, const char *separador);

/**
 * Libera o vetor e, opcionalmente, os dados que ele armazena.
 *
 * @param vetor   Vetor a destruir. Pode ser NULL (nada acontece).
 * @param liberar Função chamada para cada elemento. Se for NULL, os dados NÃO
 *                são liberados (o chamador continua dono deles).
 */
void vetor_destruir(Vetor *vetor, FuncaoLiberacao liberar);

#endif
