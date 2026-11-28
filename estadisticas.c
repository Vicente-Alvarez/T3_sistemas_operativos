#include <stdio.h>
#include "disco.h"

void imprimir_estadisticas(Disco *d)
{
    Fragmentacion f;
    int bloques_datos = 0;
    int bloques_meta = 0;
    long kb_activos = 0;

    calcular_fragmentacion(d, &f);

    for (int i = 0; i < d->n_archivos; i++) {
        if (!d->archivos[i].activo)
            continue;
        bloques_datos += d->archivos[i].n_datos;
        bloques_meta += d->archivos[i].n_meta;
        kb_activos += d->archivos[i].tam_kb;
    }

    int usados = d->total_bloques - d->libres;
    double uso = 100.0 * usados / d->total_bloques;
    double prom_frag = d->frag_muestras > 0 ? d->frag_suma / d->frag_muestras : 0.0;
    double prom_acc = d->n_accesos > 0 ? (double)d->suma_accesos / d->n_accesos : 0.0;
    double kb_ocupados = (double)usados * d->bloque_bytes / 1024.0;
    double interna = kb_ocupados > 0 ? 100.0 * (kb_ocupados - kb_activos) / kb_ocupados : 0.0;

    printf("\nESTADÍSTICAS FINALES\n");
    printf("Mecanismo de asignación:     %s\n", nombre_mecanismo(d->mec));
    printf("Tiempo simulado:             %d s\n", d->segundos);
    printf("Bloques totales:             %d (%d bytes c/u)\n", d->total_bloques, d->bloque_bytes);
    printf("Bloques usados:              %d (%.2f%%)\n", usados, uso);
    printf("  de datos:                  %d\n", bloques_datos);
    if (d->mec == INDEXADA)
        printf("  índice:                    %d\n", bloques_meta);
    printf("Bloques libres:              %d (%.2f%%)\n", d->libres, 100.0 - uso);
    printf("Espacio no aprovechado:      %.2f%% de lo asignado (punteros, índices y últimos bloques)\n",
           interna);
    printf("Archivos creados:            %d\n", d->creados);
    printf("Archivos eliminados:         %d\n", d->eliminados);
    printf("Archivos activos:            %d\n", d->activos);
    printf("Fragmentación externa final: %.2f%% (%d huecos, mayor hueco %d bloques)\n",
           f.porcentaje, f.huecos, f.mayor_hueco);
    printf("Fragmentación externa máx.:  %.2f%%\n", d->frag_max);
    printf("Fragmentación externa prom.: %.2f%% (%d mediciones)\n", prom_frag, d->frag_muestras);
    printf("Accesos lógicos realizados:  %d\n", d->n_accesos);
    printf("Promedio de accesos a disco: %.2f por acceso lógico\n", prom_acc);
}