#include "avl.h"

#include <stdlib.h>

#include "chave.h"

static int altura_no(const NoAVL *n) {
    return n ? n->altura : 0;
}

static void atualizar_altura(NoAVL *n) {
    int he = altura_no(n->esq);
    int hd = altura_no(n->dir);
    n->altura = 1 + (he > hd ? he : hd);
}

static int fator(const NoAVL *n) {
    return altura_no(n->esq) - altura_no(n->dir);
}

static NoAVL *novo_no(Registro reg) {
    NoAVL *n = malloc(sizeof(NoAVL));
    if (!n) return NULL;
    n->reg    = reg;
    n->altura = 1;
    n->esq    = NULL;
    n->dir    = NULL;
    return n;
}

static NoAVL *rotacao_dir(NoAVL *y, MetricasArvore *m) {
    NoAVL *x  = y->esq;
    NoAVL *t2 = x->dir;
    x->dir = y;
    y->esq = t2;
    atualizar_altura(y);
    atualizar_altura(x);
    m->rotacoes++;
    return x;
}

static NoAVL *rotacao_esq(NoAVL *x, MetricasArvore *m) {
    NoAVL *y  = x->dir;
    NoAVL *t2 = y->esq;
    y->esq = x;
    x->dir = t2;
    atualizar_altura(x);
    atualizar_altura(y);
    m->rotacoes++;
    return y;
}

static NoAVL *rebalancear(NoAVL *n, MetricasArvore *m) {
    atualizar_altura(n);
    int fb = fator(n);

    if (fb > 1) {
        if (fator(n->esq) < 0)
            n->esq = rotacao_esq(n->esq, m);
        return rotacao_dir(n, m);
    }
    if (fb < -1) {
        if (fator(n->dir) > 0)
            n->dir = rotacao_dir(n->dir, m);
        return rotacao_esq(n, m);
    }
    return n;
}

static int inserir_rec(NoAVL **n, Registro reg, MetricasArvore *m) {
    if (!*n) {
        *n = novo_no(reg);
        return *n != NULL ? 1 : 0;
    }

    m->comparacoes++;
    int c = comparar_registro(&reg, &(*n)->reg);
    if (c < 0) {
        if (!inserir_rec(&(*n)->esq, reg, m))
            return 0;
    } else if (c > 0) {
        if (!inserir_rec(&(*n)->dir, reg, m))
            return 0;
    } else {
        return 2;
    }

    *n = rebalancear(*n, m);
    return 1;
}

static NoAVL *minimo(NoAVL *n) {
    while (n->esq) n = n->esq;
    return n;
}

static NoAVL *remover_rec(NoAVL *n, Registro reg, MetricasArvore *m, int *removido) {
    if (!n) return NULL;

    m->comparacoes++;
    int c = comparar_registro(&reg, &n->reg);
    if (c < 0) {
        n->esq = remover_rec(n->esq, reg, m, removido);
    } else if (c > 0) {
        n->dir = remover_rec(n->dir, reg, m, removido);
    } else {
        *removido = 1;
        if (!n->esq || !n->dir) {
            NoAVL *filho = n->esq ? n->esq : n->dir;
            free(n);
            return filho;
        }
        NoAVL *suc = minimo(n->dir);
        n->reg = suc->reg;
        n->dir = remover_rec(n->dir, suc->reg, m, removido);
    }
    return rebalancear(n, m);
}

static void inorder_rec(const NoAVL *n, void (*visita)(const Registro *)) {
    if (!n) return;
    inorder_rec(n->esq, visita);
    visita(&n->reg);
    inorder_rec(n->dir, visita);
}

static void destruir_rec(NoAVL *n) {
    if (!n) return;
    destruir_rec(n->esq);
    destruir_rec(n->dir);
    free(n);
}

static int contar_rec(const NoAVL *n) {
    if (!n) return 0;
    return 1 + contar_rec(n->esq) + contar_rec(n->dir);
}

/* --- API pública --- */

ArvoreAVL avl_criar(void) {
    ArvoreAVL a;
    a.raiz = NULL;
    a.metricas = (MetricasArvore){0, 0, 0, 0, 0};
    return a;
}

int avl_inserir(ArvoreAVL *a, Registro reg) {
    int status = inserir_rec(&a->raiz, reg, &a->metricas);
    if (status == 1)
        a->metricas.insercoes++;
    return status != 0;
}

NoAVL *avl_buscar(ArvoreAVL *a, const Registro *chave) {
    NoAVL *n = a->raiz;
    a->metricas.buscas++;

    while (n) {
        a->metricas.comparacoes++;
        int c = comparar_registro(chave, &n->reg);
        if (c < 0)      n = n->esq;
        else if (c > 0) n = n->dir;
        else            return n;
    }
    return NULL;
}

int avl_remover(ArvoreAVL *a, Registro reg) {
    int removido = 0;
    a->raiz = remover_rec(a->raiz, reg, &a->metricas, &removido);
    if (removido)
        a->metricas.remocoes++;
    return removido;
}

void avl_inorder(const ArvoreAVL *a, void (*visita)(const Registro *)) {
    inorder_rec(a->raiz, visita);
}

int avl_altura(const ArvoreAVL *a) {
    return altura_no(a->raiz);
}

int avl_contar(const ArvoreAVL *a) {
    return contar_rec(a->raiz);
}

void avl_destruir(ArvoreAVL *a) {
    destruir_rec(a->raiz);
    a->raiz = NULL;
}
