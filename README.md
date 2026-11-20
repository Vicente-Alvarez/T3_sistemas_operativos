# Operaciones y métricas

Esta parte cubre la eliminación periódica de archivos, el acceso a posiciones lógicas, la fragmentación externa y las estadísticas finales. Todo se apoya en `disco.h`, que define las estructuras compartidas con el núcleo del disco.

## Archivos

| Archivo | Función | Qué hace |
|---|---|---|
| `eliminacion.c` | `eliminar_archivo_aleatorio(t)` | Elige un archivo activo al azar, llama a `liberar_archivo()` y suma 1 a los eliminados. |
| `acceso.c` | `acceder_posicion_logica(t)` | Elige un archivo activo y una posición lógica (en bytes) al azar, calcula el bloque lógico, muestra el bloque físico y cuenta los accesos. |
| `fragmentacion.c` | `calcular_fragmentacion_externa()` y `registrar_fragmentacion(t)` | Calcula la fragmentación, la muestra por pantalla y guarda último valor, máximo y promedio. |
| `estadisticas.c` | `mostrar_estadisticas()` | Imprime el resumen final. Aquí también se define la variable global `metricas`. |

## Cómo se llaman (simulacion.c)

Desde el segundo 30, cada 5 segundos:

```c
eliminar_archivo_aleatorio(t);
acceder_posicion_logica(t);
registrar_fragmentacion(t);
```

Al terminar la simulación (sin espacio o fragmentación que impide asignar):

```c
mostrar_estadisticas();
```

`t` es el segundo actual de la simulación, solo se usa para imprimir.

## Qué se necesita del núcleo del disco

- `Disco disco;` definido en algún `.c` del núcleo.
- `mapa[i] == -1` significa bloque libre. Si está ocupado, guarda el id del archivo dueño.
- Cada `Archivo` debe tener `bloques[]` con los bloques físicos de datos **en orden lógico**, para los tres mecanismos (incluida contigua).
- `liberar_archivo(Archivo *a)` debe poner `activo = 0` y marcar como `-1` en `mapa` todos los bloques del archivo (datos, punteros e índice).
- Los archivos creados se cuentan con `disco.n_archivos`, así que no hace falta otro contador.
- `bloque_indice` solo se usa en indexada (en los demás mecanismos, `-1`).

## Cómo se cuentan los accesos

| Mecanismo | Bloque lógico `k` | Accesos |
|---|---|---|
| Contigua | `pos / tam_bloque` | 1 |
| Enlazada | `pos / (tam_bloque - TAM_PUNTERO)` | `k + 1` (se recorre la cadena desde el inicio) |
| Indexada | `pos / tam_bloque` | 2 (bloque índice + bloque de datos) |

`TAM_PUNTERO` vale 4 bytes. Si usan índices de varios niveles o encadenados, hay que ajustar el `switch` de `acceso.c`.

## Fragmentación externa

```
fragmentación = (1 - mayor_hueco_libre / total_bloques_libres) * 100
```

Se recorre `mapa` buscando huecos de bloques libres consecutivos. Con un solo hueco (o sin bloques libres) el resultado es 0 %.

## Estadísticas finales

Se muestran bloques totales, usados y libres, archivos creados, eliminados y activos, fragmentación externa (final, máxima y promedio), accesos lógicos realizados y promedio de accesos por bloque.

## Compilar

```
gcc -Wall -Wextra -o simulador main.c disco.c asignacion.c simulacion.c eliminacion.c acceso.c fragmentacion.c estadisticas.c
```
