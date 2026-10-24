#include <stdio.h>
#include <stdlib.h>

#include "Bibliotecas/Generador.h"
#include "Bibliotecas/Tweener.h"

void vaciar_buffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

int main(){
    int opcion;
    int capacidad;
    unsigned int semilla;

    do{
        printf("\n=======================================================\n");
        printf("       RED SOCIAL TWEENER - SIMULADOR LRU CACHE        \n");
        printf("=======================================================\n");
        printf("1. Generar Base de Datos (50.000 posteos)\n");
        printf("2. Generar Lotes de Feeds con Semilla (Concentrado, Disperso, Ordenado)\n");
        printf("3. Simular Lote CONCENTRADO (Sin Cache vs P1 vs P2)\n");
        printf("4. Simular Lote DISPERSO    (Sin Cache vs P1 vs P2)\n");
        printf("5. Simular Lote ORDENADO    (Sin Cache vs P1 vs P2)\n");
        printf("6. Bateria Completa de Pruebas (Genera datos para el informe)\n");
        printf("0. Salir\n");
        printf("=======================================================\n");
        printf("Seleccione una opcion: ");
        if(scanf("%d", &opcion) != 1){
            vaciar_buffer();
            continue;
        }

        switch(opcion){
            case 1:
                generar_archivo_posteos(RUTA_POSTEOS, CANT_POSTEOS);
                break;

            case 2:
                printf("\nIngrese la semilla numerica (ej: 12345): ");
                scanf("%u", &semilla);
                generar_lotes_feeds(semilla, CANT_USUARIOS);
                break;

            case 3:
                printf("\nIndique la capacidad de la cache (ej: 20, 50, 100): ");
                scanf("%d", &capacidad);
                ejecutar_comparativa("CONCENTRADO", RUTA_CONCENTRADO, capacidad);
                break;

            case 4:
                printf("\nIndique la capacidad de la cache (ej: 20, 50, 100): ");
                scanf("%d", &capacidad);
                ejecutar_comparativa("DISPERSO", RUTA_DISPERSO, capacidad);
                break;

            case 5:
                printf("\nIndique la capacidad de la cache (ej: 20, 50, 100): ");
                scanf("%d", &capacidad);
                ejecutar_comparativa("ORDENADO", RUTA_ORDENADO, capacidad);
                break;

            case 6: {
                int capacidades[] = {5, 20, 100, 500};
                int cantCap = sizeof(capacidades) / sizeof(capacidades[0]);
                int i;

                printf("\n>>> INICIANDO BATERIA COMPLETA DE PRUEBAS <<<\n");
                for(i = 0; i < cantCap; i++){
                    ejecutar_comparativa("CONCENTRADO", RUTA_CONCENTRADO, capacidades[i]);
                    ejecutar_comparativa("DISPERSO", RUTA_DISPERSO, capacidades[i]);
                    ejecutar_comparativa("ORDENADO", RUTA_ORDENADO, capacidades[i]);
                }
                printf("\n>>> BATERIA DE PRUEBAS COMPLETADA <<<\n");
                break;
            }

            case 0:
                printf("\nSaliendo de la simulacion. Gracias!\n");
                break;

            default:
                printf("\nOpcion no valida. Intente nuevamente.\n");
        }

    } while(opcion != 0);

    return 0;
}
