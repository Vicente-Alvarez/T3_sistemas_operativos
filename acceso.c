#include <stdio.h>
#include <stdlib.h>
#include "disco.h"

int acceder_posicion_aleatoria(Disco *d)
{
    int t = d->segundos;
    int activos = 0;
    for (int i = 0; i < d->n_archivos; i++)
        if (d->archivos[i].activo)
            activos++;

    if (activos == 0) {
        printf("[t=%ds] ACCESO: no hay archivos activos\n", t);
        return 0;
    }

    int elegido = rand() % activos;
    Archivo *a = NULL;
    for (int i = 0; i < d->n_archivos; i++) {
        if (!d->archivos[i].activo)
            continue;
        if (elegido == 0) {
            a = &d->archivos[i];
            break;
        }
        elegido--;
    }

    long bytes = (long)a->tam_kb * 1024;
    long pos = (long)rand() % bytes;

    long util = d->bloque_bytes;
    if (d->mec == ENLAZADA)
        util -= TAM_PUNTERO;

    int idx = (int)(pos / util);
    if (idx >= a->n_datos)
        idx = a->n_datos - 1;

    int fisico = a->datos[idx];
    int bloque_indice = -1;
    int accesos;

    switch (d->mec) {
    case CONTIGUA:
        accesos = 1;
        break;
    case ENLAZADA:
        accesos = idx + 1;
        break;
    default: {
        int nro_indice = idx / indice_cap(d, a->n_datos);
        bloque_indice = a->meta[nro_indice];
        accesos = nro_indice + 2;
        break;
    }
    }

    d->n_accesos++;
    d->suma_accesos += accesos;

    printf("[t=%ds] ACCESO: archivo %d, posicion logica %ld (bloque logico %d) -> bloque fisico %d",
           t, a->id, pos, idx, fisico);
    if (d->mec == INDEXADA)
        printf(" (bloque indice %d)", bloque_indice);
    printf(", accesos: %d\n", accesos);

    return accesos;
}
