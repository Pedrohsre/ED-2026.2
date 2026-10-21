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

## Como rodar cada programa

Depois de `make`:

| Programa | Exemplo |
|---|---|
| Huffman | `./build/huffman c foto.png` gera `foto.png.huff`; `./build/huffman d foto.png.huff volta.png` descompacta |
| SAT Solver | `./build/sat data/sat_slide.cnf` imprime `SAT` com a atribuição, ou `UNSAT` |
| Kruskal | `./build/kruskal data/grafo_exemplo.txt` imprime as arestas da árvore geradora mínima e o peso total |
| Dijkstra | `./build/dijkstra data/grafo_dijkstra.txt 0` imprime a distância e o caminho a partir do vértice 0 |
| Contagem | `make contagem` gera o CSV e o gráfico e abre a imagem |

O arquivo de grafo tem o número de vértices e de arestas na primeira linha e, depois, uma aresta `u v peso` por linha (vértices de 0 a V-1). `sh scripts/testar_sat.sh` roda o SAT com todos os `.cnf` de `data/`.

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

Por padrão são 5000 inserções, com `time(NULL)` como semente. Para repetir um resultado: `make contagem N=5000 SEMENTE=42`. O executável aceita `./build/contagem [quantidade [semente]]`.

Cada linha do CSV registra uma inserção, e `n` é o tamanho da fila antes dela, a partir de zero. O mesmo valor sorteado entra nas duas filas, com contadores separados, zerados a cada inserção e incrementados só pelo comparador.

A fila sem heap (lista ordenada) está em `include/filas.h`; a fila com heap, em `include/fp_heap.h`. O gráfico usa só o R básico.
