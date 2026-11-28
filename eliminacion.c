#include <stdio.h>
#include <stdlib.h>
#include "disco.h"

int elegir_archivo_activo(const Disco *d)
{
    if (d->activos <= 0)
        return -1;

    int elegido = rand() % d->activos;
    for (int i = 0; i < d->n_archivos; i++) {
        if (!d->archivos[i].activo)
            continue;
        if (elegido == 0)
            return i;
        elegido--;
    }
    return -1;
}

int eliminar_archivo_aleatorio(Disco *d)
{
    int pos = elegir_archivo_activo(d);

    if (pos < 0) {
        printf("[t=%ds] ELIMINACIÓN: no hay archivos activos\n", d->segundos);
        return -1;
    }

    Archivo *arch = &d->archivos[pos];
    int liberados = arch->n_datos + arch->n_meta;

    printf("[t=%ds] ELIMINADO archivo #%d de %d KB, %d bloques liberados\n",
           d->segundos, arch->id, arch->tam_kb, liberados);
    disco_imprimir_archivo(d, pos);
    disco_liberar_archivo(d, pos);
    return pos;
}