#include <stdio.h>
#include <math.h>
#include "simulador.h"

/* Formulas del problema 2: MCU y gravitacion universal. */
double calcularRadio(double alturaKm)
{
    return RADIO_TIERRA + alturaKm * 1000.0;
}

double calcularVelocidad(double radio)
{
    return sqrt(G * MASA_TIERRA / radio);
}

double calcularPeriodo(double radio)
{
    return 2.0 * PI * radio / calcularVelocidad(radio);
}

double calcularFuerza(double masa, double radio)
{
    return G * MASA_TIERRA * masa / (radio * radio);
}

double calcularAceleracion(double radio)
{
    double velocidad = calcularVelocidad(radio);
    return velocidad * velocidad / radio;
}

static double porcentajeVuelta(const Mision *mision)
{
    if (mision->periodoSeg <= 0.0) {
        return 0.0;
    }
    return 100.0 * mision->tiempoSeg / mision->periodoSeg;
}

static void guardarRegistro(Mision *mision, char accion, int detalle)
{
    int pos = mision->cantidadRegistros;

    if (pos < MAX_REGISTROS) {
        mision->historial[pos].accion = accion;
        mision->historial[pos].minuto = mision->tiempoSeg / 60.0;
        mision->historial[pos].porcentaje = porcentajeVuelta(mision);
        mision->historial[pos].detalle = detalle;
        mision->cantidadRegistros++;
    }
}

/* Los controles son reglas educativas del juego, no eventos fisicos obligatorios. */
static void revisarControles(Mision *mision)
{
    int marcas[NUM_CONTROLES] = {25, 50, 75};
    int i;
    double progreso = porcentajeVuelta(mision);

    for (i = 0; i < NUM_CONTROLES; i++) {
        if (progreso >= marcas[i] && mision->controles[i] == 0) {
            mision->controles[i] = 1;
            mision->controlPendiente = i;
            printf("\nEVENTO: llego al %d%% de la vuelta.\n", marcas[i]);
            printf("Realice el control orbital (opcion 5) para seguir.\n");
            break;
        }
    }

    if (mision->tiempoSeg >= mision->periodoSeg) {
        mision->estado = COMPLETADA;
        printf("\nSe completo el tiempo de una vuelta circular de referencia.\n");
    }
}

void prepararMision(Mision *mision)
{
    int i;
    mision->alturaKm = ALTURA_INICIAL;
    mision->masaKg = MASA_INICIAL;
    mision->tiempoSeg = 0.0;
    mision->periodoSeg = 0.0;
    mision->desviacionLocal = 0.0;
    mision->estado = SIN_INICIAR;
    mision->turnos = 0;
    mision->usoPropulsor = 0;
    mision->controlPendiente = -1;
    mision->controlesRespondidos = 0;
    mision->controlesCorrectos = 0;
    mision->cantidadRegistros = 0;
    for (i = 0; i < NUM_CONTROLES; i++) {
        mision->controles[i] = 0;
    }
}

void cambiarDatos(Mision *mision, double altura, double masa)
{
    if (mision->estado == EN_CURSO) {
        printf("Primero finalice la mision.\n");
        return;
    }
    prepararMision(mision);
    mision->alturaKm = altura;
    mision->masaKg = masa;
    printf("Datos actualizados.\n");
}

void mostrarCalculos(const Mision *mision)
{
    double r = calcularRadio(mision->alturaKm);
    double v = calcularVelocidad(r);
    double fg = calcularFuerza(mision->masaKg, r);
    double fc = mision->masaKg * v * v / r;
    double periodo = calcularPeriodo(r);

    printf("\n========== CALCULOS ORBITALES ==========\n");
    printf("Altura: %.2f km | Masa Orion: %.2f kg\n", mision->alturaKm, mision->masaKg);
    printf("r = RT + h = %.0f m\n", r);
    printf("Fg = G * MT * m / r^2 = %.2f N\n", fg);
    printf("Fc = m * v^2 / r = %.2f N\n", fc);
    printf("Al igualar Fg = Fc se obtiene v = raiz(G * MT / r).\n");
    printf("Velocidad orbital: %.2f m/s\n", v);
    printf("Periodo T = 2 * PI * r / v: %.2f s = %.2f min\n", periodo, periodo / 60.0);
    printf("Aceleracion centripeta ac = v^2 / r: %.4f m/s2\n", calcularAceleracion(r));
    printf("La gravedad proporciona la fuerza centripeta.\n");
    printf("Los astronautas presentan ingravidez aparente por caida libre orbital.\n");
    printf("\nPulso lateral de 0.50 m/s2 durante 10 s:\n");
    printf("Cambio lateral de velocidad = a * t = %.2f m/s\n", ACELERACION_PULSO * TIEMPO_PULSO);
    printf("Desviacion local ideal = a * t^2 / 2 = %.2f m\n",
           ACELERACION_PULSO * TIEMPO_PULSO * TIEMPO_PULSO / 2.0);
}

