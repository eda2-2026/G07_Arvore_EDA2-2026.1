#include "rubro_negra.h"

#include <stdlib.h>
#include <string.h>

#include "chave.h"

static void rotacao_esq(ArvoreRN *a, NoRN *x) {
    NoRN *y  = x->dir;
    x->dir   = y->esq;
    if (y->esq != a->nil) y->esq->pai = x;
    y->pai = x->pai;
    if (x->pai == a->nil)          a->raiz  = y;
    else if (x == x->pai->esq)     x->pai->esq = y;
    else                           x->pai->dir = y;
    y->esq   = x;
    x->pai   = y;
    a->metricas.rotacoes++;
}

static void rotacao_dir(ArvoreRN *a, NoRN *y) {
    NoRN *x  = y->esq;
    y->esq   = x->dir;
    if (x->dir != a->nil) x->dir->pai = y;
    x->pai = y->pai;
    if (y->pai == a->nil)          a->raiz  = x;
    else if (y == y->pai->dir)     y->pai->dir = x;
    else                           y->pai->esq = x;
    x->dir   = y;
    y->pai   = x;
    a->metricas.rotacoes++;
}

static void fixup_insercao(ArvoreRN *a, NoRN *z) {
    while (z->pai->cor == VERMELHO) {
        if (z->pai == z->pai->pai->esq) {
            NoRN *tio = z->pai->pai->dir;
            if (tio->cor == VERMELHO) {
                z->pai->cor        = PRETO;
                tio->cor           = PRETO;
                z->pai->pai->cor   = VERMELHO;
                z = z->pai->pai;
            } else {
                if (z == z->pai->dir) {
                    z = z->pai;
                    rotacao_esq(a, z);
                }
                z->pai->cor      = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacao_dir(a, z->pai->pai);
            }
        } else {
            NoRN *tio = z->pai->pai->esq;
            if (tio->cor == VERMELHO) {
                z->pai->cor        = PRETO;
                tio->cor           = PRETO;
                z->pai->pai->cor   = VERMELHO;
                z = z->pai->pai;
            } else {
                if (z == z->pai->esq) {
                    z = z->pai;
                    rotacao_dir(a, z);
                }
                z->pai->cor      = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacao_esq(a, z->pai->pai);
            }
        }
    }
    a->raiz->cor = PRETO;
}

static void transplant(ArvoreRN *a, NoRN *u, NoRN *v) {
    if (u->pai == a->nil)        a->raiz     = v;
    else if (u == u->pai->esq)   u->pai->esq = v;
    else                         u->pai->dir = v;
    v->pai = u->pai;
}

static NoRN *minimo(ArvoreRN *a, NoRN *n) {
    while (n->esq != a->nil) n = n->esq;
    return n;
}

static void fixup_remocao(ArvoreRN *a, NoRN *x) {
    while (x != a->raiz && x->cor == PRETO) {
        if (x == x->pai->esq) {
            NoRN *w = x->pai->dir;
            if (w->cor == VERMELHO) {
                w->cor       = PRETO;
                x->pai->cor  = VERMELHO;
                rotacao_esq(a, x->pai);
                w = x->pai->dir;
            }
            if (w->esq->cor == PRETO && w->dir->cor == PRETO) {
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (w->dir->cor == PRETO) {
                    w->esq->cor = PRETO;
                    w->cor      = VERMELHO;
                    rotacao_dir(a, w);
                    w = x->pai->dir;
                }
                w->cor        = x->pai->cor;
                x->pai->cor   = PRETO;
                w->dir->cor   = PRETO;
                rotacao_esq(a, x->pai);
                x = a->raiz;
            }
        } else {
            NoRN *w = x->pai->esq;
            if (w->cor == VERMELHO) {
                w->cor       = PRETO;
                x->pai->cor  = VERMELHO;
                rotacao_dir(a, x->pai);
                w = x->pai->esq;
            }
            if (w->dir->cor == PRETO && w->esq->cor == PRETO) {
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (w->esq->cor == PRETO) {
                    w->dir->cor = PRETO;
                    w->cor      = VERMELHO;
                    rotacao_esq(a, w);
                    w = x->pai->esq;
                }
                w->cor        = x->pai->cor;
                x->pai->cor   = PRETO;
                w->esq->cor   = PRETO;
                rotacao_dir(a, x->pai);
                x = a->raiz;
            }
        }
    }
    x->cor = PRETO;
}

static void inorder_rec(const ArvoreRN *a, const NoRN *n,
                        void (*visita)(const Registro *)) {
    if (n == a->nil) return;
    inorder_rec(a, n->esq, visita);
    visita(&n->reg);
    inorder_rec(a, n->dir, visita);
}

