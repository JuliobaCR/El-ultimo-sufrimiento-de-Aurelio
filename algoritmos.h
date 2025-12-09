/*
 * algoritmos.h
 * Algoritmos de grafos para análisis de laberintos.
 * Incluye Dijkstra, Prim, Kruskal y otros algoritmos.
 */

#ifndef ALGORITMOS_H
#define ALGORITMOS_H

#include "grafo.h"

// ============================================================================
// DEFINICIONES DE ESTRUCTURAS PARA ALGORITMOS
// ============================================================================

/*
 * info_dijkstra - Estructura auxiliar para algoritmo de Dijkstra
 */
typedef struct {
    int distancia;  // Distancia mínima desde inicio
    int anterior;   // Nodo anterior en camino óptimo
    int visitado;   // Flag de nodo visitado
} info_dijkstra;

/*
 * arista_prim - Estructura para arista en algoritmo de Prim
 */
typedef struct {
    int origen;     // Nodo origen
    int destino;    // Nodo destino
    int peso;       // Peso de la arista
    int en_arbol;   // Si está en el árbol
} arista_prim;

// ============================================================================
// PROTOTIPOS DE FUNCIONES - ALGORITMOS PRINCIPALES
// ============================================================================

/* Dijkstra con visualización de estados */
int* dijkstra_con_estados(grafo_matriz* grafo, int inicio, int fin, 
                         int* distancia_total, int mostrar_estados);

/* Funciones auxiliares de Dijkstra */
void imprimir_estado_dijkstra(int num_nodos, int distancia[], int anterior[], 
                              int visitado[], int nodo_actual, int iteracion);
void reconstruir_camino_detallado(int* anterior, int inicio, int fin, 
                                  int distancia_total);

/* Algoritmos de árboles generadores */
arista_prim* prim(grafo_matriz* grafo, int* num_aristas);
arista* kruskal(grafo_matriz* grafo, int* num_aristas);

/* Búsqueda en grafos */
int* bfs(grafo_matriz* grafo, int inicio);

/* Análisis de grafos */
void encontrar_caminos_criticos(grafo_matriz* grafo);
void encontrar_componentes_conexas(grafo_matriz* grafo);
void ordenacion_topologica(grafo_matriz* grafo);

/* Funciones auxiliares */
void imprimir_camino(int* anterior, int inicio, int fin);
int comparar_aristas(const void* a, const void* b);
int** crear_matriz_adyacencia(grafo_matriz* grafo);
void liberar_matriz_adyacencia(int** matriz, int n);
void liberar_aristas(arista* aristas);

/* Explicación de algoritmos */
void explicar_algoritmos_seleccion(void);

#endif // ALGORITMOS_H