void iniciarMision(Mision *mision)
{
    double altura = mision->alturaKm;
    double masa = mision->masaKg;

    if (mision->estado == EN_CURSO) {
        printf("La mision ya esta en curso.\n");
        return;
    }
    prepararMision(mision);
    mision->alturaKm = altura;
    mision->masaKg = masa;
    mision->periodoSeg = calcularPeriodo(calcularRadio(altura));
    mision->estado = EN_CURSO;
    printf("\nMision iniciada. Objetivo: completar %.2f minutos de orbita.\n",
           mision->periodoSeg / 60.0);
    printf("Superar los tres controles y evitar perturbaciones conserva la estabilidad de referencia.\n");
}

void avanzarTurno(Mision *mision, int minutos)
{
    double antes;
    double avanceReal;

    if (mision->estado != EN_CURSO) {
        printf("Primero inicie la mision.\n");
        return;
    }
    if (mision->controlPendiente != -1) {
        printf("Responda el control orbital pendiente.\n");
        return;
    }
    if (minutos < 1 || minutos > 15) {
        printf("El turno debe tener de 1 a 15 minutos.\n");
        return;
    }
    if (mision->cantidadRegistros >= MAX_REGISTROS) {
        printf("Historial lleno: no se puede registrar otro turno.\n");
        return;
    }

    antes = mision->tiempoSeg;
    mision->tiempoSeg += minutos * 60.0;
    if (mision->tiempoSeg > mision->periodoSeg) {
        mision->tiempoSeg = mision->periodoSeg;
    }
    avanceReal = (mision->tiempoSeg - antes) / 60.0;
    mision->turnos++;
    guardarRegistro(mision, 'T', 0);
    printf("\nTurno %d: avanzaron %.2f minutos. Progreso: %.1f%%\n",
           mision->turnos, avanceReal, porcentajeVuelta(mision));
    revisarControles(mision);
}

void mostrarPreguntaControl(const Mision *mision)
{
    if (mision->controlPendiente == 0) {
        printf("\nCONTROL 25%%: que fuerza permite mantener la orbita circular?\n");
        printf("1. La gravedad\n2. El rozamiento\n3. El propulsor lateral\n");
    } else if (mision->controlPendiente == 1) {
        printf("\nCONTROL 50%%: cual es la formula del periodo de una vuelta?\n");
        printf("1. T = v / r\n2. T = 2 * PI * r / v\n3. T = m * a\n");
    } else if (mision->controlPendiente == 2) {
        printf("\nCONTROL 75%%: hacia donde apunta la aceleracion centripeta?\n");
        printf("1. En el sentido del movimiento\n2. Hacia afuera de la Tierra\n3. Hacia el centro de la Tierra\n");
    } else {
        printf("No hay controles pendientes.\n");
    }
}

void responderControl(Mision *mision, int respuesta)
{
    int correctas[NUM_CONTROLES] = {1, 2, 3};
    int pos = mision->controlPendiente;
    int acierto;

    if (mision->estado != EN_CURSO || pos == -1) {
        printf("No hay controles pendientes.\n");
        return;
    }
    if (respuesta < 1 || respuesta > 3) {
        printf("La respuesta debe estar entre 1 y 3.\n");
        return;
    }
    if (mision->cantidadRegistros >= MAX_REGISTROS) {
        printf("Historial lleno: no se puede registrar el control.\n");
        return;
    }

    acierto = respuesta == correctas[pos];
    mision->controles[pos] = 2;
    mision->controlPendiente = -1;
    mision->controlesRespondidos++;
    if (acierto) {
        mision->controlesCorrectos++;
        printf("Respuesta correcta. Control registrado.\n");
    } else {
        printf("Respuesta incorrecta. La opcion correcta era %d.\n", correctas[pos]);
        printf("La mision continua, pero el error queda en el reporte.\n");
    }
    guardarRegistro(mision, 'C', acierto);
}

