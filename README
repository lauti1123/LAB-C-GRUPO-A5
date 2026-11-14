# Trabajo Final Integrador - Laboratorio de C
## Escenario 2: Insercion y estabilidad en orbita terrestre baja (LEO)

 

## 1. Descripcion y objetivo

La nave Orion se encuentra a 192 km sobre la Tierra. El programa calcula las condiciones de una orbita circular y permite simular el transcurso de una vuelta por turnos. El objetivo del jugador es completar el periodo orbital y contestar tres controles. Tambien puede probar un pulso lateral para estudiar su efecto aproximado.

Se usan los datos y las cuatro preguntas del **problema 2** de la consigna. Los controles del 25 %, 50 % y 75 % son reglas didacticas creadas para convertir el ejercicio en un juego de consola. No son maniobras obligatorias de una nave real.

## 2. Reglas del juego

1. Se puede consultar la Fisica antes de iniciar una mision.
2. Se puede cambiar altura (160-1000 km) y masa de la nave (1000-100000 kg) antes de iniciar. Son limites del **juego**, no limites fisicos del enunciado. Los datos iniciales son los del problema.
3. Al iniciar, el objetivo es completar una vuelta de referencia. Cada turno avanza entre 1 y 15 minutos.
4. Al superar 25 %, 50 % o 75 % aparece un **control de conocimientos** de tres opciones. Para avanzar al siguiente turno hay que responderlo. Los errores se registran sin finalizar la mision.
5. El propulsor lateral se puede usar una vez por mision, en direccion radial hacia afuera (`A`) o hacia la Tierra (`T`). La maniobra suma 10 segundos al reloj.
6. Si se activa el propulsor, el reporte informa que la trayectoria real queda perturbada. **El programa no calcula esa nueva orbita**: para los turnos sigue usando la orbita circular de referencia.
7. La mision puede finalizarse antes del 100 %, quedando como interrumpida. El reporte muestra turnos, respuestas, maniobra y resultado.

La decision de activar o no el propulsor tiene una consecuencia en el resultado: una vuelta sin maniobra mantiene la orbita circular del modelo; una vuelta con maniobra solo completa el **tiempo de referencia**, pero no demuestra estabilidad de la orbita real perturbada.

## 3. Calculos fisicos del escenario

**Valores del enunciado:**

| Magnitud | Valor |
|---|---:|
| Altura de Orion `h` | 192000 m |
| Masa de la Tierra `MT` | 5.972 x 10^24 kg |
| Radio terrestre `RT` | 6371000 m |
| Masa de Orion `m` | 33000 kg |
| Gravitacion universal `G` | 6.674 x 10^-11 N m^2/kg^2 |
| Pulso lateral | 0.50 m/s2 durante 10 s |

** Fuerza gravitatoria y centripeta, y velocidad orbital.**

- Radio de la orbita: `r = RT + h = 6563000 m`.
- Fuerza gravitatoria: `Fg = G * MT * m / (r * r)`.
- Fuerza centripeta: `Fc = m * v * v / r`.
- Igualando `Fg = Fc`, la masa de la nave se cancela y queda `v = sqrt(G * MT / r)`.
- Resultado: `v = 7792.95 m/s`; `Fg = Fc = 305362.10 N` (aproximadamente).

A la misma altura, cambiar la masa de Orion modifica la fuerza, pero no la velocidad orbital ni el periodo: la masa de la nave se cancela en la igualdad anterior.

** Periodo orbital:** `T = 2 * PI * r / v = 5291.52 s = 88.19 min`.

** Aceleracion centripeta:** `ac = v * v / r = G * MT / (r * r) = 9.2534 m/s2`.

La gravedad no desaparece. Orion y sus astronautas caen juntos alrededor de la Tierra, por eso existe **ingravidez aparente**, aunque la aceleracion centripeta no sea cero.

** Propulsor lateral:** durante un pulso de `a = 0.50 m/s2` y `t = 10 s`:

- Cambio de velocidad perpendicular: `deltaV = a * t = 5 m/s`.
- Desviacion lateral local ideal respecto del movimiento sin pulso: `d = a * t * t / 2 = 25 m`.
- Si se empuja radialmente hacia la Tierra, el desplazamiento local se muestra con signo negativo; hacia afuera, positivo.
- La trayectoria deja de ser exactamente circular. Tras un impulso radial pequeno, bajo la sola gravedad terrestre, seria una orbita ligeramente distinta, en general aproximadamente eliptica. **No se calcula esa trayectoria posterior**, porque requeriria otro modelo y excederia los calculos solicitados.

