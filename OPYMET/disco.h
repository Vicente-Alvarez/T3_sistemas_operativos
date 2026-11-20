#ifndef DISCO_H
#define DISCO_H

#define TAM_PUNTERO 4

typedef enum { CONTIGUA = 0, ENLAZADA = 1, INDEXADA = 2 } Mecanismo;

typedef struct {
    int id;
    long tamano_bytes;
    int n_bloques_datos;
    int n_bloques_total;
    int *bloques;
    int bloque_indice;
    int activo;
} Archivo;

typedef struct {
    int total_bloques;
    int tam_bloque_bytes;
    int *mapa;
    Mecanismo mecanismo;
    Archivo *archivos;
    int n_archivos;
} Disco;

typedef struct {
    int archivos_eliminados;
    int n_accesos_logicos;
    long total_accesos_bloque;
    int n_muestras_frag;
    double frag_ultima;
    double frag_maxima;
    double frag_suma;
} Metricas;

extern Disco disco;
extern Metricas metricas;

void liberar_archivo(Archivo *a);

int eliminar_archivo_aleatorio(int t);
int acceder_posicion_logica(int t);
double calcular_fragmentacion_externa(void);
void registrar_fragmentacion(int t);
void mostrar_estadisticas(void);

#endif
