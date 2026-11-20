#include <stdio.h>
#include <stdlib.h>
#include "disco.h"

int eliminar_archivo_aleatorio(int t)
{
    int activos = 0;
    for (int i = 0; i < disco.n_archivos; i++)
        if (disco.archivos[i].activo)
            activos++;

    if (activos == 0) {
        printf("[t=%ds] ELIMINACION: no hay archivos activos\n", t);
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

    int id = a->id;
    int liberados = a->n_bloques_total;
    long bytes = a->tamano_bytes;

    liberar_archivo(a);
    metricas.archivos_eliminados++;

    printf("[t=%ds] ELIMINACION: archivo %d (%ld bytes) eliminado, %d bloques liberados\n",
           t, id, bytes, liberados);
    return 1;
}