void activarPropulsor(Mision *mision, char direccion)
{
    double desviacion = ACELERACION_PULSO * TIEMPO_PULSO * TIEMPO_PULSO / 2.0;

    if (mision->estado != EN_CURSO || mision->usoPropulsor ||
        mision->controlPendiente != -1 ||
        mision->periodoSeg - mision->tiempoSeg < TIEMPO_PULSO ||
        mision->cantidadRegistros >= MAX_REGISTROS) {
        printf("No se puede activar el propulsor en este momento.\n");
        return;
    }
    if (direccion != 'A' && direccion != 'T') {
        printf("Direccion incorrecta.\n");
        return;
    }

    mision->usoPropulsor = 1;
    mision->desviacionLocal = direccion == 'A' ? desviacion : -desviacion;
    mision->tiempoSeg += TIEMPO_PULSO;
    guardarRegistro(mision, 'P', 0);
    printf("\nPropulsor hacia %s de la Tierra.\n",
           direccion == 'A' ? "afuera" : "el centro");
    printf("Cambio de velocidad perpendicular: %.2f m/s\n", ACELERACION_PULSO * TIEMPO_PULSO);
    printf("Desviacion local ideal: %+.2f m respecto de la referencia.\n",
           mision->desviacionLocal);
    printf("ADVERTENCIA: la trayectoria real queda perturbada.\n");
    printf("El programa NO calcula la nueva orbita: los turnos siguen la referencia circular.\n");
    revisarControles(mision);
}

void mostrarEstado(const Mision *mision)
{
    if (mision->estado == SIN_INICIAR) {
        printf("\nTodavia no se inicio ninguna mision.\n");
        return;
    }
    printf("\n========== ESTADO DE ORION ==========\n");
    printf("Tiempo: %.2f / %.2f minutos\n", mision->tiempoSeg / 60.0,
           mision->periodoSeg / 60.0);
    printf("Progreso de referencia: %.1f%%\n", porcentajeVuelta(mision));
    printf("Turnos: %d\n", mision->turnos);
    printf("Controles respondidos: %d/3 | Correctos: %d\n",
           mision->controlesRespondidos, mision->controlesCorrectos);
    printf("Propulsor: %s\n", mision->usoPropulsor ? "utilizado" : "sin utilizar");
    if (mision->controlPendiente != -1) {
        printf("Hay un control pendiente (opcion 5).\n");
    }
    if (mision->estado == EN_CURSO) {
        printf("Estado: EN CURSO\n");
    } else if (mision->estado == COMPLETADA) {
        printf("Estado: VUELTA DE REFERENCIA COMPLETADA\n");
    } else {
        printf("Estado: INTERRUMPIDA\n");
    }
}

/* Muestra las acciones en consola o las escribe en un archivo. */
static void escribirHistorial(FILE *destino, const Mision *mision)
{
    int i;
    const Registro *r;

    if (mision->cantidadRegistros == 0) {
        fprintf(destino, "\nNo hay acciones registradas.\n");
        return;
    }
    fprintf(destino, "\n========== HISTORIAL DE ACCIONES ==========\n");
    fprintf(destino, "Nro | Accion       | Minutos | Vuelta | Detalle\n");
    for (i = 0; i < mision->cantidadRegistros; i++) {
        r = &mision->historial[i];
        if (r->accion == 'T') {
            fprintf(destino, "%3d | Turno        | %7.2f | %5.1f%% | avance\n",
                    i + 1, r->minuto, r->porcentaje);
        } else if (r->accion == 'C') {
            fprintf(destino, "%3d | Control      | %7.2f | %5.1f%% | %s\n",
                    i + 1, r->minuto, r->porcentaje,
                    r->detalle ? "correcto" : "incorrecto");
        } else {
            fprintf(destino, "%3d | Propulsor    | %7.2f | %5.1f%% | pulso lateral\n",
                    i + 1, r->minuto, r->porcentaje);
        }
    }
}

void mostrarHistorial(const Mision *mision)
{
    escribirHistorial(stdout, mision);
}

void finalizarMision(Mision *mision)
{
    if (mision->estado == EN_CURSO) {
        mision->estado = INTERRUMPIDA;
    }
}

