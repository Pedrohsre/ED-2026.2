#ifndef LISTA_H
#define LISTA_H

/*
 * Lista encadeada genérica (#5). Guarda void* e NÃO copia os dados: quem insere
 * continua dono da memória apontada (a lista só libera os dados se receber um
 * callback em lista_destruir).
 *
 * A lista guarda o ponteiro para o último nó, então inserir no fim é O(1).
 *
 * Os campos de No e Lista são visíveis para permitir percorrer a lista:
 *
 *     for (const No *no = lista->inicio; no != NULL; no = no->proximo) {
 *         ... no->dado ...
 *     }
 *
 * Eles são SOMENTE LEITURA: alterá-los fora deste módulo quebra a lista.
 * Para inserir e remover, use sempre as funções abaixo.
 */

/**
 * Compara dois elementos da lista.
 *
 * @return  Negativo se a vem antes de b; 0 se empatam; positivo se b vem antes de a.
 */
typedef int (*ListaComparador)(const void *a, const void *b);

typedef struct No No;

struct No {
    void *dado;
    No *proximo;
};

typedef struct Lista {
    No *inicio;     /* NULL se a lista está vazia */
    No *fim;        /* NULL se a lista está vazia */
    int tamanho;
} Lista;

/**
 * Cria uma lista vazia.
 *
 * @return  A lista, ou NULL se faltar memória. Liberar com lista_destruir.
 */
Lista *lista_criar(void);

/**
 * Insere um elemento no início da lista.
 *
 * @param lista   Lista onde o elemento será inserido. Não pode ser NULL.
 * @param dado    Ponteiro para o dado. A lista NÃO copia o dado,
 *                apenas armazena o ponteiro.
 * @return        1 em caso de sucesso, 0 se faltar memória (a lista continua válida).
 */
int lista_inserir_inicio(Lista *lista, void *dado);

/**
 * Insere um elemento no fim da lista.
 *
 * @param lista   Lista onde o elemento será inserido. Não pode ser NULL.
 * @param dado    Ponteiro para o dado. A lista NÃO copia o dado.
 * @return        1 em caso de sucesso, 0 se faltar memória (a lista continua válida).
 */
int lista_inserir_fim(Lista *lista, void *dado);

/**
 * Insere um elemento mantendo a lista em ordem crescente (o menor segundo o
 * comparador fica no início). É a base da fila de prioridade sem heap.
 *
 * Pré-condição: a lista já está ordenada segundo o mesmo comparador (por exemplo,
 * porque só foi preenchida com esta função). Em caso de empate, o novo elemento
 * fica DEPOIS dos que já estavam na lista, então elementos iguais saem na ordem
 * de chegada.
 *
 * @param lista     Lista onde o elemento será inserido. Não pode ser NULL.
 * @param dado      Ponteiro para o dado. A lista NÃO copia o dado.
 * @param comparar  Função que decide a ordem. Não pode ser NULL.
 * @return          1 em caso de sucesso, 0 se faltar memória (a lista continua válida).
 */
int lista_inserir_ordenado(Lista *lista, void *dado, ListaComparador comparar);

/**
 * Remove o primeiro elemento da lista.
 *
 * @param lista  Lista da qual o elemento será removido. Não pode ser NULL.
 * @return       O dado removido (quem chamou passa a ser o dono dele), ou NULL
 *               se a lista está vazia. O nó é liberado pela lista.
 */
void *lista_remover_inicio(Lista *lista);

/**
 * Busca o primeiro elemento igual à chave.
 *
 * @param lista     Lista consultada. Não pode ser NULL.
 * @param chave     Valor procurado, no mesmo formato dos dados da lista.
 * @param comparar  Função que devolve 0 quando dois elementos são iguais.
 *                  É chamada como comparar(chave, dado_da_lista).
 * @return          O dado encontrado (continua na lista; NÃO é removido), ou NULL
 *                  se não houver nenhum igual.
 */
void *lista_buscar(const Lista *lista, const void *chave, ListaComparador comparar);

/**
 * @param lista  Lista consultada. Não pode ser NULL.
 * @return       Quantidade de elementos na lista.
 */
int lista_tamanho(const Lista *lista);

/**
 * @param lista  Lista consultada. Não pode ser NULL.
 * @return       1 se a lista está vazia; 0 caso contrário.
 */
int lista_vazia(const Lista *lista);

/**
 * Libera a lista e todos os seus nós.
 *
 * @param lista    Lista a destruir. Pode ser NULL (nada acontece).
 * @param liberar  Se não for NULL, é chamada para cada dado que ainda estiver
 *                 na lista. Se for NULL, os dados NÃO são liberados (continuam
 *                 pertencendo a quem os inseriu).
 */
void lista_destruir(Lista *lista, void (*liberar)(void *dado));

#endif
