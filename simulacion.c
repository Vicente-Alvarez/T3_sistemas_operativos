#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "disco.h"

static volatile sig_atomic_t terminar = 0;

static void al_interrumpir(int senal)
{
    (void)senal;
    terminar = 1;
}

static void pausa_un_segundo(int velocidad)
{
    long nanos = 1000000000L / velocidad;
    struct timespec espera;

    espera.tv_sec = nanos / 1000000000L;
    espera.tv_nsec = nanos % 1000000000L;
    nanosleep(&espera, NULL);
}

static int nuevo_archivo(Disco *d)
{
    int tam = d->min_kb + rand() % (d->max_kb - d->min_kb + 1);
    int pos;
    int resultado = asignar_archivo(d, tam, &pos);

    if (resultado == ASIG_OK) {
        printf("[t=%ds] CREADO archivo #%d de %d KB\n",
               d->segundos, d->archivos[pos].id, tam);
        disco_imprimir_archivo(d, pos);
        return 0;
    }

    printf("[t=%ds] No se pudo crear un archivo de %d KB: ",
           d->segundos, tam);
    if (resultado == ASIG_FRAGMENTACION)
        printf("hay bloques libres pero la fragmentación externa "
               "impide asignarlo de forma contigua.\n");
    else
        printf("no hay espacio suficiente en el disco.\n");
    return -1;
}

void simulacion_ejecutar(Disco *d, int factor)
{
    srand((unsigned)time(NULL));
    signal(SIGINT, al_interrumpir);

    printf("\n  Inicio de la simulación (asignación %s)   \n",
           nombre_mecanismo(d->mec));
    printf("Bloques: %d de %d bytes | archivos entre %d y %d KB\n",
           d->total_bloques, d->bloque_bytes, d->min_kb, d->max_kb);
    printf("(Ctrl+C para terminar y ver estadísticas)\n\n");

    while (!terminar) {
        pausa_un_segundo(factor);
        if (terminar)
            break;
        d->segundos++;

        if (d->segundos % 2 == 0) {
            if (nuevo_archivo(d) < 0)
                break;
        }

        if (d->segundos >= 30 && d->segundos % 5 == 0) {
            eliminar_archivo_aleatorio(d);
            acceder_posicion_aleatoria(d);
            registrar_fragmentacion(d);
            disco_imprimir_mapa(d);
        }
    }

    if (terminar)
        printf("\nSimulación interrumpida por el usuario.\n");

    imprimir_estadisticas(d);
}