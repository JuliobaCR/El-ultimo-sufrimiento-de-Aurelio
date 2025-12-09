/*
 * config.h
 * Configuraciones globales del sistema de laberintos.
 * Define constantes para tamaño, colores y parámetros.
 */

#ifndef CONFIG_H
#define CONFIG_H

// Tamaño del laberinto
#define FILAS 15
#define COLUMNAS 15
#define MAX_NODOS (FILAS * COLUMNAS)

// Configuración de algoritmos
#define MAX_ARISTAS (MAX_NODOS * (MAX_NODOS - 1) / 2)
#define INFINITO 2147483647

// Configuración de archivos
#define MAX_NOMBRE 100
#define MAX_LINEA 256
#define DIRECTORIO_LABERINTOS "laberintos/"

// Configuración de generación
#define PROBABILIDAD_CAMINO 65
#define PROBABILIDAD_DIAGONAL 20
#define MAX_INTENTOS_BACKTRACKING 1000

// Colores para terminal (ANSI)
#define COLOR_RESET   "\x1b[0m"
#define COLOR_ROJO    "\x1b[31m"
#define COLOR_VERDE   "\x1b[32m"
#define COLOR_AMARILLO "\x1b[33m"
#define COLOR_AZUL    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_BLANCO  "\x1b[37m"
#define FONDO_ROJO    "\x1b[41m"
#define FONDO_VERDE   "\x1b[42m"

// Tipos de celda
#define PARED 0
#define CAMINO 1
#define INICIO 2
#define FINAL 3
#define VISITADO 4
#define SOLUCION 5

#endif