/* fprintf funciona tanto con stdout como con un archivo de texto. */
static void escribirReporte(FILE *destino, const Mision *mision)
{
    if (mision->estado == SIN_INICIAR) {
        fprintf(destino, "\nNo hay mision para informar.\n");
        return;
    }
    fprintf(destino, "\n========== REPORTE FINAL ==========\n");
    fprintf(destino, "Altura: %.2f km | Masa: %.2f kg\n", mision->alturaKm, mision->masaKg);
    fprintf(destino, "Tiempo: %.2f minutos\n", mision->tiempoSeg / 60.0);
    fprintf(destino, "Vuelta de referencia: %.1f%%\n", porcentajeVuelta(mision));
    fprintf(destino, "Turnos realizados: %d\n", mision->turnos);
    fprintf(destino, "Controles respondidos: %d/3\n", mision->controlesRespondidos);
    fprintf(destino, "Respuestas correctas: %d/3\n", mision->controlesCorrectos);
    fprintf(destino, "Propulsor activado: %s\n", mision->usoPropulsor ? "si" : "no");
    fprintf(destino, "Desviacion lateral local ideal: %+.2f m\n", mision->desviacionLocal);

    if (mision->estado == INTERRUMPIDA) {
        fprintf(destino, "RESULTADO: MISION INTERRUMPIDA.\n");
    } else if (mision->usoPropulsor) {
        fprintf(destino, "RESULTADO: VUELTA DE REFERENCIA COMPLETADA, CON ORBITA PERTURBADA.\n");
        fprintf(destino, "La estabilidad real posterior al pulso no esta calculada.\n");
    } else if (mision->controlesCorrectos == NUM_CONTROLES) {
        fprintf(destino, "RESULTADO: OBJETIVO CUMPLIDO. Orbita circular conservada en el modelo.\n");
    } else {
        fprintf(destino, "RESULTADO: VUELTA COMPLETADA, PERO HUBO ERRORES EN LOS CONTROLES.\n");
    }
    fprintf(destino, "===================================\n");
}

void mostrarReporte(const Mision *mision)
{
    escribirReporte(stdout, mision);
}

