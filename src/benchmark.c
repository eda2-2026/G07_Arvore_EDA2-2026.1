#include "benchmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>
#endif

#include "ordenacao.h"

typedef struct {
    const MetodoOrdenacao *metodo;
    int elementos;
    double milissegundos;
    long long comparacoes;
    long long movimentacoes;
    size_t memoria_bytes;
    int ordenado;
} ResultadoBenchmark;

static double agora_ms(void) {
    return (double)clock() * 1000.0 / (double)CLOCKS_PER_SEC;
}

static long memoria_processo_kb(void) {
#if defined(__unix__) || defined(__APPLE__)
    struct rusage uso;
    if (getrusage(RUSAGE_SELF, &uso) == 0) return uso.ru_maxrss;
#endif
    return 0;
}

static int esta_ordenado(const Registro *dados, int total) {
    MetricasOrdenacao metricas = {0, 0};
    for (int i = 1; i < total; i++) {
        if (comparar_por_preco(&dados[i - 1], &dados[i], &metricas) > 0) {
            return 0;
        }
    }
    return 1;
}

static int quantidade_para_metodo(const MetodoOrdenacao *metodo, int total) {
    if (metodo->limite_recomendado > 0 &&
        total > metodo->limite_recomendado) {
        return metodo->limite_recomendado;
    }
    return total;
}

static ResultadoBenchmark medir_metodo(const Registro *originais, int total,
                                       const MetodoOrdenacao *metodo) {
    ResultadoBenchmark resultado;
    memset(&resultado, 0, sizeof(resultado));
    resultado.metodo = metodo;
    resultado.elementos = quantidade_para_metodo(metodo, total);
    resultado.memoria_bytes = (size_t)resultado.elementos * sizeof(Registro);

    Registro *copia = malloc(resultado.memoria_bytes);
    if (!copia) return resultado;
    memcpy(copia, originais, resultado.memoria_bytes);

    MetricasOrdenacao metricas = {0, 0};
    double inicio = agora_ms();
    metodo->ordenar(copia, resultado.elementos, comparar_por_preco, &metricas);
    double fim = agora_ms();

    resultado.milissegundos = fim - inicio;
    resultado.comparacoes = metricas.comparacoes;
    resultado.movimentacoes = metricas.movimentacoes;
    resultado.ordenado = esta_ordenado(copia, resultado.elementos);

    free(copia);
    return resultado;
}

static void imprimir_linha_resultado(const ResultadoBenchmark *resultado,
                                     int total_base) {
    char observacao[40] = "base completa";
    if (resultado->elementos < total_base) {
        snprintf(observacao, sizeof(observacao), "amostra de %d",
                 resultado->elementos);
    }

    printf("%-16s %9d %12.3f %15lld %15lld %11.2f MB %-14s %s\n",
           resultado->metodo->nome,
           resultado->elementos,
           resultado->milissegundos,
           resultado->comparacoes,
           resultado->movimentacoes,
           resultado->memoria_bytes / (1024.0 * 1024.0),
           resultado->ordenado ? "ok" : "falhou",
           observacao);
}

static void texto_observacao(const ResultadoBenchmark *resultado, int total_base,
                             char *saida, size_t tamanho) {
    if (resultado->elementos < total_base) {
        snprintf(saida, tamanho, "amostra de %d", resultado->elementos);
    } else {
        snprintf(saida, tamanho, "base completa");
    }
}

static void gerar_csv_resultados(const ResultadoBenchmark resultados[],
                                 int total_resultados, int total_base) {
    FILE *arquivo = fopen("benchmark_resultados.csv", "w");
    if (!arquivo) return;

    fprintf(arquivo,
            "Metodo;Complexidade;Registros;Tempo_ms;Comparacoes;Movimentacoes;RAM_MB;Ordenado;Observacao\n");
    for (int i = 0; i < total_resultados; i++) {
        char observacao[40];
        texto_observacao(&resultados[i], total_base, observacao,
                         sizeof(observacao));
        fprintf(arquivo, "%s;%s;%d;%.3f;%lld;%lld;%.2f;%s;%s\n",
                resultados[i].metodo->nome,
                resultados[i].metodo->complexidade_media,
                resultados[i].elementos,
                resultados[i].milissegundos,
                resultados[i].comparacoes,
                resultados[i].movimentacoes,
                resultados[i].memoria_bytes / (1024.0 * 1024.0),
                resultados[i].ordenado ? "sim" : "nao",
                observacao);
    }

    fclose(arquivo);
}

