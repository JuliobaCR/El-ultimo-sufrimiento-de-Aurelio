/*
 * grafo.h
 * Definiciones de estructuras y operaciones de grafos.
 */

#ifndef GRAFO_H
#define GRAFO_H

#include "config.h"

// ============================================================================
// DEFINICIONES DE ESTRUCTURAS
// ============================================================================

/* arista - Estructura para representar una arista */
typedef struct {
    int origen;     // Nodo origen
    int destino;    // Nodo destino
    int peso;       // Peso de la arista
} arista;

/* nodo_lista - Estructura para un nodo en lista de adyacencia */
typedef struct nodo_lista {
    int destino;                    // Nodo destino
    int peso;                       // Peso de la arista
    struct nodo_lista* siguiente;   // Siguiente en la lista
} nodo_lista;

/* grafo_matriz - Estructura para grafo con matriz de adyacencia */
typedef struct {
    int matriz[MAX_NODOS][MAX_NODOS];   // Matriz de pesos
    int num_nodos;                      // Número total de nodos
} grafo_matriz;

/* grafo_lista - Estructura para grafo con lista de adyacencia */
typedef struct {
    nodo_lista* lista[MAX_NODOS];   // Array de listas
    int num_nodos;                  // Número total de nodos
} grafo_lista;

/* conjunto_disjunto - Estructura para algoritmo Union-Find */
typedef struct {
    int padre[MAX_NODOS];   // Padre de cada elemento
    int rango[MAX_NODOS];   // Rango para optimización
} conjunto_disjunto;

// ============================================================================
// PROTOTIPOS DE FUNCIONES
// ============================================================================

/* Funciones para grafo matriz */
void inicializar_grafo_matriz(grafo_matriz* grafo, int num_nodos);
void agregar_arista_matriz(grafo_matriz* grafo, int origen, int destino, int peso);
void eliminar_arista_matriz(grafo_matriz* grafo, int origen, int destino);
int obtener_peso_matriz(grafo_matriz* grafo, int origen, int destino);
void imprimir_matriz_adyacencia(grafo_matriz* grafo);
void liberar_grafo_matriz(grafo_matriz* grafo);

/* Funciones para grafo lista */
void inicializar_grafo_lista(grafo_lista* grafo, int num_nodos);
void agregar_arista_lista(grafo_lista* grafo, int origen, int destino, int peso);
void eliminar_arista_lista(grafo_lista* grafo, int origen, int destino);
int obtener_peso_lista(grafo_lista* grafo, int origen, int destino);
void imprimir_lista_adyacencia(grafo_lista* grafo);
void liberar_grafo_lista(grafo_lista* grafo);

/* Funciones para conjuntos disjuntos */
void hacer_conjunto(conjunto_disjunto* cd, int x);
int encontrar(conjunto_disjunto* cd, int x);
void unir(conjunto_disjunto* cd, int x, int y);

/* Conversiones entre representaciones */
void matriz_a_lista(grafo_matriz* origen, grafo_lista* destino);
void lista_a_matriz(grafo_lista* origen, grafo_matriz* destino);

/* Generación de grafos */
void generar_grafo_aleatorio(grafo_matriz* grafo);
void generar_grafo_backtracking(grafo_matriz* grafo);
void generar_grafo_completo(grafo_matriz* grafo);
void generar_grafo_cuadricula(grafo_matriz* grafo);
void generar_grafo_con_camino(grafo_matriz* grafo);

#endif // GRAFO_H