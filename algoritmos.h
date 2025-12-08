/*
 * ALGORITMOS.H
 * Algoritmos de grafos para laberintos
 */

#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "grafo.h"

// Estructuras para algoritmos
typedef struct {
    int distancia;
    int anterior;
    int visitado;
} InfoDijkstra;

typedef struct {
    int origen;
    int destino;
    int peso;
    int enArbol;
} AristaPrim;

// Prototipos de funciones
int* dijkstra(GrafoMatriz* grafo, int inicio, int fin, int* distanciaTotal);
AristaPrim* prim(GrafoMatriz* grafo, int* numAristas);
Arista* kruskal(GrafoMatriz* grafo, int* numAristas);
int* bfs(GrafoMatriz* grafo, int inicio);
void encontrarCaminosCriticos(GrafoMatriz* grafo);
void encontrarComponentesConexas(GrafoMatriz* grafo);
void ordenacionTopologica(GrafoMatriz* grafo);

// Funciones auxiliares
void imprimirCamino(int* anterior, int inicio, int fin);
int compararAristas(const void* a, const void* b);
int** crearMatrizAdyacencia(GrafoMatriz* grafo);
void liberarMatrizAdyacencia(int** matriz, int n);

// Función para liberar memoria de aristas (añadida)
void liberarAristas(Arista* aristas);

#endif // ALGORITMOS_H