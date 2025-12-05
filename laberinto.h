/*
 * LABERINTO.H
 * Funciones para generación y visualización de laberintos
 */

#ifndef LABERINTO_H
#define LABERINTO_H

#include "grafo.h"

// Estructura para representar un laberinto
typedef struct {
    int celdas[FILAS][COLUMNAS];
    int filas;
    int columnas;
    char nombre[MAX_NOMBRE];
    int tieneSolucion;
    int pasosSolucion;
} Laberinto;

// Funciones principales
Laberinto* crearLaberintoSegunTipo(int tipoGeneracion, const char* nombre);
void destruirLaberinto(Laberinto* lab);
void imprimirLaberinto(Laberinto* lab);
void imprimirLaberintoConSolucion(Laberinto* lab, int* camino, int longitud);

// Funciones de generación
void generarDesdeGrafo(Laberinto* lab, GrafoMatriz* grafo);
void generarPerfecto(Laberinto* lab);
void generarConBacktracking(Laberinto* lab);
void generarAleatorio(Laberinto* lab);

// Funciones de solución
int* resolverLaberinto(Laberinto* lab, int algoritmo, int* longitud);
int verificarSolucion(Laberinto* lab);
int contarCaminosPosibles(Laberinto* lab);

// Funciones auxiliares
int obtenerNombreAleatorio(char* buffer);
void convertirCoordenadas(int nodo, int* fila, int* columna);
int convertirANodo(int fila, int columna);
int esValida(int fila, int columna);
void animarSolucion(Laberinto* lab, int* camino, int longitud);

#endif // LABERINTO_H