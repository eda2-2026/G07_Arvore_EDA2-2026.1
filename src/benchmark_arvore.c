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

static void imprimir_bloco(const ResultadoArvore *r, int total) {
    int n_ops = total / 10;
    printf("  %-22s %s\n", "Estrutura:", r->nome);
    printf("  %-22s %d registros unicos inseridos\n", "Insercao:", (int)r->metricas.insercoes);
    printf("  %-22s %.3f ms\n", "Tempo de insercao:", r->tempo_insercao_ms);
    printf("  %-22s %d consultas  ->  %.3f ms  (%.4f ms/busca)\n",
           "Busca:", n_ops, r->tempo_busca_ms,
           n_ops > 0 ? r->tempo_busca_ms / n_ops : 0.0);
    printf("  %-22s %d remocoes   ->  %.3f ms  (%.4f ms/remocao)\n",
           "Remocao:", n_ops, r->tempo_remocao_ms,
           n_ops > 0 ? r->tempo_remocao_ms / n_ops : 0.0);
    printf("  %-22s %d\n", "Altura final:", r->altura);
    printf("  %-22s %lld\n", "Rotacoes:", r->metricas.rotacoes);
    printf("  %-22s %lld\n", "Comparacoes:", r->metricas.comparacoes);
}

static void imprimir_analise(const ResultadoArvore *avl, const ResultadoArvore *rn) {
    printf("\n  --- Comparacao direta ---\n");

    /* insercao */
    if (avl->tempo_insercao_ms < rn->tempo_insercao_ms)
        printf("  Insercao mais rapida  : AVL        (%.3f ms vs %.3f ms)\n",
               avl->tempo_insercao_ms, rn->tempo_insercao_ms);
    else
        printf("  Insercao mais rapida  : Rubro-Negra (%.3f ms vs %.3f ms)\n",
               rn->tempo_insercao_ms, avl->tempo_insercao_ms);

    /* busca */
    if (avl->tempo_busca_ms < rn->tempo_busca_ms)
        printf("  Busca mais rapida     : AVL        (%.4f ms/op vs %.4f ms/op)\n",
               avl->tempo_busca_ms / (avl->metricas.buscas ? avl->metricas.buscas : 1),
               rn->tempo_busca_ms  / (rn->metricas.buscas  ? rn->metricas.buscas  : 1));
    else
        printf("  Busca mais rapida     : Rubro-Negra (%.4f ms/op vs %.4f ms/op)\n",
               rn->tempo_busca_ms  / (rn->metricas.buscas  ? rn->metricas.buscas  : 1),
               avl->tempo_busca_ms / (avl->metricas.buscas ? avl->metricas.buscas : 1));

    /* altura */
    if (avl->altura < rn->altura)
        printf("  Arvore mais baixa     : AVL        (altura %d vs %d)\n",
               avl->altura, rn->altura);
    else if (rn->altura < avl->altura)
        printf("  Arvore mais baixa     : Rubro-Negra (altura %d vs %d)\n",
               rn->altura, avl->altura);
    else
        printf("  Altura igual          : %d nos dois casos\n", avl->altura);

    /* rotacoes */
    if (avl->metricas.rotacoes > rn->metricas.rotacoes)
        printf("  Mais rotacoes         : AVL        (%lld vs %lld) -- balanceamento mais rigido\n",
               avl->metricas.rotacoes, rn->metricas.rotacoes);
    else if (rn->metricas.rotacoes > avl->metricas.rotacoes)
        printf("  Mais rotacoes         : Rubro-Negra (%lld vs %lld)\n",
               rn->metricas.rotacoes, avl->metricas.rotacoes);
    else
        printf("  Rotacoes iguais       : %lld\n", avl->metricas.rotacoes);
}

static void imprimir_tabela(const ResultadoArvore *res, int n, int total) {
    printf("\n");
    printf("################################################################\n");
    printf("##                                                            ##\n");
    printf("##          BENCHMARK DE ARVORES AVL vs RUBRO-NEGRA          ##\n");
    printf("##                                                            ##\n");
    printf("################################################################\n");
    printf("  Registros na base : %d\n", total);
    printf("  Chave de busca    : valor_venda (double)\n");
    printf("  Operacoes medidas : insercao (base completa), busca e remocao (%d cada)\n\n",
           total / 10);

    for (int i = 0; i < n; i++) {
        printf("  ════════════════════════════════════════\n");
        imprimir_bloco(&res[i], total);
    }
    printf("  ════════════════════════════════════════\n");

    imprimir_analise(&res[0], &res[1]);
    printf("\n################################################################\n\n");
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
