#include <stdio.h>
#include <stdlib.h>
#include "disco.h"

int acceder_posicion_logica(int t)
{
    int activos = 0;
    for (int i = 0; i < disco.n_archivos; i++)
        if (disco.archivos[i].activo)
            activos++;

    if (activos == 0) {
        printf("[t=%ds] ACCESO: no hay archivos activos\n", t);
        return 0;
    }

    int elegido = rand() % activos;
    Archivo *a = NULL;
    for (int i = 0; i < disco.n_archivos; i++) {
        if (!disco.archivos[i].activo)
            continue;
        if (elegido == 0) {
            a = &disco.archivos[i];
            break;
        }
        elegido--;
    }

    long pos = (long)rand() % a->tamano_bytes;

    long util = disco.tam_bloque_bytes;
    if (disco.mecanismo == ENLAZADA)
        util -= TAM_PUNTERO;

    int idx = (int)(pos / util);
    if (idx >= a->n_bloques_datos)
        idx = a->n_bloques_datos - 1;

    int fisico = a->bloques[idx];
    int accesos;

    switch (disco.mecanismo) {
    case CONTIGUA:
        accesos = 1;
        break;
    case ENLAZADA:
        accesos = idx + 1;
        break;
    default:
        accesos = 2;
        break;
    }

    metricas.n_accesos_logicos++;
    metricas.total_accesos_bloque += accesos;

    printf("[t=%ds] ACCESO: archivo %d, posicion logica %ld (bloque logico %d) -> bloque fisico %d",
           t, a->id, pos, idx, fisico);
    if (disco.mecanismo == INDEXADA)
        printf(" (bloque indice %d)", a->bloque_indice);
    printf(", accesos: %d\n", accesos);

    return accesos;
}
