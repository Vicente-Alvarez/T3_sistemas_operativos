#include <stdio.h>
#include "disco.h"

void imprimir_estadisticas(Disco *d)
{
    int usados = d->total_bloques - d->libres;
    int datos = 0;
    int indices = 0;

    for (int i = 0; i < d->n_archivos; i++) {
        if (d->archivos[i].activo) {
            datos += d->archivos[i].n_datos;
            indices += d->archivos[i].n_meta;
        }
    }

    Fragmentacion f;
    calcular_fragmentacion(d, &f);

    double uso = d->total_bloques > 0
                     ? 100.0 * usados / d->total_bloques
                     : 0.0;
    double prom_frag = d->frag_muestras > 0
                           ? d->frag_suma / d->frag_muestras
                           : 0.0;
    double prom_acc = d->n_accesos > 0
                          ? (double)d->suma_accesos / d->n_accesos
                          : 0.0;

    printf("\n ESTADISTICAS FINALES \n");
    printf("Mecanismo de asignacion:    %s\n", nombre_mecanismo(d->mec));
    printf("Tiempo simulado:            %d s\n", d->segundos);
    printf("Bloques totales:            %d\n", d->total_bloques);
    printf("Bloques usados:             %d (%.2f%%)\n", usados, uso);
    printf("Bloques libres:             %d (%.2f%%)\n", d->libres, 100.0 - uso);
    printf("Bloques de datos:           %d\n", datos);
    if (d->mec == INDEXADA)
        printf("Bloques indice:             %d\n", indices);
    if (d->mec == ENLAZADA)
        printf("Espacio de punteros:        %.2f KB\n",
               (double)datos * TAM_PUNTERO / 1024.0);
    printf("Archivos creados:           %d\n", d->creados);
    printf("Archivos eliminados:        %d\n", d->eliminados);
    printf("Archivos activos:           %d\n", d->activos);
    printf("Fragmentacion externa:      %.2f%% (final)\n", f.porcentaje);
    printf("Fragmentacion externa max:  %.2f%%\n", d->frag_max);
    printf("Fragmentacion externa prom: %.2f%%\n", prom_frag);
    printf("Accesos logicos realizados: %d\n", d->n_accesos);
    printf("Promedio de accesos/bloque: %.2f\n", prom_acc);
    printf("\n");
}
