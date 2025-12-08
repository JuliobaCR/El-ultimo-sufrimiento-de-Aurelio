/*
 * GRAFO.H
 * Definiciones de estructuras de grafos y funciones
 */

#ifndef GRAFO_H
#define GRAFO_H

#include "config.h"

// Estructura para una arista
typedef struct {
    int origen;
    int destino;
    int peso;
} Arista;

// Estructura para un nodo en lista de adyacencia
typedef struct NodoLista {
    int destino;
    int peso;
    struct NodoLista* siguiente;
} NodoLista;

// Estructura para grafo con matriz de adyacencia
typedef struct {
    int matriz[MAX_NODOS][MAX_NODOS];
    int numNodos;
} GrafoMatriz;

// Estructura para grafo con lista de adyacencia
typedef struct {
    NodoLista* lista[MAX_NODOS];
    int numNodos;
} GrafoLista;

// Estructura para conjunto disjunto (Union-Find)
typedef struct {
    int padre[MAX_NODOS];
    int rango[MAX_NODOS];
} ConjuntoDisjunto;

// Funciones de grafo matriz
void inicializarGrafoMatriz(GrafoMatriz* grafo, int numNodos);
void agregarAristaMatriz(GrafoMatriz* grafo, int origen, int destino, int peso);
void eliminarAristaMatriz(GrafoMatriz* grafo, int origen, int destino);
int obtenerPesoMatriz(GrafoMatriz* grafo, int origen, int destino);
void imprimirMatrizAdyacencia(GrafoMatriz* grafo);
void liberarGrafoMatriz(GrafoMatriz* grafo);

// Funciones de grafo lista
void inicializarGrafoLista(GrafoLista* grafo, int numNodos);
void agregarAristaLista(GrafoLista* grafo, int origen, int destino, int peso);
void eliminarAristaLista(GrafoLista* grafo, int origen, int destino);
int obtenerPesoLista(GrafoLista* grafo, int origen, int destino);
void imprimirListaAdyacencia(GrafoLista* grafo);
void liberarGrafoLista(GrafoLista* grafo);

// Funciones de conjunto disjunto
void hacerConjunto(ConjuntoDisjunto* cd, int x);
int encontrar(ConjuntoDisjunto* cd, int x);
void unir(ConjuntoDisjunto* cd, int x, int y);

// Funciones de conversión
void matrizALista(GrafoMatriz* origen, GrafoLista* destino);
void listaAMatriz(GrafoLista* origen, GrafoMatriz* destino);

// Funciones de generación
void generarGrafoAleatorio(GrafoMatriz* grafo);
void generarGrafoBacktracking(GrafoMatriz* grafo);
void generarGrafoCompleto(GrafoMatriz* grafo);
void generarGrafoCuadricula(GrafoMatriz* grafo);
void generarGrafoConCamino(GrafoMatriz* grafo);

#endif // GRAFO_H