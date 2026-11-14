#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include "validaciones.h"

/* Lee una linea completa y evita que sobren caracteres en el teclado. */
static int leerLinea(char linea[], int capacidad)
{
    int i = 0;
    int c;

    if (fgets(linea, capacidad, stdin) == NULL) {
        printf("\nFin de la entrada.\n");
        exit(0);
    }

    while (linea[i] != '\0' && linea[i] != '\n') {
        i++;
    }
    if (linea[i] == '\n') {
        linea[i] = '\0';
        return 1;
    }

    /* Si la linea fue muy larga, descartamos lo que no entro. */
    while ((c = getchar()) != '\n' && c != EOF) {
    }
    return 0;
}

/* Comprueba que no haya texto adicional despues del numero. */
static int terminaCorrectamente(const char *texto)
{
    while (isspace((unsigned char)*texto)) {
        texto++;
    }
    return *texto == '\0';
}

int leerEntero(const char mensaje[], int minimo, int maximo)
{
    char linea[100];
    char *fin;
    long numero;

    while (1) {
        printf("%s", mensaje);
        if (leerLinea(linea, 100)) {
            errno = 0;
            numero = strtol(linea, &fin, 10);
            if (fin != linea && errno == 0 && terminaCorrectamente(fin) &&
                numero >= minimo && numero <= maximo) {
                return (int)numero;
            }
        }
        printf("Entrada incorrecta. Ingrese un entero entre %d y %d.\n", minimo, maximo);
    }
}

double leerReal(const char mensaje[], double minimo, double maximo)
{
    char linea[100];
    char *fin;
    double numero;

    while (1) {
        printf("%s", mensaje);
        if (leerLinea(linea, 100)) {
            errno = 0;
            numero = strtod(linea, &fin);
            if (fin != linea && errno == 0 && isfinite(numero) &&
                terminaCorrectamente(fin) && numero >= minimo && numero <= maximo) {
                return numero;
            }
        }
        printf("Entrada incorrecta. Ingrese un numero entre %.0f y %.0f (use punto decimal).\n",
               minimo, maximo);
    }
}

char leerLetra(const char mensaje[], char opcion1, char opcion2)
{
    char linea[100];
    int i;
    char letra;

    while (1) {
        printf("%s", mensaje);
        if (leerLinea(linea, 100)) {
            i = 0;
            while (isspace((unsigned char)linea[i])) {
                i++;
            }
            letra = (char)toupper((unsigned char)linea[i]);
            if (letra == opcion1 || letra == opcion2) {
                i++;
                if (terminaCorrectamente(linea + i)) {
                    return letra;
                }
            }
        }
        printf("Entrada incorrecta. Elija %c o %c.\n", opcion1, opcion2);
    }
}
