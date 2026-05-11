#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "tipos.h"

int comparar_por_preco(const Registro *a, const Registro *b,
                       MetricasOrdenacao *metricas);

void ordenar_insertion_sort(Registro *dados, int total,
                            ComparadorRegistro comparar,
                            MetricasOrdenacao *metricas);
void ordenar_selection_sort(Registro *dados, int total,
                            ComparadorRegistro comparar,
                            MetricasOrdenacao *metricas);
void ordenar_shell_sort(Registro *dados, int total,
                        ComparadorRegistro comparar,
                        MetricasOrdenacao *metricas);
void ordenar_merge_sort(Registro *dados, int total,
                        ComparadorRegistro comparar,
                        MetricasOrdenacao *metricas);
void ordenar_quick_sort(Registro *dados, int total,
                        ComparadorRegistro comparar,
                        MetricasOrdenacao *metricas);
void ordenar_heap_sort(Registro *dados, int total,
                       ComparadorRegistro comparar,
                       MetricasOrdenacao *metricas);
void ordenar_radix_sort(Registro *dados, int total,
                        ComparadorRegistro comparar,
                        MetricasOrdenacao *metricas);

#endif
