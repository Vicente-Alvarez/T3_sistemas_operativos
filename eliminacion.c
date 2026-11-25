#include <stdio.h>
#include <stdlib.h>
#include "disco.h"

int eliminar_archivo_aleatorio(Disco *d)
{
    int t = d->segundos;
    int activos = 0;
    for (int i = 0; i < d->n_archivos; i++)
        if (d->archivos[i].activo)
            activos++;

    if (activos == 0) {
        printf("[t=%ds] ELIMINACION: no hay archivos activos\n", t);
        return 0;
    }

    int elegido = rand() % activos;
    int pos = -1;
    for (int i = 0; i < d->n_archivos; i++) {
        if (!d->archivos[i].activo)
            continue;
        if (elegido == 0) {
            pos = i;
            break;
        }
        elegido--;
    }

    int id = d->archivos[pos].id;
    int liberados = d->archivos[pos].n_datos + d->archivos[pos].n_meta;
    int tam_kb = d->archivos[pos].tam_kb;

    disco_liberar_archivo(d, pos);

    printf("[t=%ds] ELIMINACION: archivo %d (%d KB) eliminado, %d bloques liberados\n",
           t, id, tam_kb, liberados);
    return 1;
}
