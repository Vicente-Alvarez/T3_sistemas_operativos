# Tarea 3 – Simulador de asignación de archivos

Sistemas Operativos – Universidad Diego Portales

Integrantes: Vicente Álvarez y Xiomara Vidalon

Simulador en C de los mecanismos de asignación de almacenamiento secundario (**contigua**, **enlazada** e **indexada**). El disco se divide lógicamente en bloques, cada 2 segundos se crea un archivo de tamaño aleatorio y, a partir de los 30 segundos, cada 5 segundos se elimina un archivo al azar, se accede a una posición lógica aleatoria y se registra la fragmentación externa. La simulación termina cuando un archivo no cabe (falta de espacio o fragmentación externa en contigua) y se muestran las estadísticas finales.
 
## Compilación
 
Requiere un entorno UNIX (Linux/macOS) con `gcc`.
 
```
make
```
`make clean` borra y limpia los objetos y el ejecutable.
 
## Ejecución
 
```
./simulador [velocidad]
```
 
- `velocidad` (opcional de 1 a 1000): factor de aceleración del tiempo. Con `1` cada segundo simulado dura un segundo real; con `100` dura 10 ms. Por defecto es 1.
- `Ctrl+C` detiene la simulación y muestra igualmente las estadísticas.

El programa pide por teclado:
 
| Parámetro | Validación |
|---|---|
| Tamaño del disco (MB) | entero entre 1 y 16384 |
| Tamaño de bloque (KB) | entero entre 1 y la mitad del disco (mínimo 2 bloques), máximo 65536 |
| Mecanismo | `contigua`, `enlazada`, `indexada` (o `1`, `2`, `3`) |
| Tamaño mínimo de archivo (KB) | entero entre 1 y el tamaño del disco |
| Tamaño máximo de archivo (KB) | entero entre el mínimo y el tamaño del disco |
 
Si un valor no es válido se vuelve a pedir.

 
## Estructura del código
 
| Archivo | Contenido |
|---|---|
| `disco.h` | Estructuras `Disco`, `Archivo`, `Fragmentacion` y prototipos. |
| `main.c` | Lectura y validación de parámetros, inicio de la simulación. |
| `disco.c` | Creación/destrucción del disco, mapa de bloques, alta y baja de archivos, impresión del mapa y de los bloques de cada archivo. |
| `asignacion.c` | Cálculo de bloques necesarios y asignación según el mecanismo. |
| `simulacion.c` | Bucle principal por segundos y eventos periódicos. |
| `eliminacion.c` | Selección de un archivo activo al azar y su eliminación. |
| `acceso.c` | Acceso a una posición lógica: bloque lógico, bloque físico y cantidad de accesos. |
| `fragmentacion.c` | Cálculo y registro de la fragmentación externa. |
| `estadisticas.c` | Resumen final. |
 
## Representación del disco
 
- `mapa[b]` guarda `-1` si el bloque `b` está libre, o el id del archivo dueño si está ocupado.
- `libres` lleva la cuenta de bloques libres.
- Cada `Archivo` guarda sus bloques de datos en `datos[]` **en orden lógico** y, en indexada, sus bloques índice en `meta[]`. Así siempre se sabe qué bloques usa cada archivo y dónde están.
- Al crear o eliminar un archivo se muestran sus bloques en rangos (por ejemplo `[12-40, 55]`) y, en cada evento periódico, un mapa resumido del disco (`#` lleno, `+` parcial, `.` libre).
## Mecanismos de asignación
 
**Contigua.** Se necesitan `⌈tamaño / bloque⌉` bloques consecutivos. Se usa *first fit*: el primer hueco libre lo bastante grande. Si hay bloques libres suficientes pero ningún hueco contiguo, la simulación termina por fragmentación externa.
 
**Enlazada.** Cada bloque reserva `TAM_PUNTERO` = 4 bytes para el puntero al siguiente, así que se necesitan `⌈tamaño / (bloque − 4)⌉` bloques. Pueden quedar en cualquier lugar del disco (se toman los primeros libres).
 
**Indexada.** Se necesitan `⌈tamaño / bloque⌉` bloques de datos más bloques índice. Un bloque índice guarda `bloque / 4` punteros. Si el archivo no cabe en un solo índice, se usan **índices encadenados**: cada bloque índice guarda `bloque/4 − 1` punteros a datos y el último puntero apunta al siguiente índice.
 
## Acceso a una posición lógica
 
Se elige un archivo activo y un byte `pos` al azar dentro de su tamaño.
 
| Mecanismo | Bloque lógico `k` | Accesos a disco |
|---|---|---|
| Contigua | `pos / bloque` | 1 (bloque inicial + `k`, acceso directo) |
| Enlazada | `pos / (bloque − 4)` | `k + 1` (se recorre la cadena desde el primer bloque) |
| Indexada | `pos / bloque` | `k / punteros_por_índice + 2` (índices recorridos + bloque de datos); con un solo índice son 2 |
 
Se muestran el bloque lógico, el bloque físico, el bloque índice usado (indexada) y los accesos. El promedio final es la suma de accesos dividida por la cantidad de accesos lógicos.
 
## Fragmentación externa
 
```
fragmentación = (1 − mayor_hueco_libre / total_bloques_libres) × 100
```
 
Es 0 % si todo el espacio libre forma un solo hueco (o no hay espacio libre) y se acerca a 100 % cuando el espacio libre está repartido en muchos huecos pequeños. Se mide cada 5 segundos desde el segundo 30 y se guardan el máximo y el promedio. En enlazada e indexada se informa igual, aunque no impide asignar archivos.
 
## Estadísticas finales
 
Mecanismo, tiempo simulado, bloques totales/usados/libres (separando datos e índices), espacio asignado no aprovechado (punteros, índices y relleno del último bloque), archivos creados, eliminados y activos, fragmentación externa (final, máxima y promedio), accesos lógicos realizados y promedio de accesos a disco.
 
## Decisiones de diseño
 
- El tiempo se simula con un contador de segundos y `nanosleep`; el factor de velocidad permite hacer la demostración sin esperar minutos.
- El acceso se hace después de eliminar, así que nunca se accede a un archivo eliminado.
- La simulación termina al primer archivo que no se puede asignar.
