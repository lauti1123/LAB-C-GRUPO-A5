#ifndef SIMULADOR_H
#define SIMULADOR_H

/* Constantes del escenario 2. Los calculos se hacen en metros y segundos. */
#define G 6.674e-11
#define MASA_TIERRA 5.972e24
#define RADIO_TIERRA 6371000.0
#define PI 3.141592653589793
#define ALTURA_INICIAL 192.0
#define MASA_INICIAL 33000.0
#define ACELERACION_PULSO 0.50
#define TIEMPO_PULSO 10.0

#define SIN_INICIAR 0
#define EN_CURSO 1
#define COMPLETADA 2
#define INTERRUMPIDA 3

#define NUM_CONTROLES 3
#define MAX_REGISTROS 150

typedef struct {
    char accion;       /* T: turno, C: control, P: propulsor */
    double minuto;
    double porcentaje;
    int detalle;       /* En un control: 1 correcto, 0 incorrecto */
} Registro;

typedef struct {
    double alturaKm;
    double masaKg;
    double tiempoSeg;
    double periodoSeg;
    double desviacionLocal;
    int estado;
    int turnos;
    int usoPropulsor;
    int controles[NUM_CONTROLES]; /* 0 pendiente de alcanzar, 1 por responder, 2 respondido */
    int controlPendiente;        /* -1 cuando no hay control por responder */
    int controlesRespondidos;
    int controlesCorrectos;
    Registro historial[MAX_REGISTROS];
    int cantidadRegistros;
} Mision;

/* Calculos de Fisica I */
double calcularRadio(double alturaKm);
double calcularVelocidad(double radio);
double calcularPeriodo(double radio);
double calcularFuerza(double masa, double radio);
double calcularAceleracion(double radio);

/* Funciones de la simulacion */
void prepararMision(Mision *mision);
void cambiarDatos(Mision *mision, double altura, double masa);
void mostrarCalculos(const Mision *mision);
void iniciarMision(Mision *mision);
void avanzarTurno(Mision *mision, int minutos);
void mostrarPreguntaControl(const Mision *mision);
void responderControl(Mision *mision, int respuesta);
void activarPropulsor(Mision *mision, char direccion);
void mostrarEstado(const Mision *mision);
void mostrarHistorial(const Mision *mision);
void finalizarMision(Mision *mision);
void mostrarReporte(const Mision *mision);
void guardarReporte(const Mision *mision);

#endif
