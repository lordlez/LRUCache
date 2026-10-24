#ifndef TDA_LRUCACHE_P2_H_INCLUDED
#define TDA_LRUCACHE_P2_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

typedef struct sNodoD {
    void *info;
    struct sNodoD *sig, *ant;
    unsigned tamInfo;
} tNodoD;

typedef struct sNodoA {
    void *info;
    struct sNodoA *izq, *der;
    unsigned tamInfo;
} tNodoA;

typedef tNodoA *tArbol;


typedef struct {
    tNodoD *primero;           // el mas recientemente usado
    tNodoD *ultimo;            // el menos recientemente usado
    tArbol arbol;              // indice por clave
    int elem;
    int limite;
    int (*cmp)(const void*, const void*);
} LRUCacheP2;

void crear_lrucache_p2(LRUCacheP2 *p, int limite, int (*cmp)(const void*, const void*));
int agregar_lrucache_p2(LRUCacheP2 *p, const void *pd, unsigned tam);
int obtener_lrucache_p2(LRUCacheP2 *p, void *pd, unsigned tam);
int borrar_lrucache_p2(LRUCacheP2 *p, void *pd, unsigned tam);
void vaciar_lrucache_p2(LRUCacheP2 *p);
int altura_arbol_lrucache_p2(const LRUCacheP2 *p);

#endif // TDA_LRUCACHE_P2_H_INCLUDED
