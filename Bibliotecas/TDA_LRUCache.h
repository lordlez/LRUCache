#ifndef TDA_LRUCACHE_H_INCLUDED
#define TDA_LRUCACHE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct sNodo {
    void *info;
    struct sNodo *sig;
    unsigned tamInfo;
} tNodo;

typedef tNodo *t_lru_cache;

typedef struct {
    t_lru_cache cache;
    int elem;
    int limite;
    int (*cmp)(const void*, const void*);
} LRUCache;

// Primitivas
void crear_lrucache(LRUCache *p, int limite, int (*cmp)(const void*, const void*));
int agregar_lrucache(LRUCache *p, const void *pd, unsigned tam);
int obtener_lrucache(LRUCache *p, void *pd, unsigned tam);
int borrar_lrucache(LRUCache *p, void *pd, unsigned tam);
void vaciar_lrucache(LRUCache *p);

#endif // TDA_LRUCACHE_H_INCLUDED
