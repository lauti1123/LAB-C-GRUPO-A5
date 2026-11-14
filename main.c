#include <stdio.h>
#include "simulador.h"
#include "validaciones.h"

int main(void)
{
    Mision mision;
    int opcion;
    int minutos;
    int respuesta;
    double altura;
    double masa;
    char direccion;

    prepararMision(&mision);
    printf("SIMULADOR DE MISIONES ARTEMIS - ORION (ESCENARIO 2)\n");

    do {
        printf("\n========== MENU PRINCIPAL ==========\n");
        printf("1. Ver calculos fisicos\n");
        printf("2. Cambiar altura y masa\n");
        printf("3. Iniciar mision\n");
        printf("4. Avanzar un turno\n");
        printf("5. Realizar control orbital\n");
        printf("6. Activar propulsor lateral\n");
        printf("7. Consultar estado\n");
        printf("8. Ver historial\n");
        printf("9. Finalizar y ver reporte\n");
        printf("0. Salir\n");

        opcion = leerEntero("Elija una opcion (0 a 9): ", 0, 9);

        switch (opcion) {
            case 1:
                mostrarCalculos(&mision);
                break;
            case 2:
                if (mision.estado == EN_CURSO) {
                    printf("Primero finalice la mision actual.\n");
                } else {
                    altura = leerReal("Altura en km (160 a 1000): ", 160.0, 1000.0);
                    masa = leerReal("Masa de Orion en kg (1000 a 100000): ", 1000.0, 100000.0);
                    cambiarDatos(&mision, altura, masa);
                }
                break;
            case 3:
                iniciarMision(&mision);
                break;
            case 4:
                if (mision.estado == EN_CURSO && mision.controlPendiente == -1) {
                    minutos = leerEntero("Minutos a avanzar (1 a 15): ", 1, 15);
                    avanzarTurno(&mision, minutos);
                } else if (mision.controlPendiente != -1 && mision.estado == EN_CURSO) {
                    printf("Primero responda el control pendiente (opcion 5).\n");
                } else {
                    printf("No hay una mision en curso.\n");
                }
                break;
            case 5:
                if (mision.estado == EN_CURSO && mision.controlPendiente != -1) {
                    mostrarPreguntaControl(&mision);
                    respuesta = leerEntero("Respuesta (1 a 3): ", 1, 3);
                    responderControl(&mision, respuesta);
                } else {
                    printf("No hay controles pendientes.\n");
                }
                break;
            case 6:
                if (mision.estado != EN_CURSO) {
                    printf("Primero inicie una mision.\n");
                } else if (mision.controlPendiente != -1) {
                    printf("Responda el control pendiente antes de usar el propulsor.\n");
                } else if (mision.usoPropulsor) {
                    printf("El propulsor ya fue utilizado en esta mision.\n");
                } else if (mision.periodoSeg - mision.tiempoSeg < TIEMPO_PULSO) {
                    printf("No quedan 10 segundos de mision para activar el propulsor.\n");
                } else {
                    printf("A: hacia afuera de la Tierra | T: hacia la Tierra\n");
                    direccion = leerLetra("Elija A o T: ", 'A', 'T');
                    activarPropulsor(&mision, direccion);
                }
                break;
            case 7:
                mostrarEstado(&mision);
                break;
            case 8:
                mostrarHistorial(&mision);
                break;
            case 9:
                finalizarMision(&mision);
                mostrarReporte(&mision);
                guardarReporte(&mision);
                break;
            case 0:
                if (mision.estado != SIN_INICIAR) {
                    finalizarMision(&mision);
                    mostrarReporte(&mision);
                    guardarReporte(&mision);
                }
                printf("Fin del programa.\n");
                break;
        }
    } while (opcion != 0);

    return 0;
}