**Alcance del modelo:** las formulas de MCU se aplican a la **orbita de referencia**. Los 25 m son la aproximacion local durante el pulso suponiendo aceleracion lateral constante. No son la distancia a la orbita de referencia al finalizar una vuelta completa.

## 4. Organizacion del proyecto

```text
main.c
  |-- usa simulador.h -> funciones implementadas en simulador.c
  |-- usa validaciones.h -> funciones implementadas en validaciones.c
```

| Archivo | Responsabilidad |
|---|---|
| `main.c` | Menu, seleccion de opciones y orden general de ejecucion. |
| `simulador.h` | Constantes, `struct`, `typedef`, prototipos de funciones. |
| `simulador.c` | Calculos, turnos, controles, propulsor, historial y reporte. |
| `validaciones.h` | Declaraciones de lectura de enteros, reales y caracteres. |
| `validaciones.c` | Lectura de linea y rechazo de datos mal ingresados. |
| `Makefile` | Compilacion del programa. |

Se utilizan `if`, `switch`, `for`, `while`, `do while`, arreglos, funciones con parametros, estructuras y punteros basicos. El arreglo `controles[3]` guarda los eventos educativos; `historial[150]` es un arreglo de estructuras `Registro`. El parametro `Mision *mision` permite modificar una unica mision sin copiar toda la estructura. Los datos de la mision se manejan como una estructura en memoria, sin memoria dinamica.

## 5. Compilacion y ejecucion

Requiere un compilador de C, por ejemplo GCC. Desde la carpeta que contiene los archivos:

```sh
gcc -std=c99 -Wall -Wextra -pedantic main.c simulador.c validaciones.c -o orion -lm
./orion
```

En Windows se ejecuta `orion.exe` en lugar de `./orion`. Con `make`: `make` para compilar y `make clean` para eliminar el ejecutable.

## 6. Ejemplo rapido de una mision exitosa

1. Elegir `1` para ver los calculos.
2. Elegir `3` para comenzar.
3. Elegir `4`, ingresar `15`; repetir una vez para llegar al primer evento.
4. Elegir `5`, responder `1` (gravedad).
5. Continuar con `4` y `15` hasta el segundo control, responder `2` (periodo).
6. Continuar con `4` y `15` hasta el tercer control, responder `3` (centro de la Tierra).
7. Continuar hasta 100 %, **sin activar el propulsor**, y elegir `9` para ver el reporte.

El resultado esperado es una vuelta de referencia completa, tres controles correctos y sin perturbacion por propulsor. Para demostrar el apartado , se puede iniciar una **segunda mision** y activar la opcion `6`.

## 7. Validaciones y pruebas

Las entradas se leen por linea completa. Se rechazan letras donde van numeros, numeros fuera de rango, numeros demasiado grandes, `nan`, `inf`, entradas largas y letras no previstas. La lectura de enteros y reales usa `strtol` y `strtod`, funciones de C que detectan errores con mayor seguridad que un `scanf` directo.

Se comprueba tambien que no se pueda: avanzar sin iniciar, saltar controles, activar el propulsor dos veces ni activarlo si faltan menos de 10 segundos.

**Pruebas tecnicas realizadas durante la preparacion:** compilacion con GCC y Clang sin advertencias, comprobacion de las formulas fisicas, y ejecucion de consola con entradas incorrectas. El grupo debe repetir y documentar las pruebas que realice en su propio entorno.

## 8. Material utilizado

- Consigna: *Trabajo Practico Integrador Final - Simulador de Misiones Artemis*, escenario 2, requisitos y rubrica.
- Bibliografia aportada para aprender C: `CursoC-I`, `CursoC-II`, `Intro a C`, `Programacion en C`, `Tipos y Estructuras de Datos` y *Programacion en C: metodologia, algoritmos y estructuras de datos* 

## 9. Registro obligatorio del uso de inteligencia artificial

**Herramienta utilizada:** ChatGPT.

**Tareas realizadas con asistencia:**  planteo y desarrollo de interfaz grafica, asistencia en la logica comprensiva de los calculos para plasmar de manera correcta en el codigo.  



**Validaciones hechas sobre esta propuesta:** se contrastaron formulas y valores con la consigna y se ejecutaron compilaciones y pruebas del codigo. 
