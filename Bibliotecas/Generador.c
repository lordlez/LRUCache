#include "Generador.h"

// Función auxiliar para qsort en el lote ordenado
static int comparar_enteros(const void *a, const void *b){
    return (*(const int*)a - *(const int*)b);
}

int generar_archivo_posteos(const char *ruta, int cantidad){
    FILE *arch;
    tPosteo post;
    int i;

    arch = fopen(ruta, "wb");
    if(!arch){
        printf("Error al crear el archivo de posteos: %s\n", ruta);
        return 0;
    }

    printf("Generando %d posteos en %s...\n", cantidad, ruta);
    for(i = 0; i < cantidad; i++){
        post.id = i;
        snprintf(post.texto, sizeof(post.texto), "Posteo #%d - Texto de prueba simulado para la red social Tweener.", i);
        fwrite(&post, sizeof(tPosteo), 1, arch);
    }

    fclose(arch);
    printf("-> Archivo de posteos generado con exito.\n");
    return 1;
}

int generar_lotes_feeds(unsigned int semilla, int cantUsuarios){
    FILE *archConc;
    FILE *archDisp;
    FILE *archOrd;
    tUsuario u;
    int i, j;

    // 1. LOTE CONCENTRADO (todos ven posteos dentro de los primeros 100)
    srand(semilla);
    archConc = fopen(RUTA_CONCENTRADO, "wb");
    if(!archConc){
        printf("Error al crear %s\n", RUTA_CONCENTRADO);
        return 0;
    }

    for(i = 0; i < cantUsuarios; i++){
        snprintf(u.usuario, sizeof(u.usuario), "User_%d", i);
        for(j = 0; j < POSTEOS_POR_FEED; j++){
            u.feed[j] = rand() % 100; // Concentrado en los primeros 100 posteos
        }
        fwrite(&u, sizeof(tUsuario), 1, archConc);
    }
    fclose(archConc);
    printf("-> Lote Concentrado generado (%s).\n", RUTA_CONCENTRADO);

    // 2. LOTE DISPERSO (posteos aleatorios entre los 50.000)
    srand(semilla); // Reiniciamos semilla para reproducibilidad
    archDisp = fopen(RUTA_DISPERSO, "wb");
    if(!archDisp){
        printf("Error al crear %s\n", RUTA_DISPERSO);
        return 0;
    }

    for(i = 0; i < cantUsuarios; i++){
        snprintf(u.usuario, sizeof(u.usuario), "User_%d", i);
        for(j = 0; j < POSTEOS_POR_FEED; j++){
            u.feed[j] = rand() % CANT_POSTEOS; // Repartido en 50.000
        }
        fwrite(&u, sizeof(tUsuario), 1, archDisp);
    }
    fclose(archDisp);
    printf("-> Lote Disperso generado (%s).\n", RUTA_DISPERSO);

    // 3. LOTE ORDENADO (los 20 IDs de cada feed vienen en orden creciente)
    srand(semilla); // Reiniciamos semilla
    archOrd = fopen(RUTA_ORDENADO, "wb");
    if(!archOrd){
        printf("Error al crear %s\n", RUTA_ORDENADO);
        return 0;
    }

    for(i = 0; i < cantUsuarios; i++){
        snprintf(u.usuario, sizeof(u.usuario), "User_%d", i);
        for(j = 0; j < POSTEOS_POR_FEED; j++){
            u.feed[j] = rand() % CANT_POSTEOS;
        }
        // Ordenamos los 20 posteos del feed de menor a mayor
        qsort(u.feed, POSTEOS_POR_FEED, sizeof(int), comparar_enteros);
        fwrite(&u, sizeof(tUsuario), 1, archOrd);
    }
    fclose(archOrd);
    printf("-> Lote Ordenado generado (%s).\n", RUTA_ORDENADO);

    return 1;
}
