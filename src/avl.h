#ifndef AVL_H
#define AVL_H

#include "tipos.h"

typedef struct NoAVL {
    Registro reg;
    int altura;
    struct NoAVL *esq, *dir;
} NoAVL;

typedef struct {
    NoAVL *raiz;
    MetricasArvore metricas;
} ArvoreAVL;

ArvoreAVL avl_criar(void);
int       avl_inserir(ArvoreAVL *a, Registro reg);
NoAVL    *avl_buscar(ArvoreAVL *a, const Registro *chave);
int       avl_remover(ArvoreAVL *a, Registro reg);
void      avl_inorder(const ArvoreAVL *a, void (*visita)(const Registro *));
int       avl_altura(const ArvoreAVL *a);
int       avl_contar(const ArvoreAVL *a);
void      avl_destruir(ArvoreAVL *a);

#endif
