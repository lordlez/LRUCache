#include "TDA_LRUCache.h"

void crear_lrucache(LRUCache *p, int limite, int (*cmp)(const void*, const void*)){
    p->cache = NULL;
    p->elem = 0;
    p->limite = limite;
    p->cmp = cmp;
}

int agregar_lrucache(LRUCache *p, const void *pd, unsigned tam){
    tNodo *nuevo;
    tNodo *ant = NULL;
    tNodo *act = p->cache;
    tNodo **pl;
    tNodo *elim;

    // FASE 1: Buscar si el elemento ya existía usando p->cmp
    while(act && p->cmp(act->info, pd) != 0){
        ant = act;
        act = act->sig;
    }

    if(act){ // Ya existía: lo movemos al frente y actualizamos su dato
        if(ant){ // Si no era el primero, lo mandamos a la cabeza
            ant->sig = act->sig;
            act->sig = p->cache;
            p->cache = act;
        }
        memcpy(act->info, pd, MIN(tam, act->tamInfo));
        act->tamInfo = tam;
        return 1;
    }

    // FASE 2: Si no existía, reservamos memoria en una sola línea
    if((nuevo = (tNodo*)malloc(sizeof(tNodo))) == NULL || (nuevo->info = malloc(tam)) == NULL){
        free(nuevo);
        return 0;
    }

    memcpy(nuevo->info, pd, tam);
    nuevo->tamInfo = tam;
    nuevo->sig = p->cache;
    p->cache = nuevo; // Queda como el más reciente en la cabeza
    p->elem++;

    // FASE 3: Si desborda la capacidad, eliminamos el último con tNodo **
    if(p->elem > p->limite){
        pl = &p->cache;
        while((*pl)->sig){
            pl = &(*pl)->sig;
        }
        elim = *pl;
        *pl = NULL; // Desenlaza el último nodo
        free(elim->info);
        free(elim);
        p->elem--;
    }

    return 1;
}

int obtener_lrucache(LRUCache *p, void *pd, unsigned tam){
    tNodo *ant = NULL;
    tNodo *act = p->cache;

    // 1. Buscamos el nodo en la lista usando p->cmp
    while(act && p->cmp(act->info, pd) != 0){
        ant = act;
        act = act->sig;
    }

    if(!act){
        return 0; // Cache Miss: no existe
    }

    // 2. Copiamos la información hacia el parámetro pd
    memcpy(pd, act->info, MIN(tam, act->tamInfo));

    // 3. LRU: si no era el primero, lo movemos a la cabeza
    if(ant){
        ant->sig = act->sig;
        act->sig = p->cache;
        p->cache = act;
    }

    return 1; // Cache Hit
}

int borrar_lrucache(LRUCache *p, void *pd, unsigned tam){
    tNodo **pl = &p->cache;
    tNodo *elim;

    // Buscamos con puntero doble usando p->cmp
    while(*pl && p->cmp(pd, (*pl)->info) != 0){
        pl = &(*pl)->sig;
    }

    if(!*pl){
        return 0; // No se encontró
    }

    elim = *pl;
    *pl = elim->sig; // Desenlaza directamente

    if(pd){
        memcpy(pd, elim->info, MIN(tam, elim->tamInfo));
    }

    free(elim->info);
    free(elim);
    p->elem--;

    return 1;
}

void vaciar_lrucache(LRUCache *p){
    tNodo *elim;
    while(p->cache){
        elim = p->cache;
        p->cache = elim->sig;
        free(elim->info);
        free(elim);
    }
    p->elem = 0;
}
