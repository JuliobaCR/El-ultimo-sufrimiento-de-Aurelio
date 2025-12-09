/*
 * laberinto.h
 * Estructuras y funciones para generación y manipulación de laberintos.
 */

#ifndef LABERINTO_H
#define LABERINTO_H

#include "grafo.h"

// ============================================================================
// ESTRUCTURA DE LABERINTO
// ============================================================================

/*
 * laberinto - Estructura para representar un laberinto
 */
typedef struct {
    int celdas[FILAS][COLUMNAS];   // Matriz de celdas
    int filas;                      // Número de filas
    int columnas;                   // Número de columnas
    char nombre[MAX_NOMBRE];        // Nombre del laberinto
    int tiene_solucion;             // 1 si tiene solución
    int pasos_solucion;             // Pasos en solución óptima
} laberinto;

// ============================================================================
// PROTOTIPOS DE FUNCIONES
// ============================================================================

/* Creación y destrucción */
laberinto* crear_laberinto_segun_tipo(int tipo_generacion, const char* nombre);
void destruir_laberinto(laberinto* lab);

/* Visualización */
void imprimir_laberinto(laberinto* lab);
void imprimir_laberinto_con_solucion(laberinto* lab, int* camino, int longitud);

/* Generación de laberintos */
void generar_aleatorio(laberinto* lab);
void generar_perfecto(laberinto* lab);
void generar_con_backtracking(laberinto* lab);
void generar_desde_grafo(laberinto* lab);

/* Resolución y análisis */
int* resolver_laberinto(laberinto* lab, int algoritmo, int* longitud);
int verificar_solucion(laberinto* lab);
int contar_caminos_posibles(laberinto* lab);

/* Funciones auxiliares */
int obtener_nombre_aleatorio(char* buffer);
void convertir_coordenadas(int nodo, int* fila, int* columna);
int convertir_a_nodo(int fila, int columna);
int es_valida(int fila, int columna);
void animar_solucion(laberinto* lab, int* camino, int longitud);

#endif // LABERINTO_H