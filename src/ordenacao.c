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

static void quick_sort_recursivo(Registro *dados, int inicio, int fim,
                                 ComparadorRegistro comparar,
                                 MetricasOrdenacao *metricas) {
  if (inicio >= fim)
    return;

  int pivo = particionar(dados, inicio, fim, comparar, metricas);
  quick_sort_recursivo(dados, inicio, pivo - 1, comparar, metricas);
  quick_sort_recursivo(dados, pivo + 1, fim, comparar, metricas);
}

void ordenar_quick_sort(Registro *dados, int total, ComparadorRegistro comparar,
                        MetricasOrdenacao *metricas) {
  if (total <= 1)
    return;
  quick_sort_recursivo(dados, 0, total - 1, comparar, metricas);
}

static void afundar(Registro *dados, int tamanho, int raiz,
                    ComparadorRegistro comparar, MetricasOrdenacao *metricas) {
  int maior = raiz;
  int esquerda = 2 * raiz + 1;
  int direita = 2 * raiz + 2;

  if (esquerda < tamanho &&
      comparar(&dados[esquerda], &dados[maior], metricas) > 0) {
    maior = esquerda;
  }
  if (direita < tamanho &&
      comparar(&dados[direita], &dados[maior], metricas) > 0) {
    maior = direita;
  }

  if (maior != raiz) {
    trocar(&dados[raiz], &dados[maior], metricas);
    afundar(dados, tamanho, maior, comparar, metricas);
  }
}

void ordenar_heap_sort(Registro *dados, int total, ComparadorRegistro comparar,
                       MetricasOrdenacao *metricas) {
  if (total <= 1)
    return;

  for (int i = total / 2 - 1; i >= 0; i--) {
    afundar(dados, total, i, comparar, metricas);
  }

  for (int i = total - 1; i > 0; i--) {
    trocar(&dados[0], &dados[i], metricas);
    afundar(dados, i, 0, comparar, metricas);
  }
}

/*
 * Radix Sort (LSD, base 10) sobre o preco em centavos.
 * Como a chave usada pelo benchmark e composta (preco + estado + municipio +
 * produto + revenda), apos a fase de radix fazemos um insertion sort dentro
 * de cada faixa de precos iguais para garantir o desempate consistente com
 * comparar_por_preco. Isso preserva a validacao "esta_ordenado" do benchmark.
 */
void ordenar_radix_sort(Registro *dados, int total, ComparadorRegistro comparar,
                        MetricasOrdenacao *metricas) {
  if (total <= 1)
    return;

  long long *chaves = malloc((size_t)total * sizeof(long long));
  long long *chaves_aux = malloc((size_t)total * sizeof(long long));
  Registro *aux = malloc((size_t)total * sizeof(Registro));
  if (!chaves || !chaves_aux || !aux) {
    free(chaves);
    free(chaves_aux);
    free(aux);
    return;
  }

  long long max_chave = 0;
  for (int i = 0; i < total; i++) {
    chaves[i] = (long long)(dados[i].valor_venda * 100.0 + 0.5);
    if (chaves[i] > max_chave)
      max_chave = chaves[i];
  }

  for (long long exp = 1; max_chave / exp > 0; exp *= 10) {
    int contador[10] = {0};

    for (int i = 0; i < total; i++) {
      int digito = (int)((chaves[i] / exp) % 10);
      contador[digito]++;
    }
    for (int i = 1; i < 10; i++) {
      contador[i] += contador[i - 1];
    }

    for (int i = total - 1; i >= 0; i--) {
      int digito = (int)((chaves[i] / exp) % 10);
      int pos = --contador[digito];
      aux[pos] = dados[i];
      chaves_aux[pos] = chaves[i];
      metricas->movimentacoes++;
    }

    for (int i = 0; i < total; i++) {
      dados[i] = aux[i];
      chaves[i] = chaves_aux[i];
      metricas->movimentacoes++;
    }
  }

  int inicio_grupo = 0;
  for (int i = 1; i <= total; i++) {
    if (i == total || chaves[i] != chaves[inicio_grupo]) {
      for (int k = inicio_grupo + 1; k < i; k++) {
        Registro chave_reg = dados[k];
        metricas->movimentacoes++;
        int j = k - 1;
        while (j >= inicio_grupo &&
               comparar(&dados[j], &chave_reg, metricas) > 0) {
          dados[j + 1] = dados[j];
          metricas->movimentacoes++;
          j--;
        }
        dados[j + 1] = chave_reg;
        metricas->movimentacoes++;
      }
      inicio_grupo = i;
    }
  }

  free(chaves);
  free(chaves_aux);
  free(aux);
}
