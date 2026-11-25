#include <stdio.h>
#include "disco.h"

void calcular_fragmentacion(const Disco *d, Fragmentacion *f)
{
    int actual = 0;

    f->libres = 0;
    f->huecos = 0;
    f->mayor_hueco = 0;

    for (int i = 0; i < d->total_bloques; i++) {
        if (d->mapa[i] == -1) {
            if (actual == 0)
                f->huecos++;
            f->libres++;
            actual++;
            if (actual > f->mayor_hueco)
                f->mayor_hueco = actual;
        } else {
            actual = 0;
        }
    }

    if (f->libres == 0)
        f->porcentaje = 0.0;
    else
        f->porcentaje = (1.0 - (double)f->mayor_hueco / (double)f->libres) * 100.0;
}

void registrar_fragmentacion(Disco *d)
{
    Fragmentacion f;

    calcular_fragmentacion(d, &f);

    if (f.porcentaje > d->frag_max)
        d->frag_max = f.porcentaje;
    d->frag_suma += f.porcentaje;
    d->frag_muestras++;

    printf("[t=%ds] FRAGMENTACION EXTERNA: %.2f%% (%d bloques libres en %d huecos)\n",
           d->segundos, f.porcentaje, f.libres, f.huecos);
}