/* Guarda el resultado en una pagina HTML que se abre con un navegador. */
void guardarReporte(const Mision *mision)
{
    FILE *archivo;
    const char *resultado;
    const Registro *registro;
    double radio, velocidad, periodo, fuerza, aceleracion;
    double avance;
    int i;

    if (mision->estado == SIN_INICIAR) {
        printf("No hay mision para guardar.\n");
        return;
    }

    archivo = fopen("reporte_mision.html", "w");
    if (archivo == NULL) {
        printf("Error: no se pudo crear reporte_mision.html.\n");
        return;
    }

    if (mision->estado == INTERRUMPIDA) {
        resultado = "Mision interrumpida";
    } else if (mision->usoPropulsor) {
        resultado = "Vuelta de referencia completada con propulsor";
    } else if (mision->controlesCorrectos == NUM_CONTROLES) {
        resultado = "Objetivo cumplido";
    } else {
        resultado = "Vuelta completada con errores en controles";
    }

    radio = calcularRadio(mision->alturaKm);
    velocidad = calcularVelocidad(radio);
    periodo = calcularPeriodo(radio);
    fuerza = calcularFuerza(mision->masaKg, radio);
    aceleracion = calcularAceleracion(radio);
    avance = porcentajeVuelta(mision);
    if (avance > 100.0) {
        avance = 100.0;
    }

    /* Encabezado y estilos: todo queda en un unico archivo HTML. */
    fputs("<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n"
          "<meta charset=\"UTF-8\">\n"
          "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
          "<title>Reporte - Mision Orion</title>\n<style>\n"
          "body{font-family:Arial,sans-serif;background:#eef2f6;color:#203247;margin:0;padding:24px;}\n"
          "main{max-width:850px;margin:auto;background:white;border:1px solid #ccd5df;border-radius:12px;padding:24px;}\n"
          "h1{font-size:24px;margin:0 0 6px;}h2{font-size:17px;margin:24px 0 10px;}\n"
          ".subtitulo{color:#5b6a7b;font-size:14px;margin:0 0 20px;}\n"
          ".resultado{background:#e7f0f6;border-left:4px solid #245b7b;padding:12px;margin:14px 0;font-weight:bold;}\n"
          ".datos{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px;}\n"
          ".dato{background:#f6f9fc;border:1px solid #dce3eb;border-radius:7px;padding:12px;}\n"
          ".dato span{display:block;color:#5b6a7b;font-size:13px;margin-bottom:5px;}\n"
          ".dato strong{font-size:17px;}\n"
          ".barra{height:14px;background:#e1e8ef;border-radius:12px;overflow:hidden;}\n"
          ".avance{height:100%;background:#2d759b;}\n"
          "table{width:100%;border-collapse:collapse;font-size:14px;}\n"
          "th,td{padding:9px 8px;text-align:left;border-bottom:1px solid #e6eaf0;}\n"
          "th{background:#f0f4f8;}p{line-height:1.5;}\n"
          "@media(max-width:600px){body{padding:10px;}main{padding:14px;}"
          ".datos{grid-template-columns:1fr;}table{font-size:12px;}}\n"
          "</style>\n</head>\n<body>\n<main>\n"
          "<h1>Mision Orion - Reporte final</h1>\n"
          "<p class=\"subtitulo\">Escenario 2 - Orbita terrestre baja (LEO)</p>\n",
          archivo);

    fprintf(archivo, "<div class=\"resultado\">Resultado: %s</div>\n", resultado);
    fputs("<h2>Resumen de la mision</h2>\n<div class=\"datos\">\n", archivo);
    fprintf(archivo, "<div class=\"dato\"><span>Altura orbital</span><strong>%.2f km</strong></div>\n", mision->alturaKm);
    fprintf(archivo, "<div class=\"dato\"><span>Masa de Orion</span><strong>%.2f kg</strong></div>\n", mision->masaKg);
    fprintf(archivo, "<div class=\"dato\"><span>Tiempo simulado</span><strong>%.2f min</strong></div>\n", mision->tiempoSeg / 60.0);
    fprintf(archivo, "<div class=\"dato\"><span>Turnos realizados</span><strong>%d</strong></div>\n", mision->turnos);
    fprintf(archivo, "<div class=\"dato\"><span>Controles respondidos</span><strong>%d de %d</strong></div>\n", mision->controlesRespondidos, NUM_CONTROLES);
    fprintf(archivo, "<div class=\"dato\"><span>Respuestas correctas</span><strong>%d de %d</strong></div>\n", mision->controlesCorrectos, NUM_CONTROLES);
    fprintf(archivo, "<div class=\"dato\"><span>Propulsor utilizado</span><strong>%s</strong></div>\n", mision->usoPropulsor ? "Si" : "No");
    fprintf(archivo, "<div class=\"dato\"><span>Desviacion local ideal</span><strong>%+.2f m</strong></div>\n", mision->desviacionLocal);
    fputs("</div>\n<h2>Progreso de la orbita de referencia</h2>\n", archivo);
    fprintf(archivo, "<p>Vuelta completada: <strong>%.1f%%</strong></p>\n", avance);
    fprintf(archivo, "<div class=\"barra\"><div class=\"avance\" style=\"width:%.1f%%\"></div></div>\n", avance);

    fputs("<h2>Calculos fisicos</h2>\n<div class=\"datos\">\n", archivo);
    fprintf(archivo, "<div class=\"dato\"><span>Velocidad orbital</span><strong>%.2f m/s</strong></div>\n", velocidad);
    fprintf(archivo, "<div class=\"dato\"><span>Periodo orbital</span><strong>%.2f min</strong></div>\n", periodo / 60.0);
    fprintf(archivo, "<div class=\"dato\"><span>Fuerza gravitatoria / centripeta</span><strong>%.2f N</strong></div>\n", fuerza);
    fprintf(archivo, "<div class=\"dato\"><span>Aceleracion centripeta</span><strong>%.4f m/s2</strong></div>\n", aceleracion);
    fputs("</div>\n<h2>Historial de acciones</h2>\n"
          "<table><thead><tr><th>Nro</th><th>Accion</th><th>Minuto</th><th>Vuelta</th><th>Detalle</th></tr></thead><tbody>\n", archivo);

    for (i = 0; i < mision->cantidadRegistros; i++) {
        const char *accion;
        const char *detalle;
        registro = &mision->historial[i];
        if (registro->accion == 'T') {
            accion = "Turno";
            detalle = "Avance";
        } else if (registro->accion == 'C') {
            accion = "Control";
            detalle = registro->detalle ? "Correcto" : "Incorrecto";
        } else {
            accion = "Propulsor";
            detalle = "Pulso lateral";
        }
        fprintf(archivo,
                "<tr><td>%d</td><td>%s</td><td>%.2f</td><td>%.1f%%</td><td>%s</td></tr>\n",
                i + 1, accion, registro->minuto, registro->porcentaje, detalle);
    }
    if (mision->cantidadRegistros == 0) {
        fputs("<tr><td colspan=\"5\">Sin acciones registradas.</td></tr>\n", archivo);
    }
    fputs("</tbody></table>\n"
          "<p class=\"subtitulo\">Los calculos describen la orbita circular de referencia. "
          "El pulso lateral solo se evalua durante sus 10 segundos; "
          "no se calcula la nueva trayectoria posterior.</p>\n"
          "</main>\n</body>\n</html>\n", archivo);

    if (fclose(archivo) != 0) {
        printf("Error: no se pudo terminar de guardar el reporte.\n");
    } else {
        printf("Reporte guardado en reporte_mision.html\n");
        printf("Abralo con Chrome o Edge desde la carpeta de ejecucion.\n");
    }
}