static void gerar_html_resultados(const ResultadoBenchmark resultados[],
                                  int total_resultados, int total_base) {
    FILE *arquivo = fopen("benchmark_relatorio.html", "w");
    if (!arquivo) return;

    fprintf(arquivo, "<!doctype html>\n<html lang=\"pt-BR\">\n<head>\n");
    fprintf(arquivo, "<meta charset=\"utf-8\">\n");
    fprintf(arquivo, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n");
    fprintf(arquivo, "<title>Benchmark de Ordenacao</title>\n");
    fprintf(arquivo, "<style>\n");
    fprintf(arquivo, "body{font-family:Arial,sans-serif;margin:0;background:#f6f7f9;color:#1f2933}main{max-width:1120px;margin:0 auto;padding:32px 20px}h1{font-size:28px;margin:0 0 8px}p{line-height:1.5}.grid{display:grid;grid-template-columns:repeat(3,1fr);gap:12px;margin:22px 0}.kpi{background:white;border:1px solid #d9dee7;border-radius:8px;padding:16px}.kpi strong{display:block;font-size:24px;margin-top:6px}table{width:100%%;border-collapse:collapse;background:white;border:1px solid #d9dee7}th,td{padding:12px;border-bottom:1px solid #e5e9f0;text-align:left;font-size:14px}th{background:#e9edf3}tr:last-child td{border-bottom:0}.ok{font-weight:bold;color:#176b43}.note{background:#fff7e6;border:1px solid #f2d48a;padding:12px;border-radius:8px}@media(max-width:760px){.grid{grid-template-columns:1fr}th,td{font-size:12px;padding:8px}}\n");
    fprintf(arquivo, "</style>\n</head>\n<body>\n<main>\n");
    fprintf(arquivo, "<h1>Benchmark de Ordenacao - Precos de Combustiveis</h1>\n");
    fprintf(arquivo, "<p>Relatorio gerado pelo programa em C. O CSV foi carregado uma vez e cada algoritmo recebeu uma copia identica dos registros em memoria.</p>\n");
    fprintf(arquivo, "<div class=\"grid\">\n");
    fprintf(arquivo, "<div class=\"kpi\">Registros na base<strong>%d</strong></div>\n", total_base);
    fprintf(arquivo, "<div class=\"kpi\">Chave<strong>Preco + local</strong></div>\n");
    fprintf(arquivo, "<div class=\"kpi\">Metodos<strong>%d</strong></div>\n", total_resultados);
    fprintf(arquivo, "</div>\n");
    fprintf(arquivo, "<p class=\"note\">Algoritmos O(n^2) (Insertion e Selection) usam amostra limitada para a apresentacao nao travar. Shell, Merge, Quick, Heap e Radix Sort rodam na base completa.</p>\n");
    fprintf(arquivo, "<table><thead><tr><th>Metodo</th><th>Complexidade</th><th>Registros</th><th>Tempo (ms)</th><th>Comparacoes</th><th>Movimentacoes</th><th>RAM dados</th><th>Status</th><th>Observacao</th></tr></thead><tbody>\n");

    for (int i = 0; i < total_resultados; i++) {
        char observacao[40];
        texto_observacao(&resultados[i], total_base, observacao,
                         sizeof(observacao));
        fprintf(arquivo,
                "<tr><td>%s</td><td>%s</td><td>%d</td><td>%.3f</td><td>%lld</td><td>%lld</td><td>%.2f MB</td><td class=\"ok\">%s</td><td>%s</td></tr>\n",
                resultados[i].metodo->nome,
                resultados[i].metodo->complexidade_media,
                resultados[i].elementos,
                resultados[i].milissegundos,
                resultados[i].comparacoes,
                resultados[i].movimentacoes,
                resultados[i].memoria_bytes / (1024.0 * 1024.0),
                resultados[i].ordenado ? "ordenado" : "falhou",
                observacao);
    }

    fprintf(arquivo, "</tbody></table>\n");
    fprintf(arquivo, "</main>\n</body>\n</html>\n");
    fclose(arquivo);
}

static void imprimir_amostra_extremos(const Registro *originais, int total) {
    int elementos = total;
    size_t bytes = (size_t)elementos * sizeof(Registro);
    Registro *copia = malloc(bytes);
    if (!copia) return;
    memcpy(copia, originais, bytes);

    MetricasOrdenacao metricas = {0, 0};
    ordenar_quick_sort(copia, elementos, comparar_por_preco, &metricas);

    printf("\nAmostra apos ordenar por Valor de Venda:\n");
    printf("  Menores precos:\n");
    for (int i = 0; i < 3 && i < elementos; i++) {
        printf("    R$ %5.2f | %-2s | %-18s | %-9s | %s\n",
               copia[i].valor_venda, copia[i].estado, copia[i].municipio,
               copia[i].produto, copia[i].revenda);
    }

    printf("  Maiores precos:\n");
    for (int i = elementos - 3; i < elementos; i++) {
        if (i < 0) continue;
        printf("    R$ %5.2f | %-2s | %-18s | %-9s | %s\n",
               copia[i].valor_venda, copia[i].estado, copia[i].municipio,
               copia[i].produto, copia[i].revenda);
    }

    free(copia);
}

void executar_benchmark(const Registro *originais, int total,
                        const MetodoOrdenacao metodos[], int total_metodos) {
    printf("\n================ Benchmark de Ordenacao ================\n");
    printf("Chave de ordenacao: valor de venda, estado, municipio, produto e revenda.\n");
    printf("Os metodos O(n^2) usam amostra limitada para a apresentacao nao travar.\n\n");

    printf("%-16s %9s %12s %15s %15s %14s %-14s %s\n",
           "Metodo", "Registros", "Tempo(ms)", "Comparacoes",
           "Movimentacoes", "RAM dados", "Ordenacao", "Observacao");
    printf("--------------------------------------------------------------------------------------------------------\n");

    ResultadoBenchmark *resultados =
        malloc((size_t)total_metodos * sizeof(ResultadoBenchmark));
    if (!resultados) return;

    for (int i = 0; i < total_metodos; i++) {
        resultados[i] = medir_metodo(originais, total, &metodos[i]);
        imprimir_linha_resultado(&resultados[i], total);
    }

    gerar_csv_resultados(resultados, total_metodos, total);
    gerar_html_resultados(resultados, total_metodos, total);
    printf("\nArquivos gerados: benchmark_resultados.csv e benchmark_relatorio.html\n");

    long memoria_kb = memoria_processo_kb();
    if (memoria_kb > 0) {
#if defined(__APPLE__)
        printf("\nPico aproximado do processo: %.2f MB\n",
               memoria_kb / (1024.0 * 1024.0));
#else
        printf("\nPico aproximado do processo: %.2f MB\n", memoria_kb / 1024.0);
#endif
    }

    imprimir_amostra_extremos(originais, total);
    free(resultados);
}
