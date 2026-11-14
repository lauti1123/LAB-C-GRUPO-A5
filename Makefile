CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

all: orion

orion: main.c simulador.c simulador.h validaciones.c validaciones.h
	$(CC) $(CFLAGS) main.c simulador.c validaciones.c -o orion -lm

clean:
	rm -f orion
