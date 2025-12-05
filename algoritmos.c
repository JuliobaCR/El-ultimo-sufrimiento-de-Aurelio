/*
 * ALGORITMOS.C
 * Implementación de algoritmos de grafos
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "algoritmos.h"
#include "config.h"

// ==================== DIJKSTRA ====================

int* dijkstra(GrafoMatriz* grafo, int inicio, int fin, int* distanciaTotal) {
    static int anterior[MAX_NODOS];
    int distancia[MAX_NODOS];
    int visitado[MAX_NODOS] = {0};
    
    // Inicialización
    for (int i = 0; i < grafo->numNodos; i++) {
        distancia[i] = INFINITO;
        anterior[i] = -1;
    }
    
    distancia[inicio] = 0;
    
    // Encontrar el camino más corto para todos los nodos
    for (int count = 0; count < grafo->numNodos - 1; count++) {
        // Encontrar el nodo con la distancia mínima no visitado
        int min = INFINITO;
        int u = -1;
        
        for (int v = 0; v < grafo->numNodos; v++) {
            if (!visitado[v] && distancia[v] < min) {
                min = distancia[v];
                u = v;
            }
        }
        
        if (u == -1 || u == fin) break;
        
        visitado[u] = 1;
        
        // Actualizar distancias de los vecinos
        for (int v = 0; v < grafo->numNodos; v++) {
            if (!visitado[v] && grafo->matriz[u][v] != 0) {
                int nuevaDistancia = distancia[u] + grafo->matriz[u][v];
                if (nuevaDistancia < distancia[v]) {
                    distancia[v] = nuevaDistancia;
                    anterior[v] = u;
                }
            }
        }
    }
    
    *distanciaTotal = (distancia[fin] == INFINITO) ? -1 : distancia[fin];
    return anterior;
}

// ==================== PRIM ====================

AristaPrim* prim(GrafoMatriz* grafo, int* numAristas) {
    static AristaPrim arbol[MAX_NODOS - 1];
    int clave[MAX_NODOS];
    int padre[MAX_NODOS];
    int enArbol[MAX_NODOS] = {0};
    
    // Inicialización
    for (int i = 0; i < grafo->numNodos; i++) {
        clave[i] = INFINITO;
        padre[i] = -1;
    }
    
    clave[0] = 0;
    padre[0] = -1;
    
    // Construir árbol generador mínimo
    for (int count = 0; count < grafo->numNodos - 1; count++) {
        // Encontrar nodo con clave mínima
        int min = INFINITO;
        int u = -1;
        
        for (int v = 0; v < grafo->numNodos; v++) {
            if (!enArbol[v] && clave[v] < min) {
                min = clave[v];
                u = v;
            }
        }
        
        if (u == -1) break;
        
        enArbol[u] = 1;
        
        // Actualizar claves de los vecinos
        for (int v = 0; v < grafo->numNodos; v++) {
            if (grafo->matriz[u][v] != 0 && !enArbol[v] && 
                grafo->matriz[u][v] < clave[v]) {
                clave[v] = grafo->matriz[u][v];
                padre[v] = u;
            }
        }
    }
    
    // Recolectar aristas del árbol
    *numAristas = 0;
    for (int i = 1; i < grafo->numNodos; i++) {
        if (padre[i] != -1) {
            arbol[*numAristas].origen = padre[i];
            arbol[*numAristas].destino = i;
            arbol[*numAristas].peso = grafo->matriz[padre[i]][i];
            arbol[*numAristas].enArbol = 1;
            (*numAristas)++;
        }
    }
    
    return arbol;
}

// ==================== KRUSKAL ====================

int compararAristas(const void* a, const void* b) {
    Arista* aristaA = (Arista*)a;
    Arista* aristaB = (Arista*)b;
    return aristaA->peso - aristaB->peso;
}

Arista* kruskal(GrafoMatriz* grafo, int* numAristas) {
    static Arista arbol[MAX_ARISTAS];
    ConjuntoDisjunto cd;
    Arista aristas[MAX_ARISTAS];
    int totalAristas = 0;
    
    // Recolectar todas las aristas
    for (int i = 0; i < grafo->numNodos; i++) {
        for (int j = i + 1; j < grafo->numNodos; j++) {
            if (grafo->matriz[i][j] > 0) {
                aristas[totalAristas].origen = i;
                aristas[totalAristas].destino = j;
                aristas[totalAristas].peso = grafo->matriz[i][j];
                totalAristas++;
            }
        }
    }
    
    // Ordenar aristas por peso
    qsort(aristas, totalAristas, sizeof(Arista), compararAristas);
    
    // Inicializar conjuntos disjuntos
    for (int i = 0; i < grafo->numNodos; i++) {
        hacerConjunto(&cd, i);
    }
    
    // Construir árbol generador mínimo
    *numAristas = 0;
    for (int i = 0; i < totalAristas && *numAristas < grafo->numNodos - 1; i++) {
        int origen = aristas[i].origen;
        int destino = aristas[i].destino;
        
        if (encontrar(&cd, origen) != encontrar(&cd, destino)) {
            arbol[*numAristas] = aristas[i];
            (*numAristas)++;
            unir(&cd, origen, destino);
        }
    }
    
    return arbol;
}

// ==================== BFS ====================

int* bfs(GrafoMatriz* grafo, int inicio) {
    static int anterior[MAX_NODOS];
    int cola[MAX_NODOS];
    int visitado[MAX_NODOS] = {0};
    int frente = 0, final = 0;
    
    for (int i = 0; i < grafo->numNodos; i++) {
        anterior[i] = -1;
    }
    
    cola[final++] = inicio;
    visitado[inicio] = 1;
    
    while (frente < final) {
        int actual = cola[frente++];
        
        for (int v = 0; v < grafo->numNodos; v++) {
            if (grafo->matriz[actual][v] > 0 && !visitado[v]) {
                visitado[v] = 1;
                anterior[v] = actual;
                cola[final++] = v;
            }
        }
    }
    
    return anterior;
}

// ==================== FUNCIONES AUXILIARES ====================

void imprimirCamino(int* anterior, int inicio, int fin) {
    if (anterior[fin] == -1) {
        printf("No hay camino desde %d hasta %d\n", inicio, fin);
        return;
    }
    
    // Reconstruir camino en orden inverso
    int camino[MAX_NODOS];
    int longitud = 0;
    int nodo = fin;
    
    while (nodo != -1) {
        camino[longitud++] = nodo;
        nodo = anterior[nodo];
    }
    
    // Imprimir camino en orden correcto
    printf(COLOR_VERDE "Camino encontrado (%d pasos):\n" COLOR_RESET, longitud - 1);
    for (int i = longitud - 1; i >= 0; i--) {
        printf("%d", camino[i]);
        if (i > 0) {
            printf(" -> ");
            if ((longitud - i) % 10 == 0) printf("\n              ");
        }
    }
    printf("\n");
}

int** crearMatrizAdyacencia(GrafoMatriz* grafo) {
    int** matriz = (int**)malloc(grafo->numNodos * sizeof(int*));
    for (int i = 0; i < grafo->numNodos; i++) {
        matriz[i] = (int*)malloc(grafo->numNodos * sizeof(int));
        for (int j = 0; j < grafo->numNodos; j++) {
            matriz[i][j] = grafo->matriz[i][j];
        }
    }
    return matriz;
}

void liberarMatrizAdyacencia(int** matriz, int n) {
    for (int i = 0; i < n; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

// ==================== ALGORITMOS ADICIONALES ====================

void encontrarCaminosCriticos(GrafoMatriz* grafo) {
    printf(COLOR_CYAN "\n=== CAMINOS CRITICOS ===\n" COLOR_RESET);
    
    int** distancias = crearMatrizAdyacencia(grafo);
    int n = grafo->numNodos;
    
    // Inicializar Floyd-Warshall
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i != j && distancias[i][j] == 0) {
                distancias[i][j] = INFINITO;
            }
        }
    }
    
    // Floyd-Warshall
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (distancias[i][k] != INFINITO && 
                    distancias[k][j] != INFINITO &&
                    distancias[i][k] + distancias[k][j] < distancias[i][j]) {
                    distancias[i][j] = distancias[i][k] + distancias[k][j];
                }
            }
        }
    }
    
    // Encontrar caminos críticos (los más largos)
    int maxDistancia = 0;
    int inicioCritico = -1, finCritico = -1;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (distancias[i][j] != INFINITO && distancias[i][j] > maxDistancia) {
                maxDistancia = distancias[i][j];
                inicioCritico = i;
                finCritico = j;
            }
        }
    }
    
    if (inicioCritico != -1) {
        printf("Camino más largo: del nodo %d al nodo %d\n", 
               inicioCritico, finCritico);
        printf("Distancia: %d\n", maxDistancia);
    }
    
    liberarMatrizAdyacencia(distancias, n);
}

void encontrarComponentesConexas(GrafoMatriz* grafo) {
    printf(COLOR_CYAN "\n=== COMPONENTES CONEXAS ===\n" COLOR_RESET);
    
    int visitado[MAX_NODOS] = {0};
    int componentes = 0;
    
    for (int i = 0; i < grafo->numNodos; i++) {
        if (!visitado[i]) {
            componentes++;
            printf("\nComponente %d: ", componentes);
            
            // BFS desde este nodo
            int cola[MAX_NODOS];
            int frente = 0, final = 0;
            
            cola[final++] = i;
            visitado[i] = 1;
            
            while (frente < final) {
                int actual = cola[frente++];
                printf("%d ", actual);
                
                for (int v = 0; v < grafo->numNodos; v++) {
                    if (grafo->matriz[actual][v] > 0 && !visitado[v]) {
                        visitado[v] = 1;
                        cola[final++] = v;
                    }
                }
            }
        }
    }
    
    printf("\n\nTotal de componentes conexas: %d\n", componentes);
}

void ordenacionTopologica(GrafoMatriz* grafo) {
    printf(COLOR_CYAN "\n=== ORDENACION TOPOLOGICA ===\n" COLOR_RESET);
    
    // Contar grados de entrada
    int gradoEntrada[MAX_NODOS] = {0};
    
    for (int i = 0; i < grafo->numNodos; i++) {
        for (int j = 0; j < grafo->numNodos; j++) {
            if (grafo->matriz[i][j] > 0) {
                gradoEntrada[j]++;
            }
        }
    }
    
    // Cola para nodos sin dependencias
    int cola[MAX_NODOS];
    int frente = 0, final = 0;
    
    for (int i = 0; i < grafo->numNodos; i++) {
        if (gradoEntrada[i] == 0) {
            cola[final++] = i;
        }
    }
    
    // Procesar en orden topológico
    int orden[MAX_NODOS];
    int idx = 0;
    
    while (frente < final) {
        int actual = cola[frente++];
        orden[idx++] = actual;
        
        for (int v = 0; v < grafo->numNodos; v++) {
            if (grafo->matriz[actual][v] > 0) {
                gradoEntrada[v]--;
                if (gradoEntrada[v] == 0) {
                    cola[final++] = v;
                }
            }
        }
    }
    
    // Verificar si hay ciclo
    if (idx != grafo->numNodos) {
        printf("El grafo contiene ciclos! (no es DAG)\n");
    } else {
        printf("Orden topológico: ");
        for (int i = 0; i < idx; i++) {
            printf("%d ", orden[i]);
        }
        printf("\n");
    }
}