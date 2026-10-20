# ED-2026.2

Projeto de Estruturas de Dados / Programação 2 (ECOM008), IC/UFAL: codificação de Huffman, SAT Solver (DIMACS), Dijkstra e Kruskal, e a contagem de comparações entre fila de prioridade sem heap e com heap.

As regras de trabalho estão no [CONTRIBUTING.md](CONTRIBUTING.md).

## Dependências

Linux ou WSL:

```bash
sudo apt install build-essential libcunit1-dev valgrind r-base
```

## Comandos

| Comando | O que faz |
|---|---|
| `make` | compila todos os programas em `build/` |
| `make huffman`, `make sat`, `make dijkstra`, `make kruskal` | compila um programa |
| `make test` | compila e roda os testes CUnit |
| `make valgrind` | roda os testes no valgrind |
| `make contagem` | gera `build/contagem.csv` e o gráfico `build/contagem.png` |
| `make clean` | apaga `build/` |

## Estrutura

| Pasta | Conteúdo |
|---|---|
| `include/` | arquivos `.h` (interfaces documentadas) |
| `src/` | implementação; `src/main_<programa>.c` é o ponto de entrada de cada programa |
| `tests/` | testes CUnit |
| `scripts/` | script R do gráfico da contagem |
| `data/` | arquivos de exemplo (`.cnf`, grafos, arquivos para o Huffman) |
| `build/` | tudo o que o `make` gera (não versionado) |

## Testes

Para testar um módulo novo:

1. Copie `tests/test_exemplo.c` para `tests/test_<modulo>.c` e escreva os testes.
2. Declare `registrar_testes_<modulo>` em `tests/testes.h`.
3. Inclua a função na lista de `tests/main_testes.c`.

`make test` termina com erro se algum teste falhar.

## Contagem

O programa `build/contagem` imprime o CSV na saída padrão, com o cabeçalho `n,comparacoes_sem_heap,comparacoes_com_heap`. O script `scripts/grafico_contagem.R` recebe o caminho do CSV e o do PNG:

```bash
Rscript scripts/grafico_contagem.R build/contagem.csv build/contagem.png
```

Depois de gerar o gráfico, o `make contagem` abre a imagem com `xdg-open`. Em outra máquina, troque o comando: `make contagem ABRIR=wslview`.

Por padrao sao 5000 insercoes, usando `time(NULL)` como semente. Para repetir
um resultado no mesmo ambiente: `make contagem N=5000 SEMENTE=42`.
O executavel aceita `./build/contagem [quantidade [semente]]`.
Cada linha do CSV registra uma insercao; `n` e o tamanho antes dela, desde zero.
O mesmo valor sorteado entra nas duas filas, com contadores locais separados,
zerados por insercao e incrementados apenas pelo comparador.

`include/filas.h` e `src/filas.c` oferecem somente criar, inserir e destruir
na lista ordenada e no min-heap, com dados `void*`. As filas nao liberam os dados:
`contagem_gerar` faz isso. Sao dependencias minimas da contagem, nao os TADs
completos das issues #5 a #8. Substituir pelas interfaces do grupo quando existirem.

Os quatro testes da contagem usam o executor CUnit existente e verificam
contagens, empates, crescimento do heap, comparador e reproducibilidade do CSV.
O grafico usa apenas R base. Compilacao, testes e geracao CSV/PNG foram
verificados no Windows; execucao em Linux/WSL e Valgrind continuam pendentes.
Uso de IA: implementacao e verificacoes da contagem preparadas com Codex.
