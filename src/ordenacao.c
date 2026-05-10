#include "ordenacao.h"

#include <stdlib.h>
#include <string.h>

static void trocar(Registro *a, Registro *b, MetricasOrdenacao *metricas) {
  Registro temp = *a;
  *a = *b;
  *b = temp;
  metricas->movimentacoes += 3;
}

int comparar_por_preco(const Registro *a, const Registro *b,
                       MetricasOrdenacao *metricas) {
  if (metricas)
    metricas->comparacoes++;

  if (a->valor_venda < b->valor_venda)
    return -1;
  if (a->valor_venda > b->valor_venda)
    return 1;

  int estado = strcmp(a->estado, b->estado);
  if (estado != 0)
    return estado;

  int municipio = strcmp(a->municipio, b->municipio);
  if (municipio != 0)
    return municipio;

  int produto = strcmp(a->produto, b->produto);
  if (produto != 0)
    return produto;

  return strcmp(a->revenda, b->revenda);
}

void ordenar_insertion_sort(Registro *dados, int total,
                            ComparadorRegistro comparar,
                            MetricasOrdenacao *metricas) {
  for (int i = 1; i < total; i++) {
    Registro chave = dados[i];
    metricas->movimentacoes++;
    int j = i - 1;

    while (j >= 0 && comparar(&dados[j], &chave, metricas) > 0) {
      dados[j + 1] = dados[j];
      metricas->movimentacoes++;
      j--;
    }
    dados[j + 1] = chave;
    metricas->movimentacoes++;
  }
}

void ordenar_selection_sort(Registro *dados, int total,
                            ComparadorRegistro comparar,
                            MetricasOrdenacao *metricas) {
  for (int i = 0; i < total - 1; i++) {
    int menor = i;
    for (int j = i + 1; j < total; j++) {
      if (comparar(&dados[j], &dados[menor], metricas) < 0)
        menor = j;
    }
    if (menor != i)
      trocar(&dados[i], &dados[menor], metricas);
  }
}

static void intercalar(Registro *dados, Registro *auxiliar, int inicio,
                       int meio, int fim, ComparadorRegistro comparar,
                       MetricasOrdenacao *metricas) {
  int esquerda = inicio;
  int direita = meio + 1;
  int posicao = inicio;

  while (esquerda <= meio && direita <= fim) {
    if (comparar(&dados[esquerda], &dados[direita], metricas) <= 0) {
      auxiliar[posicao++] = dados[esquerda++];
    } else {
      auxiliar[posicao++] = dados[direita++];
    }
    metricas->movimentacoes++;
  }

  while (esquerda <= meio) {
    auxiliar[posicao++] = dados[esquerda++];
    metricas->movimentacoes++;
  }

  while (direita <= fim) {
    auxiliar[posicao++] = dados[direita++];
    metricas->movimentacoes++;
  }

  for (int i = inicio; i <= fim; i++) {
    dados[i] = auxiliar[i];
    metricas->movimentacoes++;
  }
}

static void merge_sort_recursivo(Registro *dados, Registro *auxiliar,
                                 int inicio, int fim,
                                 ComparadorRegistro comparar,
                                 MetricasOrdenacao *metricas) {
  if (inicio >= fim)
    return;

  int meio = inicio + (fim - inicio) / 2;
  merge_sort_recursivo(dados, auxiliar, inicio, meio, comparar, metricas);
  merge_sort_recursivo(dados, auxiliar, meio + 1, fim, comparar, metricas);
  intercalar(dados, auxiliar, inicio, meio, fim, comparar, metricas);
}

void ordenar_merge_sort(Registro *dados, int total, ComparadorRegistro comparar,
                        MetricasOrdenacao *metricas) {
  Registro *auxiliar = malloc((size_t)total * sizeof(Registro));
  if (!auxiliar)
    return;

  merge_sort_recursivo(dados, auxiliar, 0, total - 1, comparar, metricas);
  free(auxiliar);
}

static int particionar(Registro *dados, int inicio, int fim,
                       ComparadorRegistro comparar,
                       MetricasOrdenacao *metricas) {
  int meio = inicio + (fim - inicio) / 2;
  trocar(&dados[meio], &dados[fim], metricas);
  Registro pivo = dados[fim];
  metricas->movimentacoes++;

  int menor = inicio - 1;
  for (int atual = inicio; atual < fim; atual++) {
    if (comparar(&dados[atual], &pivo, metricas) <= 0) {
      menor++;
      if (menor != atual)
        trocar(&dados[menor], &dados[atual], metricas);
    }
  }

  trocar(&dados[menor + 1], &dados[fim], metricas);
  return menor + 1;
}
