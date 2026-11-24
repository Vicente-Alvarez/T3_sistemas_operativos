#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disco.h"

long div_techo(long a, long b)
{
    return (a + b - 1) / b;
}

const char *nombre_mecanismo(Mecanismo m)
{
    switch (m) {
    case CONTIGUA: return "contigua";
    case ENLAZADA: return "enlazada";
    default:       return "indexada";
    }
}

int disco_init(Disco *d, int disco_mb, int bloque_kb, Mecanismo mec,
               int min_kb, int max_kb)
{
    long cantidad = (long)disco_mb * 1024 / bloque_kb;

    memset(d, 0, sizeof(*d));
    if (cantidad < 2)
        return -1;

    d->total_bloques = (int)cantidad;
    d->bloque_bytes = bloque_kb * 1024;
    d->mec = mec;
    d->min_kb = min_kb;
    d->max_kb = max_kb;

    d->mapa = malloc(sizeof(int) * d->total_bloques);
    if (d->mapa == NULL)
        return -1;
    for (int b = 0; b < d->total_bloques; b++)
        d->mapa[b] = -1;
    d->libres = d->total_bloques;
    return 0;
}

void disco_destruir(Disco *d)
{
    for (int i = 0; i < d->n_archivos; i++) {
        free(d->archivos[i].datos);
        free(d->archivos[i].meta);
    }
    free(d->archivos);
    free(d->mapa);
    memset(d, 0, sizeof(*d));
}

int disco_nuevo_archivo(Disco *d, int tam_kb, int *datos, int n_datos,
                        int *meta, int n_meta)
{
    if (d->n_archivos == d->cap_archivos) {
        int nueva_cap = d->cap_archivos ? d->cap_archivos * 2 : 64;
        Archivo *nuevo = realloc(d->archivos, sizeof(Archivo) * nueva_cap);
        if (nuevo == NULL)
            return -1;
        d->archivos = nuevo;
        d->cap_archivos = nueva_cap;
    }

    int pos = d->n_archivos++;
    Archivo *arch = &d->archivos[pos];
    arch->id = pos + 1;
    arch->tam_kb = tam_kb;
    arch->activo = 1;
    arch->n_datos = n_datos;
    arch->datos = datos;
    arch->n_meta = n_meta;
    arch->meta = meta;

    for (int i = 0; i < n_datos; i++)
        d->mapa[datos[i]] = arch->id;
    for (int i = 0; i < n_meta; i++)
        d->mapa[meta[i]] = arch->id;

    d->libres -= n_datos + n_meta;
    d->activos++;
    d->creados++;
    return pos;
}

void disco_liberar_archivo(Disco *d, int pos)
{
    Archivo *arch = &d->archivos[pos];

    for (int i = 0; i < arch->n_datos; i++)
        d->mapa[arch->datos[i]] = -1;
    for (int i = 0; i < arch->n_meta; i++)
        d->mapa[arch->meta[i]] = -1;

    d->libres += arch->n_datos + arch->n_meta;
    free(arch->datos);
    free(arch->meta);
    arch->datos = NULL;
    arch->meta = NULL;
    arch->activo = 0;
    d->activos--;
    d->eliminados++;
}

static void mostrar_rangos(const int *lista, int cantidad)
{
    int grupos = 0;
    int inicio = 0;

    while (inicio < cantidad) {
        int fin = inicio;
        while (fin + 1 < cantidad && lista[fin + 1] == lista[fin] + 1)
            fin++;
        if (grupos >= 8) {
            printf("...");
            return;
        }
        if (fin > inicio)
            printf("%d-%d", lista[inicio], lista[fin]);
        else
            printf("%d", lista[inicio]);
        grupos++;
        inicio = fin + 1;
        if (inicio < cantidad)
            printf(", ");
    }
}

void disco_imprimir_archivo(const Disco *d, int pos)
{
    const Archivo *arch = &d->archivos[pos];

    printf("    Archivo #%d (%d KB): %d bloques de datos en [",
           arch->id, arch->tam_kb, arch->n_datos);
    mostrar_rangos(arch->datos, arch->n_datos);
    printf("]");
    if (arch->n_meta > 0) {
        printf(", %d bloque(s) índice en [", arch->n_meta);
        mostrar_rangos(arch->meta, arch->n_meta);
        printf("]");
    }
    printf("\n");
}

void disco_imprimir_mapa(const Disco *d)
{
    int columnas = d->total_bloques < 64 ? d->total_bloques : 64;

    printf("    Disco [");
    for (int col = 0; col < columnas; col++) {
        long desde = (long)col * d->total_bloques / columnas;
        long hasta = (long)(col + 1) * d->total_bloques / columnas;
        long ocupados = 0;
        for (long b = desde; b < hasta; b++)
            if (d->mapa[b] != -1)
                ocupados++;
        if (ocupados == 0)
            putchar('.');
        else if (ocupados == hasta - desde)
            putchar('#');
        else
            putchar('+');
    }
    printf("]  libres: %d de %d bloques\n", d->libres, d->total_bloques);
}