static int altura_rec(const ArvoreRN *a, const NoRN *n) {
    if (n == a->nil) return 0;
    int he = altura_rec(a, n->esq);
    int hd = altura_rec(a, n->dir);
    return 1 + (he > hd ? he : hd);
}

static int contar_rec(const ArvoreRN *a, const NoRN *n) {
    if (n == a->nil) return 0;
    return 1 + contar_rec(a, n->esq) + contar_rec(a, n->dir);
}

static void destruir_rec(ArvoreRN *a, NoRN *n) {
    if (n == a->nil) return;
    destruir_rec(a, n->esq);
    destruir_rec(a, n->dir);
    free(n);
}

/* --- API pública --- */

ArvoreRN rn_criar(void) {
    ArvoreRN a;
    a.nil = malloc(sizeof(NoRN));
    if (!a.nil) {
        a.raiz = NULL;
        a.metricas = (MetricasArvore){0, 0, 0, 0, 0};
        return a;
    }
    a.nil->cor = PRETO;
    a.nil->esq = a.nil->dir = a.nil->pai = a.nil;
    memset(&a.nil->reg, 0, sizeof(a.nil->reg));
    a.raiz = a.nil;
    a.metricas = (MetricasArvore){0, 0, 0, 0, 0};
    return a;
}

int rn_inserir(ArvoreRN *a, Registro reg) {
    if (!a->nil) return 0;

    NoRN *z = malloc(sizeof(NoRN));
    if (!z) return 0;

    z->reg = reg;
    z->cor = VERMELHO;
    z->esq = z->dir = z->pai = a->nil;

    NoRN *y = a->nil;
    NoRN *x = a->raiz;
    while (x != a->nil) {
        y = x;
        a->metricas.comparacoes++;
        int c = comparar_registro(&z->reg, &x->reg);
        if (c < 0)       x = x->esq;
        else if (c > 0)  x = x->dir;
        else {
            free(z);
            return 2;
        }
    }

    z->pai = y;
    if (y == a->nil)         a->raiz  = z;
    else if (comparar_registro(&z->reg, &y->reg) < 0) y->esq = z;
    else                     y->dir   = z;

    fixup_insercao(a, z);
    a->metricas.insercoes++;
    return 1;
}

NoRN *rn_buscar(ArvoreRN *a, const Registro *chave) {
    if (!a->nil) return NULL;

    NoRN *n = a->raiz;
    a->metricas.buscas++;

    while (n != a->nil) {
        a->metricas.comparacoes++;
        int c = comparar_registro(chave, &n->reg);
        if (c < 0)      n = n->esq;
        else if (c > 0) n = n->dir;
        else            return n;
    }
    return NULL;
}

int rn_remover(ArvoreRN *a, Registro reg) {
    if (!a->nil) return 0;

    NoRN *z = a->raiz;
    while (z != a->nil) {
        a->metricas.comparacoes++;
        int c = comparar_registro(&reg, &z->reg);
        if (c < 0)      z = z->esq;
        else if (c > 0) z = z->dir;
        else break;
    }
    if (z == a->nil) return 0;

    NoRN *y = z;
    CorNo y_cor_orig = y->cor;
    NoRN *x;

    if (z->esq == a->nil) {
        x = z->dir;
        transplant(a, z, z->dir);
    } else if (z->dir == a->nil) {
        x = z->esq;
        transplant(a, z, z->esq);
    } else {
        y = minimo(a, z->dir);
        y_cor_orig = y->cor;
        x = y->dir;
        if (y->pai == z) {
            x->pai = y;
        } else {
            transplant(a, y, y->dir);
            y->dir = z->dir;
            y->dir->pai = y;
        }
        transplant(a, z, y);
        y->esq = z->esq;
        y->esq->pai = y;
        y->cor = z->cor;
    }
    free(z);
    if (y_cor_orig == PRETO) fixup_remocao(a, x);
    a->metricas.remocoes++;
    return 1;
}

void rn_inorder(const ArvoreRN *a, void (*visita)(const Registro *)) {
    inorder_rec(a, a->raiz, visita);
}

int rn_altura(const ArvoreRN *a) {
    if (!a->nil) return 0;
    return altura_rec(a, a->raiz);
}

int rn_contar(const ArvoreRN *a) {
    if (!a->nil) return 0;
    return contar_rec(a, a->raiz);
}

void rn_destruir(ArvoreRN *a) {
    if (!a->nil) return;
    destruir_rec(a, a->raiz);
    free(a->nil);
    a->raiz = NULL;
    a->nil  = NULL;
}
