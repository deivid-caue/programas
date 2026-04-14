#include "arvoreB.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct item_fila {
  nodo *ptr_nodo;
  int32_t nivel;
  struct item_fila *prox;
} item_fila;

typedef struct fila {
  item_fila *ini;
  item_fila *fim;
} fila;

static void *malloc_check(size_t tam) {
  void *ptr = malloc(tam);
  if (ptr == NULL) {
    fprintf(stderr, "Falha ao alocar memoria.\n");
    exit(1);
  }
  return ptr;
}

static fila *criar_fila(void) {
  fila *f = malloc_check(sizeof(fila));
  f->ini = NULL;
  f->fim = NULL;
  return f;
}

static bool fila_vazia(const fila *f) {
  return f->ini == NULL;
}

static void enfileirar(fila *f, nodo *ptr_nodo, int32_t nivel) {
  item_fila *novo = malloc_check(sizeof(item_fila));
  novo->ptr_nodo = ptr_nodo;
  novo->nivel = nivel;
  novo->prox = NULL;

  if (f->fim == NULL) {
    f->ini = novo;
    f->fim = novo;
    return;
  }

  f->fim->prox = novo;
  f->fim = novo;
}

static void desenfileirar(fila *f, nodo **ptr_nodo, int32_t *nivel) {
  item_fila *rem = f->ini;

  *ptr_nodo = rem->ptr_nodo;
  *nivel = rem->nivel;

  f->ini = rem->prox;
  if (f->ini == NULL) {
    f->fim = NULL;
  }

  free(rem);
}

static void destruir_fila(fila *f) {
  while (!fila_vazia(f)) {
    nodo *ptr_nodo = NULL;
    int32_t nivel = 0;
    desenfileirar(f, &ptr_nodo, &nivel);
  }
  free(f);
}

static nodo *alocar_nodo(int32_t t_arvore) {
  nodo *novo = malloc_check(sizeof(nodo));

  novo->chaves = malloc_check((size_t)(2 * t_arvore - 1) * sizeof(int32_t));
  novo->filhos = malloc_check((size_t)(2 * t_arvore) * sizeof(nodo *));
  novo->n = 0;
  novo->ehfolha = true;

  for (int32_t i = 0; i < 2 * t_arvore; ++i) {
    novo->filhos[i] = NULL;
  }

  return novo;
}

static void destruir_nodos(nodo *x) {
  if (x == NULL) {
    return;
  }

  if (!x->ehfolha) {
    for (int32_t i = 0; i <= x->n; ++i) {
      destruir_nodos(x->filhos[i]);
    }
  }

  free(x->chaves);
  free(x->filhos);
  free(x);
}

static nodo *buscar_nodo(nodo *x, int32_t chave, int32_t *idx_encontrado) {
  int32_t i = 0;

  while (i < x->n && chave > x->chaves[i]) {
    ++i;
  }

  if (i < x->n && chave == x->chaves[i]) {
    *idx_encontrado = i;
    return x;
  }

  if (x->ehfolha) {
    *idx_encontrado = -1;
    return NULL;
  }

  return buscar_nodo(x->filhos[i], chave, idx_encontrado);
}

static void dividir_filho(nodo *x, int32_t i, int32_t t_arvore) {
  nodo *y = x->filhos[i];
  nodo *z = alocar_nodo(t_arvore);

  z->ehfolha = y->ehfolha;
  z->n = t_arvore - 1;

  for (int32_t j = 0; j < t_arvore - 1; ++j) {
    z->chaves[j] = y->chaves[j + t_arvore];
  }

  if (!y->ehfolha) {
    for (int32_t j = 0; j < t_arvore; ++j) {
      z->filhos[j] = y->filhos[j + t_arvore];
      y->filhos[j + t_arvore] = NULL;
    }
  }

  y->n = t_arvore - 1;

  for (int32_t j = x->n; j >= i + 1; --j) {
    x->filhos[j + 1] = x->filhos[j];
  }

  x->filhos[i + 1] = z;

  for (int32_t j = x->n - 1; j >= i; --j) {
    x->chaves[j + 1] = x->chaves[j];
  }

  x->chaves[i] = y->chaves[t_arvore - 1];
  x->n += 1;
}

