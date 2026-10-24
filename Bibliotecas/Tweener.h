#ifndef TWEENER_H_INCLUDED
#define TWEENER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "TDA_LRUCache.h"
#include "TDA_LRUCache_P2.h"
#include "Generador.h"

// Estructura para almacenar las métricas de cada corrida
typedef struct {
    double tiempoTotalSeg;
    double tiempoPromUsuarioSeg;
    double tiempoPromPosteoSeg;
    int hits;
    int misses;
    int lecturasDisco;
    int alturaArbol; // Solo para Parte 2 (-1 en los otros)
} tMetricas;


int comparar_posteos_id(const void *a, const void *b);

// Funciones de simulación para cada modalidad
int simular_sin_cache(const char *rutaUsuarios, const char *rutaPosteos, tMetricas *met);
int simular_con_cache_p1(const char *rutaUsuarios, const char *rutaPosteos, int capacidad, tMetricas *met);
int simular_con_cache_p2(const char *rutaUsuarios, const char *rutaPosteos, int capacidad, tMetricas *met);

// Ejecuta las 3 modalidades juntas e imprime la tabla comparativa
void ejecutar_comparativa(const char *nombreLote, const char *rutaLote, int capacidad);

#endif // TWEENER_H_INCLUDED
