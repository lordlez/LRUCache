#include "Tweener.h"

int comparar_posteos_id(const void *a, const void *b){
    const tPosteo *p1 = (const tPosteo*)a;
    const tPosteo *p2 = (const tPosteo*)b;
    return p1->id - p2->id;
}

// 1. SIMULACIÓN SIN CACHÉ
int simular_sin_cache(const char *rutaUsuarios, const char *rutaPosteos, tMetricas *met){
    FILE *archUsr;
    FILE *archPost;
    tUsuario u;
    tPosteo p;
    clock_t tIni, tFin;
    int cantUsuarios = 0;
    int totalPosteosBuscados = 0;
    int j;

    archUsr = fopen(rutaUsuarios, "rb");
    if(!archUsr) return 0;

    archPost = fopen(rutaPosteos, "rb");
    if(!archPost){
        fclose(archUsr);
        return 0;
    }

    met->hits = 0;
    met->misses = 0;
    met->lecturasDisco = 0;
    met->alturaArbol = -1;

    tIni = clock();

    while(fread(&u, sizeof(tUsuario), 1, archUsr) == 1){
        cantUsuarios++;
        for(j = 0; j < POSTEOS_POR_FEED; j++){
            totalPosteosBuscados++;
            // Búsqueda directa en disco
            fseek(archPost, (long)u.feed[j] * sizeof(tPosteo), SEEK_SET);
            fread(&p, sizeof(tPosteo), 1, archPost);
            met->lecturasDisco++;
            met->misses++; // Sin caché, toda petición va a disco
        }
    }

    tFin = clock();

    fclose(archUsr);
    fclose(archPost);

    met->tiempoTotalSeg = (double)(tFin - tIni) / CLOCKS_PER_SEC;
    met->tiempoPromUsuarioSeg = cantUsuarios > 0 ? met->tiempoTotalSeg / cantUsuarios : 0;
    met->tiempoPromPosteoSeg = totalPosteosBuscados > 0 ? met->tiempoTotalSeg / totalPosteosBuscados : 0;

    return 1;
}

// 2. SIMULACIÓN CON CACHÉ PARTE 1 (Lista Simple)
int simular_con_cache_p1(const char *rutaUsuarios, const char *rutaPosteos, int capacidad, tMetricas *met){
    FILE *archUsr;
    FILE *archPost;
    tUsuario u;
    tPosteo pBuscado;
    tPosteo pLeido;
    LRUCache cache;
    clock_t tIni, tFin;
    int cantUsuarios = 0;
    int totalPosteosBuscados = 0;
    int j;

    archUsr = fopen(rutaUsuarios, "rb");
    if(!archUsr) return 0;

    archPost = fopen(rutaPosteos, "rb");
    if(!archPost){
        fclose(archUsr);
        return 0;
    }

    crear_lrucache(&cache, capacidad, comparar_posteos_id);

    met->hits = 0;
    met->misses = 0;
    met->lecturasDisco = 0;
    met->alturaArbol = -1;

    tIni = clock();

    while(fread(&u, sizeof(tUsuario), 1, archUsr) == 1){
        cantUsuarios++;
        for(j = 0; j < POSTEOS_POR_FEED; j++){
            totalPosteosBuscados++;
            pBuscado.id = u.feed[j];

            // 1. Trata de obtenerlo de la caché
            if(obtener_lrucache(&cache, &pBuscado, sizeof(tPosteo))){
                met->hits++; // Encontrado en RAM
            }else{
                met->misses++; // Fallo de caché -> ir al disco
                fseek(archPost, (long)u.feed[j] * sizeof(tPosteo), SEEK_SET);
                fread(&pLeido, sizeof(tPosteo), 1, archPost);
                met->lecturasDisco++;
                // Lo agregamos a la caché
                agregar_lrucache(&cache, &pLeido, sizeof(tPosteo));
            }
        }
    }

    tFin = clock();

    vaciar_lrucache(&cache);
    fclose(archUsr);
    fclose(archPost);

    met->tiempoTotalSeg = (double)(tFin - tIni) / CLOCKS_PER_SEC;
    met->tiempoPromUsuarioSeg = cantUsuarios > 0 ? met->tiempoTotalSeg / cantUsuarios : 0;
    met->tiempoPromPosteoSeg = totalPosteosBuscados > 0 ? met->tiempoTotalSeg / totalPosteosBuscados : 0;

    return 1;
}

