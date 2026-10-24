#include "TDA_LRUCache_P2.h"

/**************************************************************
 * FUNCIONES AUXILIARES INTERNAS
 **************************************************************/

// Busca en el ABB. Devuelve la dirección del puntero al nodo (tArbol*)
static tArbol *buscar_en_arbol(tArbol *pa, const void *pd, int (*cmp)(const void*, const void*)){
    tNodoD *nodoLista;
    int rc;

    while(*pa){
        nodoLista = *(tNodoD**)(*pa)->info;
        rc = cmp(pd, nodoLista->info);
        if(rc == 0){
            return pa;
        }
        if(rc < 0){
            pa = &(*pa)->izq;
        }else{
            pa = &(*pa)->der;
        }
    }
    return pa;
}

static int calcular_altura(const tArbol *pa){
    int hizq, hder;
    if(!*pa){
        return 0;
    }
    hizq = calcular_altura(&(*pa)->izq);
    hder = calcular_altura(&(*pa)->der);
    return (hizq > hder ? hizq : hder) + 1;
}

// Mayor nodo (el que está más a la derecha)
static tNodoA **mayor_nodo(const tArbol *pa){
    if(!*pa){
        return NULL;
    }
    while((*pa)->der){
        pa = &(*pa)->der;
    }
    return (tNodoA**)pa;
}

// Menor nodo (el que está más a la izquierda)
static tNodoA **menor_nodo(const tArbol *pa){
    if(!*pa){
        return NULL;
    }
    while((*pa)->izq){
        pa = &(*pa)->izq;
    }
    return (tNodoA**)pa;
}

static int eliminar_raiz(tArbol *pa){
    tNodoA **remp;
    tNodoA *elim;

    if(!*pa){
        return 0;
    }

    free((*pa)->info);

    // Caso 1: Es una hoja (sin hijos)
    if(!(*pa)->izq && !(*pa)->der){
        free(*pa);
        *pa = NULL;
        return 1;
    }

    // Caso 2: Tiene hijos -> elije la rama más alta para mantener balance
    remp = calcular_altura(&(*pa)->izq) > calcular_altura(&(*pa)->der)
           ? mayor_nodo(&(*pa)->izq)
           : menor_nodo(&(*pa)->der);

    elim = *remp;
    (*pa)->info = elim->info;
    (*pa)->tamInfo = elim->tamInfo;
    *remp = elim->izq ? elim->izq : elim->der;
    free(elim);
    return 1;
}

// Saca un nodo de la lista doble en O(1)
static void desenlazar_de_lista(LRUCacheP2 *p, tNodoD *nodo){
    if(nodo->ant){
        nodo->ant->sig = nodo->sig;
    }else{
        p->primero = nodo->sig;
    }

    if(nodo->sig){
        nodo->sig->ant = nodo->ant;
    }else{
        p->ultimo = nodo->ant;
    }
}

// Engancha un nodo al principio de la lista doble en O(1)
static void enlazar_al_inicio(LRUCacheP2 *p, tNodoD *nodo){
    nodo->ant = NULL;
    nodo->sig = p->primero;

    if(p->primero){
        p->primero->ant = nodo;
    }else{
        p->ultimo = nodo;
    }
    p->primero = nodo;
}

// Vacía el árbol en postorden
static void vaciar_arbol(tArbol *pa){
    if(!*pa){
        return;
    }
    vaciar_arbol(&(*pa)->izq);
    vaciar_arbol(&(*pa)->der);
    free((*pa)->info);
    free(*pa);
    *pa = NULL;
}

/**************************************************************
 * PRIMITIVAS PARTE 2
 **************************************************************/

void crear_lrucache_p2(LRUCacheP2 *p, int limite, int (*cmp)(const void*, const void*)){
    p->primero = NULL;
    p->ultimo = NULL;
    p->arbol = NULL;
    p->elem = 0;
    p->limite = limite;
    p->cmp = cmp;
}