static void inserir_nao_cheio(nodo *x, int32_t chave, int32_t t_arvore) {
  int32_t i = x->n - 1;

  if (x->ehfolha) {
    while (i >= 0 && chave < x->chaves[i]) {
      x->chaves[i + 1] = x->chaves[i];
      --i;
    }

    x->chaves[i + 1] = chave;
    x->n += 1;
    return;
  }

  while (i >= 0 && chave < x->chaves[i]) {
    --i;
  }

  ++i;

  if (x->filhos[i]->n == 2 * t_arvore - 1) {
    dividir_filho(x, i, t_arvore);

    if (chave > x->chaves[i]) {
      ++i;
    }
  }

  inserir_nao_cheio(x->filhos[i], chave, t_arvore);
}

static void imprimir_em_ordem(nodo *x, bool *primeiro) {
  if (x == NULL) {
    return;
  }

  for (int32_t i = 0; i < x->n; ++i) {
    if (!x->ehfolha) {
      imprimir_em_ordem(x->filhos[i], primeiro);
    }

    if (!*primeiro) {
      printf(" ");
    }
    printf("%" PRId32, x->chaves[i]);
    *primeiro = false;
  }

  if (!x->ehfolha) {
    imprimir_em_ordem(x->filhos[x->n], primeiro);
  }
}



static void imprimir_nodo(const nodo *x) {
  printf("%c (n:%" PRId32 ") [", x->ehfolha ? 'F' : 'I', x->n);

  for (int32_t i = 0; i < x->n; ++i) {
    if (i > 0) {
      printf(" ");
    }
    printf("%" PRId32, x->chaves[i]);
  }

  printf("]");
}

struct arvoreB *criarArvoreB(int32_t t_arvore) {
  struct arvoreB *arvore = malloc_check(sizeof(struct arvoreB));
  arvore->raiz = NULL;
  arvore->t_arvore = t_arvore;
  return arvore;
}

void inserirArvoreB(struct arvoreB *arvore, int32_t chave) {
  nodo *r;
  nodo *s;
  int32_t i;

  if (arvore == NULL) {
    return;
  }

  if (arvore->raiz == NULL) {
    arvore->raiz = alocar_nodo(arvore->t_arvore);
    arvore->raiz->chaves[0] = chave;
    arvore->raiz->n = 1;
    return;
  }

  r = arvore->raiz;

  if (r->n == 2 * arvore->t_arvore - 1) {
    s = alocar_nodo(arvore->t_arvore);
    s->ehfolha = false;
    s->n = 0;
    s->filhos[0] = r;
    arvore->raiz = s;

    dividir_filho(s, 0, arvore->t_arvore);

    i = 0;
    if (chave > s->chaves[0]) {
      i = 1;
    }

    inserir_nao_cheio(s->filhos[i], chave, arvore->t_arvore);
    return;
  }

  inserir_nao_cheio(r, chave, arvore->t_arvore);
}

void imprimirArvoreB(struct arvoreB *arvore) {
  fila *f;
  int32_t nivel_atual = -1;
  bool primeiro_no_nivel = true;

  if (arvore == NULL || arvore->raiz == NULL) {
    return;
  }

  f = criar_fila();
  enfileirar(f, arvore->raiz, 0);

  while (!fila_vazia(f)) {
    nodo *atual = NULL;
    int32_t nivel = 0;

    desenfileirar(f, &atual, &nivel);

    if (nivel != nivel_atual) {
      if (nivel_atual != -1) {
        printf("\n");
      }

      printf("----//----\n");
      printf("Nivel %" PRId32 "\n", nivel);
      printf("----//----\n");

      nivel_atual = nivel;
      primeiro_no_nivel = true;
    }

    if (!primeiro_no_nivel) {
      printf(" ");
    }

    imprimir_nodo(atual);
    primeiro_no_nivel = false;

    if (!atual->ehfolha) {
      for (int32_t i = 0; i <= atual->n; ++i) {
        enfileirar(f, atual->filhos[i], nivel + 1);
      }
    }
  }

  printf("\n");
  destruir_fila(f);
}

void imprimirEmOrdem(struct arvoreB *arvore) {
  bool primeiro = true;

  printf("Em ordem:");
  if (arvore != NULL && arvore->raiz != NULL) {
    printf(" ");
    imprimir_em_ordem(arvore->raiz, &primeiro);
  }
  printf("\n");
}

nodo *buscarArvoreB(struct arvoreB *arvore, int32_t chave,
                    int32_t *idxEncontrado) {
  if (idxEncontrado == NULL) {
    return NULL;
  }

  if (arvore == NULL || arvore->raiz == NULL) {
    *idxEncontrado = -1;
    return NULL;
  }

  return buscar_nodo(arvore->raiz, chave, idxEncontrado);
}

void deletarArvore(struct arvoreB *arvore) {
  if (arvore == NULL) {
    return;
  }

  destruir_nodos(arvore->raiz);
  free(arvore);
}
