#ifndef DISCO_H
#define DISCO_H

#define TAM_PUNTERO 4

#define ASIG_OK            0
#define ASIG_SIN_ESPACIO   1
#define ASIG_FRAGMENTACION 2

typedef enum { CONTIGUA = 1, ENLAZADA = 2, INDEXADA = 3 } Mecanismo;

typedef struct {
    int id;
    int tam_kb;
    int activo;
    int n_datos;
    int *datos;
    int n_meta;
    int *meta;
} Archivo;

typedef struct {
    int total_bloques;
    int bloque_bytes;
    Mecanismo mec;
    int min_kb, max_kb;

    int *mapa;
    int libres;

    Archivo *archivos;
    int n_archivos;
    int cap_archivos;
    int activos;

    int creados, eliminados;
    long suma_accesos;
    int n_accesos;

    double frag_suma;
    int frag_muestras;
    double frag_max;

    int segundos;
} Disco;

typedef struct {
    int libres;
    int huecos;
    int mayor_hueco;
    double porcentaje;
} Fragmentacion;

int  disco_init(Disco *d, int disco_mb, int bloque_kb, Mecanismo mec,
                int min_kb, int max_kb);
void disco_destruir(Disco *d);
int  disco_nuevo_archivo(Disco *d, int tam_kb, int *datos, int n_datos,
                         int *meta, int n_meta);
void disco_liberar_archivo(Disco *d, int idx);
void disco_imprimir_archivo(const Disco *d, int idx);
void disco_imprimir_mapa(const Disco *d);
const char *nombre_mecanismo(Mecanismo m);
long div_techo(long a, long b);

int asignar_archivo(Disco *d, int tam_kb, int *idx_out);
int indice_cap(const Disco *d, int n_datos);

void simulacion_ejecutar(Disco *d, int factor);

int eliminar_archivo_aleatorio(Disco *d);

int acceder_posicion_aleatoria(Disco *d);

void calcular_fragmentacion(const Disco *d, Fragmentacion *f);
void registrar_fragmentacion(Disco *d);

void imprimir_estadisticas(Disco *d);

#endif
