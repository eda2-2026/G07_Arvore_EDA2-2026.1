#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "benchmark.h"
#include "benchmark_arvore.h"
#include "csv.h"
#include "ordenacao.h"

#define ALUNO1_NOME "VICTOR LEANDRO ROCHA DE ASSIS"
#define ALUNO1_MATRICULA "222021826"
#define ALUNO2_NOME "PEDRO LUIZ FONSECA DA SILVA"
#define ALUNO2_MATRICULA "231036980"

#define CSV_PADRAO "02-cados-abertos-preco-gasolina-etanol.csv"

static void imprimir_cabecalho(void) {
  printf("============================================================\n");
  printf("  Ordenacao de Precos de Combustiveis - EDA2 2026.1\n");
  printf("  Grupo 07\n");
  printf("  %-38s Matricula: %s\n", ALUNO1_NOME, ALUNO1_MATRICULA);
  printf("  %-38s Matricula: %s\n", ALUNO2_NOME, ALUNO2_MATRICULA);
  printf("============================================================\n");
}

static void imprimir_metodos(const MetodoOrdenacao metodos[], int total) {
  printf("\nMetodos implementados:\n");
  for (int i = 0; i < total; i++) {
    printf("  - %-14s | %-9s | %s\n", metodos[i].nome,
           metodos[i].complexidade_media, metodos[i].descricao);
  }
}

int main(int argc, char *argv[]) {
  const char *caminho_csv = CSV_PADRAO;
  if (argc >= 2)
    caminho_csv = argv[1];

  const MetodoOrdenacao metodos[] = {
      {"Insertion Sort", "O(n^2)", "insere cada item na parte ja ordenada",
       5000, ordenar_insertion_sort},
      {"Selection Sort", "O(n^2)",
       "procura o menor item e coloca na posicao correta", 5000,
       ordenar_selection_sort},
      {"Shell Sort", "O(n^1.3)",
       "Insertion Sort com gaps decrescentes (sequencia de Knuth)", 0,
       ordenar_shell_sort},
      {"Merge Sort", "O(n log n)",
       "divide a lista e intercala partes ordenadas", 0, ordenar_merge_sort},
      {"Quick Sort", "O(n log n)",
       "particiona em torno de um pivo e ordena recursivamente", 0,
       ordenar_quick_sort},
      {"Heap Sort", "O(n log n)",
       "constroi um heap maximo e extrai o maior repetidamente", 0,
       ordenar_heap_sort},
      {"Radix Sort", "O(n*d)",
       "distribui por digitos do preco e desempata pela chave composta", 0,
       ordenar_radix_sort},
  };

  imprimir_cabecalho();
  imprimir_metodos(metodos, (int)(sizeof(metodos) / sizeof(metodos[0])));

  printf("\nCarregando CSV: %s\n", caminho_csv);
  int total = 0;
  double inicio = (double)clock() * 1000.0 / (double)CLOCKS_PER_SEC;
  Registro *registros = carregar_csv(caminho_csv, &total);
  double fim = (double)clock() * 1000.0 / (double)CLOCKS_PER_SEC;

  if (!registros || total == 0) {
    fprintf(stderr, "Falha ao carregar a base de dados.\n");
    free(registros);
    return 1;
  }

  printf("Registros carregados: %d\n", total);
  printf("Tempo de leitura do arquivo: %.3f ms\n", fim - inicio);
  printf("Memoria da base em RAM: %.2f MB\n",
         (total * sizeof(Registro)) / (1024.0 * 1024.0));
  printf("Observacao: o benchmark abaixo nao relera o CSV; cada algoritmo "
         "recebe uma copia identica em RAM.\n");

  executar_benchmark(registros, total, metodos,
                     (int)(sizeof(metodos) / sizeof(metodos[0])));

  executar_benchmark_arvore(registros, total);

  free(registros);
  return 0;
}