// 3. SIMULACIÓN CON CACHÉ PARTE 2 (Lista Doble + Árbol ABB)
int simular_con_cache_p2(const char *rutaUsuarios, const char *rutaPosteos, int capacidad, tMetricas *met){
    FILE *archUsr;
    FILE *archPost;
    tUsuario u;
    tPosteo pBuscado;
    tPosteo pLeido;
    LRUCacheP2 cache;
    clock_t tIni, tFin;
    int cantUsuarios = 0;
    int totalPosteosBuscados = 0;
    int j;

    archUsr = fopen(rutaUsuarios, "rb");
    if(!archUsr) return 0;

    archPost = fopen(rutaPosteos, "rb");
    if(!archPost){
        fclose(archUsr);
        return 0;
    }

    crear_lrucache_p2(&cache, capacidad, comparar_posteos_id);

    met->hits = 0;
    met->misses = 0;
    met->lecturasDisco = 0;

    tIni = clock();

    while(fread(&u, sizeof(tUsuario), 1, archUsr) == 1){
        cantUsuarios++;
        for(j = 0; j < POSTEOS_POR_FEED; j++){
            totalPosteosBuscados++;
            pBuscado.id = u.feed[j];

            // 1. Busca en el árbol índice O(log N) y mueve en O(1)
            if(obtener_lrucache_p2(&cache, &pBuscado, sizeof(tPosteo))){
                met->hits++;
            }else{
                met->misses++;
                fseek(archPost, (long)u.feed[j] * sizeof(tPosteo), SEEK_SET);
                fread(&pLeido, sizeof(tPosteo), 1, archPost);
                met->lecturasDisco++;
                agregar_lrucache_p2(&cache, &pLeido, sizeof(tPosteo));
            }
        }
    }

    tFin = clock();

    met->alturaArbol = altura_arbol_lrucache_p2(&cache);

    vaciar_lrucache_p2(&cache);
    fclose(archUsr);
    fclose(archPost);

    met->tiempoTotalSeg = (double)(tFin - tIni) / CLOCKS_PER_SEC;
    met->tiempoPromUsuarioSeg = cantUsuarios > 0 ? met->tiempoTotalSeg / cantUsuarios : 0;
    met->tiempoPromPosteoSeg = totalPosteosBuscados > 0 ? met->tiempoTotalSeg / totalPosteosBuscados : 0;

    return 1;
}

// 4. TABLA COMPARATIVA
void ejecutar_comparativa(const char *nombreLote, const char *rutaLote, int capacidad){
    tMetricas mSin, mP1, mP2;

    printf("\n========================================================================================\n");
    printf(" LOTE: %s | CAPACIDAD DE CACHE: %d\n", nombreLote, capacidad);
    printf("========================================================================================\n");

    printf("Ejecutando Sin Cache...\n");
    simular_sin_cache(rutaLote, RUTA_POSTEOS, &mSin);

    printf("Ejecutando LRU Parte 1 (Lista Simple)...\n");
    simular_con_cache_p1(rutaLote, RUTA_POSTEOS, capacidad, &mP1);

    printf("Ejecutando LRU Parte 2 (Lista Doble + Arbol)...\n");
    simular_con_cache_p2(rutaLote, RUTA_POSTEOS, capacidad, &mP2);

    printf("\n----------------------------------------------------------------------------------------\n");
    printf("%-20s %-12s %-12s %-8s %-8s %-8s %-6s\n",
           "MODALIDAD", "TIEMPO TOTAL", "PROM/POSTEO", "HITS", "MISSES", "DISCO", "ALTURA");
    printf("----------------------------------------------------------------------------------------\n");

    printf("%-20s %10.6f s %10.6f s %8d %8d %8d %6s\n",
           "1. Sin Cache", mSin.tiempoTotalSeg, mSin.tiempoPromPosteoSeg,
           mSin.hits, mSin.misses, mSin.lecturasDisco, "-");

    printf("%-20s %10.6f s %10.6f s %8d %8d %8d %6s\n",
           "2. LRU Parte 1", mP1.tiempoTotalSeg, mP1.tiempoPromPosteoSeg,
           mP1.hits, mP1.misses, mP1.lecturasDisco, "-");

    printf("%-20s %10.6f s %10.6f s %8d %8d %8d %6d\n",
           "3. LRU Parte 2", mP2.tiempoTotalSeg, mP2.tiempoPromPosteoSeg,
           mP2.hits, mP2.misses, mP2.lecturasDisco, mP2.alturaArbol);

    printf("========================================================================================\n\n");
}