int agregar_lrucache_p2(LRUCacheP2 *p, const void *pd, unsigned tam){
    tArbol *pos;
    tNodoD *nodo;
    tNodoD *nuevo;
    tNodoA *nuevoA;
    tNodoD *viejo;

    // FASE 1: Búsqueda rápida en el Árbol en O(log N)
    pos = buscar_en_arbol(&p->arbol, pd, p->cmp);

    if(*pos){ // Ya existía en la caché (Hit)
        nodo = *(tNodoD**)(*pos)->info;
        memcpy(nodo->info, pd, MIN(tam, nodo->tamInfo));
        nodo->tamInfo = tam;

        // Si no era el primero, lo mudamos a la cabeza en O(1)
        if(nodo != p->primero){
            desenlazar_de_lista(p, nodo);
            enlazar_al_inicio(p, nodo);
        }
        return 1;
    }

    // FASE 2: No existía (Miss) -> Creamos nodo de lista y nodo de árbol
    if((nuevo = (tNodoD*)malloc(sizeof(tNodoD))) == NULL || (nuevo->info = malloc(tam)) == NULL){
        free(nuevo);
        return 0;
    }

    if((nuevoA = (tNodoA*)malloc(sizeof(tNodoA))) == NULL || (nuevoA->info = malloc(sizeof(tNodoD*))) == NULL){
        free(nuevoA);
        free(nuevo->info);
        free(nuevo);
        return 0;
    }

    // Cargamos la lista y lo ponemos al inicio en O(1)
    memcpy(nuevo->info, pd, tam);
    nuevo->tamInfo = tam;
    enlazar_al_inicio(p, nuevo);

    // Cargamos el árbol apuntando al nodo de la lista
    memcpy(nuevoA->info, &nuevo, sizeof(tNodoD*));
    nuevoA->tamInfo = sizeof(tNodoD*);
    nuevoA->izq = NULL;
    nuevoA->der = NULL;
    *pos = nuevoA; // Se inserta en el ABB

    p->elem++;

    // FASE 3: Si desborda, eliminamos el último en O(1)
    if(p->elem > p->limite){
        viejo = p->ultimo; // El menos usado está directamente en p->ultimo

        // Lo eliminamos del árbol
        pos = buscar_en_arbol(&p->arbol, viejo->info, p->cmp);
        if(*pos){
            eliminar_raiz(pos);
        }

        // Lo desenlazamos de la lista doble y liberamos memoria
        desenlazar_de_lista(p, viejo);
        free(viejo->info);
        free(viejo);
        p->elem--;
    }

    return 1;
}

int obtener_lrucache_p2(LRUCacheP2 *p, void *pd, unsigned tam){
    tArbol *pos;
    tNodoD *nodo;

    // 1. Buscamos en el árbol en O(log N)
    pos = buscar_en_arbol(&p->arbol, pd, p->cmp);
    if(!*pos){
        return 0; // Cache Miss
    }

    // 2. Tomamos el nodo de la lista y copiamos el dato
    nodo = *(tNodoD**)(*pos)->info;
    memcpy(pd, nodo->info, MIN(tam, nodo->tamInfo));

    // 3. Lo movemos al frente de la lista en O(1)
    if(nodo != p->primero){
        desenlazar_de_lista(p, nodo);
        enlazar_al_inicio(p, nodo);
    }

    return 1; // Cache Hit
}

int borrar_lrucache_p2(LRUCacheP2 *p, void *pd, unsigned tam){
    tArbol *pos;
    tNodoD *nodo;

    pos = buscar_en_arbol(&p->arbol, pd, p->cmp);
    if(!*pos){
        return 0;
    }

    nodo = *(tNodoD**)(*pos)->info;

    // Borramos del árbol
    eliminar_raiz(pos);

    // Desconectamos de la lista doble en O(1)
    desenlazar_de_lista(p, nodo);

    if(pd){
        memcpy(pd, nodo->info, MIN(tam, nodo->tamInfo));
    }
    free(nodo->info);
    free(nodo);
    p->elem--;

    return 1;
}

void vaciar_lrucache_p2(LRUCacheP2 *p){
    tNodoD *act = p->primero;
    tNodoD *aux;

    // 1. Vaciamos todos los nodos del árbol
    vaciar_arbol(&p->arbol);

    // 2. Vaciamos la lista doble y liberamos los datos reales
    while(act){
        aux = act->sig;
        free(act->info);
        free(act);
        act = aux;
    }

    p->primero = NULL;
    p->ultimo = NULL;
    p->elem = 0;
}

int altura_arbol_lrucache_p2(const LRUCacheP2 *p){
    return calcular_altura(&p->arbol);
}
