#include <stdio.h>
#include "disco.h"

double calcular_fragmentacion_externa(void)
{
    int libres = 0;
    int mayor_hueco = 0;
    int actual = 0;

    for (int i = 0; i < disco.total_bloques; i++) {
        if (disco.mapa[i] == -1) {
            libres++;
            actual++;
            if (actual > mayor_hueco)
                mayor_hueco = actual;
        } else {
            actual = 0;
        }
    }

    if (libres == 0)
        return 0.0;

    return (1.0 - (double)mayor_hueco / (double)libres) * 100.0;
}

void registrar_fragmentacion(int t)
{
    double f = calcular_fragmentacion_externa();

    metricas.frag_ultima = f;
    if (f > metricas.frag_maxima)
        metricas.frag_maxima = f;
    metricas.frag_suma += f;
    metricas.n_muestras_frag++;

    int libres = 0;
    int huecos = 0;
    int en_hueco = 0;
    for (int i = 0; i < disco.total_bloques; i++) {
        if (disco.mapa[i] == -1) {
            libres++;
            if (!en_hueco) {
                huecos++;
                en_hueco = 1;
            }
        } else {
            en_hueco = 0;
        }
    }

    printf("[t=%ds] FRAGMENTACION EXTERNA: %.2f%% (%d bloques libres en %d huecos)\n",
           t, f, libres, huecos);
}
