#ifndef GENERADOR_H_INCLUDED
#define GENERADOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANT_POSTEOS 50000
#define CANT_USUARIOS 200
#define POSTEOS_POR_FEED 20
#define TEXTO_MAX 141

#define RUTA_POSTEOS "Archivos/posteos.bin"
#define RUTA_CONCENTRADO "Archivos/lote_concentrado.bin"
#define RUTA_DISPERSO "Archivos/lote_disperso.bin"
#define RUTA_ORDENADO "Archivos/lote_ordenado.bin"

typedef struct {
    int id;                // Coincide con la posición relativa en el archivo (0 a 49.999)
    char texto[TEXTO_MAX]; // Menos de 140 caracteres
} tPosteo;

typedef struct {
    char usuario[15];
    int feed[POSTEOS_POR_FEED]; // 20 IDs de posteos
} tUsuario;

// Genera la base de datos de 50.000 posteos
int generar_archivo_posteos(const char *ruta, int cantidad);

// Genera los 3 lotes de prueba con la semilla provista
int generar_lotes_feeds(unsigned int semilla, int cantUsuarios);

#endif // GENERADOR_H_INCLUDED
