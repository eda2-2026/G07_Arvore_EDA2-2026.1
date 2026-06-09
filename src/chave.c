#include "chave.h"

#include <string.h>

int comparar_registro(const Registro *a, const Registro *b) {
  if (a->valor_venda < b->valor_venda)
    return -1;
  if (a->valor_venda > b->valor_venda)
    return 1;

  int r = strcmp(a->estado, b->estado);
  if (r != 0)
    return r;

  r = strcmp(a->municipio, b->municipio);
  if (r != 0)
    return r;

  r = strcmp(a->produto, b->produto);
  if (r != 0)
    return r;

  return strcmp(a->revenda, b->revenda);
}
