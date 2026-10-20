CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g
FONTES  = main.c dimacs.c formula.c clausula.c interpretacao.c vetor.c sat.c

satsolver: $(FONTES) $(wildcard *.h)
	$(CC) $(CFLAGS) -o satsolver $(FONTES)

test: satsolver
	sh run_tests.sh

clean:
	rm -f satsolver
