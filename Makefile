CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g
SRC = main.c disco.c asignacion.c simulacion.c eliminacion.c acceso.c fragmentacion.c estadisticas.c
OBJ = $(SRC:.c=.o)

simulador: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c disco.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) simulador

.PHONY: clean														