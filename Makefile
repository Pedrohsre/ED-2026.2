# make             compila todos os programas em build/
# make <programa>  compila um programa (huffman, sat, dijkstra, kruskal, contagem)
# make test        compila e roda os testes CUnit
# make valgrind    roda os testes no valgrind
# make contagem    gera o CSV da contagem e o gráfico (comando da apresentação)
# make clean       apaga build/
#
# Convenções:
#   src/main_<programa>.c   ponto de entrada de cada programa, vira build/<programa>
#   src/*.c (os demais)     módulos, ligados a todos os programas e aos testes
#   tests/*.c               testes CUnit, ligados em um só executável (build/testes)

CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -Iinclude -MMD -MP
LDLIBS = -lm
BUILD  = build
ABRIR ?= xdg-open

MAINS       = $(wildcard src/main_*.c)
MODULOS     = $(filter-out $(MAINS), $(wildcard src/*.c))
PROGRAMAS   = $(MAINS:src/main_%.c=$(BUILD)/%)
OBJ_MODULOS = $(MODULOS:src/%.c=$(BUILD)/%.o)
OBJ_TESTES  = $(patsubst tests/%.c, $(BUILD)/tests/%.o, $(wildcard tests/*.c))

.PHONY: all huffman sat dijkstra kruskal contagem test valgrind clean
# Não apaga os .o intermediários (senão o make recompila tudo a cada chamada)
.SECONDARY:

all: $(PROGRAMAS)

huffman sat dijkstra kruskal: %: $(BUILD)/%

$(BUILD)/%: $(BUILD)/main_%.o $(OBJ_MODULOS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD)/testes: $(OBJ_TESTES) $(OBJ_MODULOS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS) -lcunit

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/tests/%.o: tests/%.c | $(BUILD)/tests
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD) $(BUILD)/tests:
	mkdir -p $@

test: $(BUILD)/testes
	./$(BUILD)/testes

valgrind: $(BUILD)/testes
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(BUILD)/testes

# O programa da contagem imprime o CSV na saída padrão; o script R recebe o CSV e o PNG.
contagem: $(BUILD)/contagem
	./$(BUILD)/contagem > $(BUILD)/contagem.csv
	Rscript scripts/grafico_contagem.R $(BUILD)/contagem.csv $(BUILD)/contagem.png
	-$(ABRIR) $(BUILD)/contagem.png

clean:
	rm -rf $(BUILD)

# Dependências dos .h geradas pelo -MMD: alterar um .h recompila quem o inclui
-include $(wildcard $(BUILD)/*.d $(BUILD)/tests/*.d)
