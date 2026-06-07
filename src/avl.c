#include "avl.h"

#include <stdlib.h>
#include <string.h>

static int cmp_chave(const Registro *a, const Registro *b) {
    if (a->valor_venda < b->valor_venda) return -1;
    if (a->valor_venda > b->valor_venda) return  1;
    int r = strcmp(a->estado, b->estado);
    if (r != 0) return r;
    r = strcmp(a->municipio, b->municipio);
    if (r != 0) return r;
    r = strcmp(a->produto, b->produto);
    if (r != 0) return r;
    return strcmp(a->revenda, b->revenda);
}

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

static NoAVL *inserir_rec(NoAVL *n, Registro reg, MetricasArvore *m) {
    if (!n) return novo_no(reg);
    m->comparacoes++;
    int c = cmp_chave(&reg, &n->reg);
    if (c < 0)
        n->esq = inserir_rec(n->esq, reg, m);
    else if (c > 0)
        n->dir = inserir_rec(n->dir, reg, m);
    else
        return n; /* chave duplicada: ignora */
    return rebalancear(n, m);
}

static NoAVL *minimo(NoAVL *n) {
    while (n->esq) n = n->esq;
    return n;
}

static NoAVL *remover_rec(NoAVL *n, Registro reg, MetricasArvore *m) {
    if (!n) return NULL;
    m->comparacoes++;
    int c = cmp_chave(&reg, &n->reg);
    if (c < 0) {
        n->esq = remover_rec(n->esq, reg, m);
    } else if (c > 0) {
        n->dir = remover_rec(n->dir, reg, m);
    } else {
        if (!n->esq || !n->dir) {
            NoAVL *filho = n->esq ? n->esq : n->dir;
            free(n);
            return filho;
        }
        NoAVL *suc = minimo(n->dir);
        n->reg = suc->reg;
        n->dir = remover_rec(n->dir, suc->reg, m);
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

/* --- API pública --- */

ArvoreAVL avl_criar(void) {
    ArvoreAVL a;
    a.raiz = NULL;
    memset(&a.metricas, 0, sizeof(a.metricas));
    return a;
}

void avl_inserir(ArvoreAVL *a, Registro reg) {
    a->raiz = inserir_rec(a->raiz, reg, &a->metricas);
    a->metricas.insercoes++;
}

NoAVL *avl_buscar(ArvoreAVL *a, double chave) {
    NoAVL *n = a->raiz;
    a->metricas.buscas++;
    while (n) {
        a->metricas.comparacoes++;
        if (chave < n->reg.valor_venda)      n = n->esq;
        else if (chave > n->reg.valor_venda) n = n->dir;
        else                                 return n;
    }
    return NULL;
}

void avl_remover(ArvoreAVL *a, double chave) {
    Registro chave_reg;
    memset(&chave_reg, 0, sizeof(chave_reg));
    chave_reg.valor_venda = chave;
    a->raiz = remover_rec(a->raiz, chave_reg, &a->metricas);
    a->metricas.remocoes++;
}

void avl_inorder(const ArvoreAVL *a, void (*visita)(const Registro *)) {
    inorder_rec(a->raiz, visita);
}

int avl_altura(const ArvoreAVL *a) {
    return altura_no(a->raiz);
}

void avl_destruir(ArvoreAVL *a) {
    destruir_rec(a->raiz);
    a->raiz = NULL;
}
