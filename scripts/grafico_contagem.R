args <- commandArgs(trailingOnly = TRUE)
if (length(args) != 2) stop("Uso: Rscript scripts/grafico_contagem.R <csv> <png>")
dados <- read.csv(args[1])
stopifnot(identical(names(dados),
                   c("n", "comparacoes_sem_heap", "comparacoes_com_heap")),
          nrow(dados) > 0, all(dados$n == seq_len(nrow(dados)) - 1),
          all(is.finite(as.matrix(dados))), all(as.matrix(dados) >= 0))
png(args[2], width = 1000, height = 650)
plot(dados$n, dados$comparacoes_sem_heap, type = "p", pch = 20, cex = 0.35,
     col = "steelblue", xlab = "Tamanho da fila antes da inserção (n)",
     ylab = "Comparações por inserção", main = "Inserção em filas de prioridade",
     ylim = c(0, max(1, dados$comparacoes_sem_heap, dados$comparacoes_com_heap)))
points(dados$n, dados$comparacoes_com_heap, col = "darkorange", pch = 20, cex = 0.5)
legend("topleft", c("Sem heap (lista ordenada)", "Com heap"),
       col = c("steelblue", "darkorange"), pch = 20, bg = "white")
invisible(dev.off())
