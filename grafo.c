/*
 * GRAFO.C
 * Implementación de operaciones de grafos
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "grafo.h"

// ==================== GENERACION DE GRAFOS CON CAMINO ====================

void generarGrafoConCamino(GrafoMatriz* grafo) {
    srand(time(NULL));
    
    // Primero crear un camino principal desde el nodo 0 hasta el último
    int numNodos = grafo->numNodos;
    
    // Crear camino principal (garantizado)
    for (int i = 0; i < numNodos - 1; i++) {
        int peso = rand() % 10 + 1;
        agregarAristaMatriz(grafo, i, i + 1, peso);
    }
    
    // Ahora agregar conexiones aleatorias para crear caminos alternativos
    for (int i = 0; i < numNodos; i++) {
        // Conectar con algunos vecinos aleatorios
        for (int k = 0; k < 3; k++) { // Hasta 3 conexiones adicionales por nodo
            int posibleVecino = i + (rand() % 5) - 2; // Vecino cerca
            if (posibleVecino >= 0 && posibleVecino < numNodos && 
                posibleVecino != i && grafo->matriz[i][posibleVecino] == 0) {
                
                // Probabilidad de conectar
                if (rand() % 100 < 40) {
                    int peso = rand() % 15 + 1;
                    agregarAristaMatriz(grafo, i, posibleVecino, peso);
                }
            }
        }
    }
    
    // Asegurar algunas conexiones diagonales para hacer el grafo más interesante
    for (int i = 0; i < numNodos / 4; i++) {
        int nodo1 = rand() % numNodos;
        int nodo2 = rand() % numNodos;
        
        if (nodo1 != nodo2 && grafo->matriz[nodo1][nodo2] == 0) {
            if (rand() % 100 < 20) { // 20% de probabilidad
                int peso = rand() % 20 + 5;
                agregarAristaMatriz(grafo, nodo1, nodo2, peso);
            }
        }
    }
}

// ==================== GRAFO CON MATRIZ ====================

void inicializarGrafoMatriz(GrafoMatriz* grafo, int numNodos) {
    grafo->numNodos = numNodos;
    
    for (int i = 0; i < MAX_NODOS; i++) {
        for (int j = 0; j < MAX_NODOS; j++) {
            grafo->matriz[i][j] = 0;
        }
    }
}

void agregarAristaMatriz(GrafoMatriz* grafo, int origen, int destino, int peso) {
    if (origen >= 0 && origen < grafo->numNodos &&
        destino >= 0 && destino < grafo->numNodos) {
        grafo->matriz[origen][destino] = peso;
        grafo->matriz[destino][origen] = peso; // Grafo no dirigido
    }
}

void eliminarAristaMatriz(GrafoMatriz* grafo, int origen, int destino) {
    if (origen >= 0 && origen < grafo->numNodos &&
        destino >= 0 && destino < grafo->numNodos) {
        grafo->matriz[origen][destino] = 0;
        grafo->matriz[destino][origen] = 0;
    }
}

int obtenerPesoMatriz(GrafoMatriz* grafo, int origen, int destino) {
    if (origen >= 0 && origen < grafo->numNodos &&
        destino >= 0 && destino < grafo->numNodos) {
        return grafo->matriz[origen][destino];
    }
    return 0;
}

void imprimirMatrizAdyacencia(GrafoMatriz* grafo) {
    printf("\n" COLOR_CYAN "=== MATRIZ DE ADYACENCIA ===\n" COLOR_RESET);
    printf("   ");
    for (int i = 0; i < 10 && i < grafo->numNodos; i++) {
        printf("%3d", i);
    }
    if (grafo->numNodos > 10) printf(" ...");
    printf("\n");
    
    for (int i = 0; i < 10 && i < grafo->numNodos; i++) {
        printf("%2d ", i);
        for (int j = 0; j < 10 && j < grafo->numNodos; j++) {
            if (grafo->matriz[i][j] > 0) {
                printf(COLOR_VERDE "%3d" COLOR_RESET, grafo->matriz[i][j]);
            } else {
                printf("  .");
            }
        }
        if (grafo->numNodos > 10) printf(" ...");
        printf("\n");
    }
    if (grafo->numNodos > 10) {
        printf("...\n");
        printf("(Matriz completa: %d x %d)\n", grafo->numNodos, grafo->numNodos);
    }
}

void liberarGrafoMatriz(GrafoMatriz* grafo) {
    // En matriz estática, solo reseteamos
    grafo->numNodos = 0;
}

// ==================== GRAFO CON LISTA ====================

void inicializarGrafoLista(GrafoLista* grafo, int numNodos) {
    grafo->numNodos = numNodos;
    for (int i = 0; i < MAX_NODOS; i++) {
        grafo->lista[i] = NULL;
    }
}

void agregarAristaLista(GrafoLista* grafo, int origen, int destino, int peso) {
    if (origen < 0 || origen >= grafo->numNodos ||
        destino < 0 || destino >= grafo->numNodos) {
        return;
    }
    
    // Agregar a lista de origen
    NodoLista* nuevo = (NodoLista*)malloc(sizeof(NodoLista));
    nuevo->destino = destino;
    nuevo->peso = peso;
    nuevo->siguiente = grafo->lista[origen];
    grafo->lista[origen] = nuevo;
    
    // Agregar a lista de destino (grafo no dirigido)
    nuevo = (NodoLista*)malloc(sizeof(NodoLista));
    nuevo->destino = origen;
    nuevo->peso = peso;
    nuevo->siguiente = grafo->lista[destino];
    grafo->lista[destino] = nuevo;
}

void eliminarAristaLista(GrafoLista* grafo, int origen, int destino) {
    // Eliminar de lista de origen
    NodoLista* actual = grafo->lista[origen];
    NodoLista* anterior = NULL;
    
    while (actual != NULL && actual->destino != destino) {
        anterior = actual;
        actual = actual->siguiente;
    }
    
    if (actual != NULL) {
        if (anterior == NULL) {
            grafo->lista[origen] = actual->siguiente;
        } else {
            anterior->siguiente = actual->siguiente;
        }
        free(actual);
    }
    
    // Eliminar de lista de destino
    actual = grafo->lista[destino];
    anterior = NULL;
    
    while (actual != NULL && actual->destino != origen) {
        anterior = actual;
        actual = actual->siguiente;
    }
    
    if (actual != NULL) {
        if (anterior == NULL) {
            grafo->lista[destino] = actual->siguiente;
        } else {
            anterior->siguiente = actual->siguiente;
        }
        free(actual);
    }
}

int obtenerPesoLista(GrafoLista* grafo, int origen, int destino) {
    NodoLista* actual = grafo->lista[origen];
    
    while (actual != NULL) {
        if (actual->destino == destino) {
            return actual->peso;
        }
        actual = actual->siguiente;
    }
    
    return 0;
}

void imprimirListaAdyacencia(GrafoLista* grafo) {
    printf("\n" COLOR_CYAN "=== LISTA DE ADYACENCIA ===\n" COLOR_RESET);
    
    for (int i = 0; i < grafo->numNodos; i++) {
        printf(COLOR_AMARILLO "Nodo %d:" COLOR_RESET, i);
        
        NodoLista* actual = grafo->lista[i];
        while (actual != NULL) {
            printf(" -> [%d (%d)]", actual->destino, actual->peso);
            actual = actual->siguiente;
        }
        printf("\n");
    }
}

void liberarGrafoLista(GrafoLista* grafo) {
    for (int i = 0; i < grafo->numNodos; i++) {
        NodoLista* actual = grafo->lista[i];
        while (actual != NULL) {
            NodoLista* temp = actual;
            actual = actual->siguiente;
            free(temp);
        }
        grafo->lista[i] = NULL;
    }
    grafo->numNodos = 0;
}

// ==================== CONJUNTO DISJUNTO ====================

void hacerConjunto(ConjuntoDisjunto* cd, int x) {
    cd->padre[x] = x;
    cd->rango[x] = 0;
}

int encontrar(ConjuntoDisjunto* cd, int x) {
    if (cd->padre[x] != x) {
        cd->padre[x] = encontrar(cd, cd->padre[x]);
    }
    return cd->padre[x];
}

void unir(ConjuntoDisjunto* cd, int x, int y) {
    int raizX = encontrar(cd, x);
    int raizY = encontrar(cd, y);
    
    if (raizX != raizY) {
        if (cd->rango[raizX] < cd->rango[raizY]) {
            cd->padre[raizX] = raizY;
        } else if (cd->rango[raizX] > cd->rango[raizY]) {
            cd->padre[raizY] = raizX;
        } else {
            cd->padre[raizY] = raizX;
            cd->rango[raizX]++;
        }
    }
}

// ==================== CONVERSIONES ====================

void matrizALista(GrafoMatriz* origen, GrafoLista* destino) {
    inicializarGrafoLista(destino, origen->numNodos);
    
    for (int i = 0; i < origen->numNodos; i++) {
        for (int j = i + 1; j < origen->numNodos; j++) {
            if (origen->matriz[i][j] > 0) {
                agregarAristaLista(destino, i, j, origen->matriz[i][j]);
            }
        }
    }
}

void listaAMatriz(GrafoLista* origen, GrafoMatriz* destino) {
    inicializarGrafoMatriz(destino, origen->numNodos);
    
    for (int i = 0; i < origen->numNodos; i++) {
        NodoLista* actual = origen->lista[i];
        while (actual != NULL) {
            if (actual->destino > i) { // Evitar duplicados
                agregarAristaMatriz(destino, i, actual->destino, actual->peso);
            }
            actual = actual->siguiente;
        }
    }
}

// ==================== GENERACION DE GRAFOS ====================

void generarGrafoAleatorio(GrafoMatriz* grafo) {
    srand(time(NULL));
    
    // Conectar cada nodo con sus vecinos (arriba, abajo, izquierda, derecha)
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            int nodoActual = i * COLUMNAS + j;
            
            // Conectar con nodo de la derecha (si existe)
            if (j < COLUMNAS - 1) {
                int nodoDerecha = i * COLUMNAS + (j + 1);
                int peso = rand() % 10 + 1;
                if (rand() % 100 < PROBABILIDAD_CAMINO) {
                    agregarAristaMatriz(grafo, nodoActual, nodoDerecha, peso);
                }
            }
            
            // Conectar con nodo de abajo (si existe)
            if (i < FILAS - 1) {
                int nodoAbajo = (i + 1) * COLUMNAS + j;
                int peso = rand() % 10 + 1;
                if (rand() % 100 < PROBABILIDAD_CAMINO) {
                    agregarAristaMatriz(grafo, nodoActual, nodoAbajo, peso);
                }
            }
        }
    }
    
    // Agregar algunas conexiones diagonales aleatorias
    for (int k = 0; k < FILAS * COLUMNAS / 4; k++) {
        int nodo1 = rand() % grafo->numNodos;
        int nodo2 = rand() % grafo->numNodos;
        
        if (nodo1 != nodo2 && grafo->matriz[nodo1][nodo2] == 0) {
            if (rand() % 100 < PROBABILIDAD_DIAGONAL) {
                int peso = rand() % 15 + 5;
                agregarAristaMatriz(grafo, nodo1, nodo2, peso);
            }
        }
    }
}

void generarGrafoBacktracking(GrafoMatriz* grafo) {
    srand(time(NULL));
    
    // Inicializar todas las celdas como no visitadas
    int visitado[MAX_NODOS] = {0};
    
    // Usar pila para backtracking
    int pila[MAX_NODOS];
    int tope = 0;
    
    // Empezar en una celda aleatoria
    int celdaInicial = rand() % grafo->numNodos;
    pila[tope++] = celdaInicial;
    visitado[celdaInicial] = 1;
    
    while (tope > 0) {
        int celdaActual = pila[tope - 1];
        
        // Obtener vecinos no visitados
        int i = celdaActual / COLUMNAS;
        int j = celdaActual % COLUMNAS;
        int vecinos[4];
        int numVecinos = 0;
        
        // Arriba
        if (i > 0) {
            int vecino = (i - 1) * COLUMNAS + j;
            if (!visitado[vecino]) {
                vecinos[numVecinos++] = vecino;
            }
        }
        
        // Derecha
        if (j < COLUMNAS - 1) {
            int vecino = i * COLUMNAS + (j + 1);
            if (!visitado[vecino]) {
                vecinos[numVecinos++] = vecino;
            }
        }
        
        // Abajo
        if (i < FILAS - 1) {
            int vecino = (i + 1) * COLUMNAS + j;
            if (!visitado[vecino]) {
                vecinos[numVecinos++] = vecino;
            }
        }
        
        // Izquierda
        if (j > 0) {
            int vecino = i * COLUMNAS + (j - 1);
            if (!visitado[vecino]) {
                vecinos[numVecinos++] = vecino;
            }
        }
        
        if (numVecinos > 0) {
            // Elegir un vecino aleatorio
            int indice = rand() % numVecinos;
            int vecino = vecinos[indice];
            
            // Conectar con el vecino
            int peso = rand() % 10 + 1;
            agregarAristaMatriz(grafo, celdaActual, vecino, peso);
            
            // Marcar como visitado y agregar a la pila
            visitado[vecino] = 1;
            pila[tope++] = vecino;
        } else {
            // Backtrack
            tope--;
        }
    }
}

void generarGrafoCompleto(GrafoMatriz* grafo) {
    for (int i = 0; i < grafo->numNodos; i++) {
        for (int j = i + 1; j < grafo->numNodos; j++) {
            int peso = rand() % 20 + 1;
            agregarAristaMatriz(grafo, i, j, peso);
        }
    }
}

void generarGrafoCuadricula(GrafoMatriz* grafo) {
    // Conectar solo vecinos ortogonales
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            int nodoActual = i * COLUMNAS + j;
            
            // Derecha
            if (j < COLUMNAS - 1) {
                int nodoDerecha = i * COLUMNAS + (j + 1);
                agregarAristaMatriz(grafo, nodoActual, nodoDerecha, 1);
            }
            
            // Abajo
            if (i < FILAS - 1) {
                int nodoAbajo = (i + 1) * COLUMNAS + j;
                agregarAristaMatriz(grafo, nodoActual, nodoAbajo, 1);
            }
        }
    }
}