#include <stdio.h>
#include "disco.h"

Metricas metricas = {0};

void mostrar_estadisticas(void)
{
    int usados = 0;
    for (int i = 0; i < disco.total_bloques; i++)
        if (disco.mapa[i] != -1)
            usados++;
    int libres = disco.total_bloques - usados;

    int activos = 0;
    for (int i = 0; i < disco.n_archivos; i++)
        if (disco.archivos[i].activo)
            activos++;

    double uso = disco.total_bloques > 0
                     ? 100.0 * usados / disco.total_bloques
                     : 0.0;
    double prom_frag = metricas.n_muestras_frag > 0
                           ? metricas.frag_suma / metricas.n_muestras_frag
                           : 0.0;
    double prom_acc = metricas.n_accesos_logicos > 0
                          ? (double)metricas.total_accesos_bloque / metricas.n_accesos_logicos
                          : 0.0;

    printf("\n========== ESTADISTICAS FINALES ==========\n");
    printf("Bloques totales:            %d\n", disco.total_bloques);
    printf("Bloques usados:             %d (%.2f%%)\n", usados, uso);
    printf("Bloques libres:             %d (%.2f%%)\n", libres, 100.0 - uso);
    printf("Archivos creados:           %d\n", disco.n_archivos);
    printf("Archivos eliminados:        %d\n", metricas.archivos_eliminados);
    printf("Archivos activos:           %d\n", activos);
    printf("Fragmentacion externa:      %.2f%% (final)\n", calcular_fragmentacion_externa());
    printf("Fragmentacion externa max:  %.2f%%\n", metricas.frag_maxima);
    printf("Fragmentacion externa prom: %.2f%%\n", prom_frag);
    printf("Accesos logicos realizados: %d\n", metricas.n_accesos_logicos);
    printf("Promedio de accesos/bloque: %.2f\n", prom_acc);
    printf("==========================================\n");
}
