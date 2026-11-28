#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "disco.h"

#define MAX_DISCO_MB 16384
#define MAX_BLOQUE_KB 65536

static void vaciar_entrada(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

static int pedir_entero(const char *mensaje, int minimo, int maximo)
{
    int valor;

    for (;;) {
        printf("%s", mensaje);
        if (scanf("%d", &valor) == 1 && valor >= minimo && valor <= maximo) {
            vaciar_entrada();
            return valor;
        }
        if (feof(stdin)) {
            printf("\nEntrada cerrada, saliendo.\n");
            exit(1);
        }
        vaciar_entrada();
        printf("  Valor inválido (debe ser un entero entre %d y %d).\n", minimo, maximo);
    }
}

static Mecanismo pedir_mecanismo(void)
{
    char texto[64];

    for (;;) {
        printf("Mecanismo de asignación (contigua / enlazada / indexada): ");
        if (scanf("%63s", texto) != 1) {
            printf("\nEntrada cerrada, saliendo.\n");
            exit(1);
        }
        vaciar_entrada();
        for (char *c = texto; *c; c++)
            *c = (char)tolower((unsigned char)*c);

        if (strcmp(texto, "contigua") == 0 || strcmp(texto, "1") == 0)
            return CONTIGUA;
        if (strcmp(texto, "enlazada") == 0 || strcmp(texto, "2") == 0)
            return ENLAZADA;
        if (strcmp(texto, "indexada") == 0 || strcmp(texto, "3") == 0)
            return INDEXADA;
        printf("  Opción inválida.\n");
    }
}

int main(int argc, char *argv[])
{
    int velocidad = 1;
    Disco disco;

    if (argc > 1) {
        velocidad = atoi(argv[1]);
        if (velocidad < 1 || velocidad > 1000)
            velocidad = 1;
    }

    printf("Simulador de asignación de archivos\n\n");

    int disco_mb = pedir_entero("Tamaño del disco (MB): ", 1, MAX_DISCO_MB);
    int tope_bloque = disco_mb * 512 < MAX_BLOQUE_KB ? disco_mb * 512 : MAX_BLOQUE_KB;
    int bloque_kb = pedir_entero("Tamaño de cada bloque (KB): ", 1, tope_bloque);
    Mecanismo mecanismo = pedir_mecanismo();
    int min_kb = pedir_entero("Tamaño mínimo de archivo (KB): ", 1, disco_mb * 1024);
    int max_kb = pedir_entero("Tamaño máximo de archivo (KB): ", min_kb, disco_mb * 1024);

    if (disco_init(&disco, disco_mb, bloque_kb, mecanismo, min_kb, max_kb) != 0) {
        fprintf(stderr, "Error: no se pudo inicializar el disco.\n");
        return 1;
    }

    simulacion_ejecutar(&disco, velocidad);
    disco_destruir(&disco);
    return 0;
}