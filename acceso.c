#include <stdio.h>
#include <stdlib.h>
#include "disco.h"

int acceder_posicion_aleatoria(Disco *d)
{
    int p = elegir_archivo_activo(d);

    if (p < 0) {
        printf("[t=%ds] Acceso: no hay archivos activos\n", d->segundos);
        return 0;
    }

    const Archivo *arch = &d->archivos[p];
    long bytes = (long)arch->tam_kb * 1024;
    long pos = (long)rand() % bytes;

    long util = d->bloque_bytes;
    if (d->mec == ENLAZADA)
        util -= TAM_PUNTERO;

    int k = (int)(pos / util);
    if (k >= arch->n_datos)
        k = arch->n_datos - 1;

    int fisico = arch->datos[k];
    int accesos;

    printf("[t=%ds] Acceso archivo #%d, posición lógica %ld (bloque lógico %d) -> bloque físico %d\n",
           d->segundos, arch->id, pos, k, fisico);

    switch (d->mec) {
    case CONTIGUA:
        accesos = 1;
        printf("    Contigua: inicio %d + desplazamiento %d = bloque %d\n",
               arch->datos[0], k, fisico);
        break;
    case ENLAZADA:
        accesos = k + 1;
        printf("    Enlazada: se recorre la cadena desde el bloque %d (%d bloques leídos)\n",
               arch->datos[0], accesos);
        break;
    default: {
        int cap = indice_cap(d, arch->n_datos);
        int nivel = k / cap;
        accesos = nivel + 2;
        printf("    Indexada: bloque índice %d", arch->meta[nivel]);
        if (arch->n_meta > 1)
            printf(" (índice %d de %d en la cadena, %d punteros por índice)",
                   nivel + 1, arch->n_meta, cap);
        printf(", entrada %d -> bloque %d\n", k % cap, fisico);
        break;
    }
    }

    printf("    Accesos a disco necesarios: %d\n", accesos);

    d->suma_accesos += accesos;
    d->n_accesos++;
    return accesos;
}