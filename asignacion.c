#include <stdlib.h>
#include "disco.h"

int indice_cap(const Disco *d, int n_datos)
{
    int punteros_por_bloque = d->bloque_bytes / TAM_PUNTERO;

    if (n_datos <= punteros_por_bloque)
        return punteros_por_bloque;
    return punteros_por_bloque - 1;
}

static void bloques_necesarios(const Disco *d, long bytes,
                               int *cant_datos, int *cant_meta)
{
    *cant_meta = 0;

    switch (d->mec) {
    case CONTIGUA:
        *cant_datos = (int)div_techo(bytes, d->bloque_bytes);
        break;
    case ENLAZADA:
        *cant_datos = (int)div_techo(bytes, d->bloque_bytes - TAM_PUNTERO);
        break;
    default:
        *cant_datos = (int)div_techo(bytes, d->bloque_bytes);
        *cant_meta = (int)div_techo(*cant_datos, indice_cap(d, *cant_datos));
        break;
    }
}

static int primer_hueco(const Disco *d, int necesarios)
{
    int comienzo = 0;
    int seguidos = 0;

    for (int b = 0; b < d->total_bloques; b++) {
        if (d->mapa[b] == -1) {
            if (seguidos == 0)
                comienzo = b;
            seguidos++;
            if (seguidos == necesarios)
                return comienzo;
        } else {
            seguidos = 0;
        }
    }
    return -1;
}

static void reservar_libres(const Disco *d, int *meta, int cant_meta,
                            int *datos, int cant_datos)
{
    int k_meta = 0;
    int k_datos = 0;

    for (int b = 0; b < d->total_bloques; b++) {
        if (k_meta >= cant_meta && k_datos >= cant_datos)
            break;
        if (d->mapa[b] != -1)
            continue;
        if (k_meta < cant_meta)
            meta[k_meta++] = b;
        else
            datos[k_datos++] = b;
    }
}

int asignar_archivo(Disco *d, int tam_kb, int *idx_out)
{
    long bytes = (long)tam_kb * 1024;
    int cant_datos, cant_meta;

    bloques_necesarios(d, bytes, &cant_datos, &cant_meta);

    if (cant_datos + cant_meta > d->libres)
        return ASIG_SIN_ESPACIO;

    int *datos = malloc(sizeof(int) * cant_datos);
    int *meta = cant_meta > 0 ? malloc(sizeof(int) * cant_meta) : NULL;
    if (datos == NULL || (cant_meta > 0 && meta == NULL)) {
        free(datos);
        free(meta);
        return ASIG_SIN_ESPACIO;
    }

    if (d->mec == CONTIGUA) {
        int comienzo = primer_hueco(d, cant_datos);
        if (comienzo < 0) {
            free(datos);
            return ASIG_FRAGMENTACION;
        }
        for (int i = 0; i < cant_datos; i++)
            datos[i] = comienzo + i;
    } else {
        reservar_libres(d, meta, cant_meta, datos, cant_datos);
    }

    int pos = disco_nuevo_archivo(d, tam_kb, datos, cant_datos, meta, cant_meta);
    if (pos < 0) {
        free(datos);
        free(meta);
        return ASIG_SIN_ESPACIO;
    }
    *idx_out = pos;
    return ASIG_OK;
}
