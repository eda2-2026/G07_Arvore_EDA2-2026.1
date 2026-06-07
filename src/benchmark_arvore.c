#include "benchmark_arvore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "avl.h"
#include "rubro_negra.h"

typedef struct {
    const char *nome;
    double      tempo_insercao_ms;
    double      tempo_busca_ms;
    double      tempo_remocao_ms;
    int         altura;
    MetricasArvore metricas;
} ResultadoArvore;

static double agora_ms(void) {
    return (double)clock() * 1000.0 / (double)CLOCKS_PER_SEC;
}

static ResultadoArvore medir_avl(const Registro *originais, int total) {
    ResultadoArvore r;
    memset(&r, 0, sizeof(r));
    r.nome = "AVL";

    ArvoreAVL a = avl_criar();

    double t0 = agora_ms();
    for (int i = 0; i < total; i++)
        avl_inserir(&a, originais[i]);
    r.tempo_insercao_ms = agora_ms() - t0;

    int n_ops = total / 10;
    t0 = agora_ms();
    for (int i = 0; i < n_ops; i++)
        avl_buscar(&a, originais[i * 10].valor_venda);
    r.tempo_busca_ms = agora_ms() - t0;

    t0 = agora_ms();
    for (int i = 0; i < n_ops; i++)
        avl_remover(&a, originais[i * 10].valor_venda);
    r.tempo_remocao_ms = agora_ms() - t0;

    r.altura   = avl_altura(&a);
    r.metricas = a.metricas;
    avl_destruir(&a);
    return r;
}

static ResultadoArvore medir_rn(const Registro *originais, int total) {
    ResultadoArvore r;
    memset(&r, 0, sizeof(r));
    r.nome = "Rubro-Negra";

    ArvoreRN a = rn_criar();

    double t0 = agora_ms();
    for (int i = 0; i < total; i++)
        rn_inserir(&a, originais[i]);
    r.tempo_insercao_ms = agora_ms() - t0;

    int n_ops = total / 10;
    t0 = agora_ms();
    for (int i = 0; i < n_ops; i++)
        rn_buscar(&a, originais[i * 10].valor_venda);
    r.tempo_busca_ms = agora_ms() - t0;

    t0 = agora_ms();
    for (int i = 0; i < n_ops; i++)
        rn_remover(&a, originais[i * 10].valor_venda);
    r.tempo_remocao_ms = agora_ms() - t0;

    r.altura   = rn_altura(&a);
    r.metricas = a.metricas;
    rn_destruir(&a);
    return r;
}

static void imprimir_tabela(const ResultadoArvore *res, int n, int total) {
    printf("\n================ Benchmark de Arvores ================\n");
    printf("Chave: valor_venda. Insercoes: %d. Buscas/Remocoes: %d cada.\n\n",
           total, total / 10);
    printf("%-12s %12s %11s %11s %8s %12s %12s\n",
           "Estrutura", "Ins(ms)", "Bus(ms)", "Rem(ms)",
           "Altura", "Rotacoes", "Comparacoes");
    printf("-----------------------------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-12s %12.3f %11.3f %11.3f %8d %12lld %12lld\n",
               res[i].nome,
               res[i].tempo_insercao_ms,
               res[i].tempo_busca_ms,
               res[i].tempo_remocao_ms,
               res[i].altura,
               res[i].metricas.rotacoes,
               res[i].metricas.comparacoes);
    }
}

static void gerar_csv(const ResultadoArvore *res, int n, int total) {
    FILE *f = fopen("benchmark_arvore_resultados.csv", "w");
    if (!f) return;
    fprintf(f, "Estrutura;Insercoes;Ins_ms;Buscas;Bus_ms;Remocoes;Rem_ms;Altura;Rotacoes;Comparacoes\n");
    for (int i = 0; i < n; i++) {
        fprintf(f, "%s;%d;%.3f;%d;%.3f;%d;%.3f;%d;%lld;%lld\n",
                res[i].nome, total, res[i].tempo_insercao_ms,
                total / 10, res[i].tempo_busca_ms,
                total / 10, res[i].tempo_remocao_ms,
                res[i].altura, res[i].metricas.rotacoes,
                res[i].metricas.comparacoes);
    }
    fclose(f);
}

static void gerar_html(const ResultadoArvore *res, int n, int total) {
    FILE *f = fopen("benchmark_arvore_relatorio.html", "w");
    if (!f) return;
    fprintf(f, "<!doctype html>\n<html lang=\"pt-BR\">\n<head>\n");
    fprintf(f, "<meta charset=\"utf-8\">\n");
    fprintf(f, "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">\n");
    fprintf(f, "<title>Benchmark de Arvores</title>\n");
    fprintf(f, "<style>body{font-family:Arial,sans-serif;margin:0;background:#f6f7f9;color:#1f2933}"
               "main{max-width:960px;margin:0 auto;padding:32px 20px}h1{font-size:26px;margin:0 0 8px}"
               "table{width:100%%;border-collapse:collapse;background:white;border:1px solid #d9dee7}"
               "th,td{padding:11px;border-bottom:1px solid #e5e9f0;text-align:left;font-size:14px}"
               "th{background:#e9edf3}tr:last-child td{border-bottom:0}</style>\n");
    fprintf(f, "</head>\n<body>\n<main>\n");
    fprintf(f, "<h1>Benchmark de Arvores AVL vs Rubro-Negra</h1>\n");
    fprintf(f, "<p>Registros inseridos: %d &nbsp;|&nbsp; Buscas e remocoes: %d cada.</p>\n",
            total, total / 10);
    fprintf(f, "<table><thead><tr>"
               "<th>Estrutura</th><th>Ins (ms)</th><th>Bus (ms)</th><th>Rem (ms)</th>"
               "<th>Altura</th><th>Rotacoes</th><th>Comparacoes</th>"
               "</tr></thead><tbody>\n");
    for (int i = 0; i < n; i++) {
        fprintf(f, "<tr><td>%s</td><td>%.3f</td><td>%.3f</td><td>%.3f</td>"
                   "<td>%d</td><td>%lld</td><td>%lld</td></tr>\n",
                res[i].nome, res[i].tempo_insercao_ms,
                res[i].tempo_busca_ms, res[i].tempo_remocao_ms,
                res[i].altura, res[i].metricas.rotacoes,
                res[i].metricas.comparacoes);
    }
    fprintf(f, "</tbody></table>\n</main>\n</body>\n</html>\n");
    fclose(f);
}

void executar_benchmark_arvore(const Registro *originais, int total) {
    ResultadoArvore resultados[2];
    resultados[0] = medir_avl(originais, total);
    resultados[1] = medir_rn(originais, total);

    imprimir_tabela(resultados, 2, total);
    gerar_csv(resultados, 2, total);
    gerar_html(resultados, 2, total);
    printf("\nArquivos gerados: benchmark_arvore_resultados.csv e benchmark_arvore_relatorio.html\n");
}
