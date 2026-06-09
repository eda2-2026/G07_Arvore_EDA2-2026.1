#ifndef RUBRO_NEGRA_H
#define RUBRO_NEGRA_H

#include "tipos.h"

typedef enum { VERMELHO, PRETO } CorNo;

typedef struct NoRN {
    Registro reg;
    CorNo cor;
    struct NoRN *esq, *dir, *pai;
} NoRN;

typedef struct {
    NoRN *raiz;
    NoRN *nil;
    MetricasArvore metricas;
} ArvoreRN;

ArvoreRN rn_criar(void);
int      rn_inserir(ArvoreRN *a, Registro reg);
NoRN    *rn_buscar(ArvoreRN *a, const Registro *chave);
int      rn_remover(ArvoreRN *a, Registro reg);
void     rn_inorder(const ArvoreRN *a, void (*visita)(const Registro *));
int      rn_altura(const ArvoreRN *a);
int      rn_contar(const ArvoreRN *a);
void     rn_destruir(ArvoreRN *a);

#endif
