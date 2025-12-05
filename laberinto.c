/*
 * LABERINTO.C
 * Implementación de funciones de laberinto
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "laberinto.h"
#include "algoritmos.h"
#include "memes.h"

// ==================== CREACION Y DESTRUCCION ====================

Laberinto* crearLaberintoSegunTipo(int tipoGeneracion, const char* nombre) {
    Laberinto* lab = (Laberinto*)malloc(sizeof(Laberinto));
    if (!lab) return NULL;
    
    lab->filas = FILAS;
    lab->columnas = COLUMNAS;
    lab->tieneSolucion = 0;
    lab->pasosSolucion = 0;
    
    // Asignar nombre
    if (nombre == NULL || strlen(nombre) == 0) {
        obtenerNombreAleatorio(lab->nombre);
    } else {
        strncpy(lab->nombre, nombre, MAX_NOMBRE - 1);
        lab->nombre[MAX_NOMBRE - 1] = '\0';
    }
    
    // Inicializar todas las celdas como paredes
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;
        }
    }
    
    // Generar según el tipo
    switch (tipoGeneracion) {
        case 1:
            generarAleatorio(lab);
            break;
        case 2:
            generarPerfecto(lab);
            break;
        case 3:
            generarConBacktracking(lab);
            break;
        case 4: {
            GrafoMatriz grafo;
            inicializarGrafoMatriz(&grafo, FILAS * COLUMNAS);
            generarGrafoAleatorio(&grafo);
            generarDesdeGrafo(lab, &grafo);
            liberarGrafoMatriz(&grafo);
            break;
        }
        default:
            generarAleatorio(lab);
            break;
    }
    
    // Asegurar entrada y salida
    lab->celdas[0][0] = INICIO;
    lab->celdas[FILAS-1][COLUMNAS-1] = FINAL;
    
    // Verificar si tiene solución
    lab->tieneSolucion = verificarSolucion(lab);
    
    return lab;
}

void destruirLaberinto(Laberinto* lab) {
    if (lab) {
        free(lab);
    }
}

// ==================== GENERACION ====================

void generarAleatorio(Laberinto* lab) {
    srand(time(NULL));
    
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            // Mayor probabilidad de caminos cerca del centro
            int distanciaCentro = abs(i - FILAS/2) + abs(j - COLUMNAS/2);
            int probabilidad = 70 - distanciaCentro * 3;
            if (probabilidad < 20) probabilidad = 20;
            
            if (rand() % 100 < probabilidad) {
                lab->celdas[i][j] = CAMINO;
            }
        }
    }
    
    // Conectar caminos
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (lab->celdas[i][j] == CAMINO) {
                // Conectar con vecinos
                if (i > 0 && lab->celdas[i-1][j] == PARED && rand() % 100 < 30) {
                    lab->celdas[i-1][j] = CAMINO;
                }
                if (j > 0 && lab->celdas[i][j-1] == PARED && rand() % 100 < 30) {
                    lab->celdas[i][j-1] = CAMINO;
                }
            }
        }
    }
}

void generarPerfecto(Laberinto* lab) {
    // Usar el algoritmo de Prim para laberintos perfectos
    GrafoMatriz grafo;
    inicializarGrafoMatriz(&grafo, FILAS * COLUMNAS);
    
    // Primero, crear todas las paredes posibles
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            int nodo = i * COLUMNAS + j;
            
            if (i > 0) {
                agregarAristaMatriz(&grafo, nodo, (i-1)*COLUMNAS + j, rand() % 10 + 1);
            }
            if (j > 0) {
                agregarAristaMatriz(&grafo, nodo, i*COLUMNAS + (j-1), rand() % 10 + 1);
            }
        }
    }
    
    // Aplicar Prim
    int numAristas;
    AristaPrim* arbol = prim(&grafo, &numAristas);
    
    // Convertir aristas del árbol a caminos
    for (int i = 0; i < numAristas; i++) {
        int f1 = arbol[i].origen / COLUMNAS;
        int c1 = arbol[i].origen % COLUMNAS;
        int f2 = arbol[i].destino / COLUMNAS;
        int c2 = arbol[i].destino % COLUMNAS;
        
        lab->celdas[f1][c1] = CAMINO;
        lab->celdas[f2][c2] = CAMINO;
        
        // Conectar también la celda entre ellos si es necesario
        if (f1 == f2) {
            lab->celdas[f1][(c1 + c2) / 2] = CAMINO;
        } else {
            lab->celdas[(f1 + f2) / 2][c1] = CAMINO;
        }
    }
    
    liberarGrafoMatriz(&grafo);
}

void generarConBacktracking(Laberinto* lab) {
    int visitado[FILAS][COLUMNAS] = {0};
    int direcciones[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
    
    // Pila para backtracking
    int pila[FILAS * COLUMNAS][2];
    int tope = 0;
    
    // Empezar en una posición aleatoria
    int inicioFila = rand() % FILAS;
    int inicioCol = rand() % COLUMNAS;
    
    pila[tope][0] = inicioFila;
    pila[tope][1] = inicioCol;
    tope++;
    visitado[inicioFila][inicioCol] = 1;
    lab->celdas[inicioFila][inicioCol] = CAMINO;
    
    while (tope > 0) {
        int actualFila = pila[tope-1][0];
        int actualCol = pila[tope-1][1];
        
        // Mezclar direcciones
        for (int i = 0; i < 4; i++) {
            int j = rand() % 4;
            int temp[2] = {direcciones[i][0], direcciones[i][1]};
            direcciones[i][0] = direcciones[j][0];
            direcciones[i][1] = direcciones[j][1];
            direcciones[j][0] = temp[0];
            direcciones[j][1] = temp[1];
        }
        
        int encontrado = 0;
        
        for (int i = 0; i < 4; i++) {
            int nuevaFila = actualFila + direcciones[i][0] * 2;
            int nuevaCol = actualCol + direcciones[i][1] * 2;
            
            if (nuevaFila >= 0 && nuevaFila < FILAS &&
                nuevaCol >= 0 && nuevaCol < COLUMNAS &&
                !visitado[nuevaFila][nuevaCol]) {
                
                // Quitar pared entre celdas
                lab->celdas[actualFila + direcciones[i][0]][actualCol + direcciones[i][1]] = CAMINO;
                lab->celdas[nuevaFila][nuevaCol] = CAMINO;
                
                visitado[nuevaFila][nuevaCol] = 1;
                pila[tope][0] = nuevaFila;
                pila[tope][1] = nuevaCol;
                tope++;
                encontrado = 1;
                break;
            }
        }
        
        if (!encontrado) {
            tope--; // Backtrack
        }
    }
}

void generarDesdeGrafo(Laberinto* lab, GrafoMatriz* grafo) {
    // Convertir conexiones del grafo a caminos en el laberinto
    for (int i = 0; i < grafo->numNodos; i++) {
        int fila1 = i / COLUMNAS;
        int col1 = i % COLUMNAS;
        
        lab->celdas[fila1][col1] = CAMINO;
        
        for (int j = i + 1; j < grafo->numNodos; j++) {
            if (grafo->matriz[i][j] > 0) {
                int fila2 = j / COLUMNAS;
                int col2 = j % COLUMNAS;
                
                lab->celdas[fila2][col2] = CAMINO;
                
                // Conectar celdas intermedias si están en la misma fila o columna
                if (fila1 == fila2) {
                    int inicio = (col1 < col2) ? col1 : col2;
                    int fin = (col1 < col2) ? col2 : col1;
                    for (int c = inicio; c <= fin; c++) {
                        lab->celdas[fila1][c] = CAMINO;
                    }
                } else if (col1 == col2) {
                    int inicio = (fila1 < fila2) ? fila1 : fila2;
                    int fin = (fila1 < fila2) ? fila2 : fila1;
                    for (int f = inicio; f <= fin; f++) {
                        lab->celdas[f][col1] = CAMINO;
                    }
                }
            }
        }
    }
}

// ==================== VISUALIZACION ====================

void imprimirLaberinto(Laberinto* lab) {
    printf(COLOR_CYAN "\n=== LABERINTO: %s ===\n" COLOR_RESET, lab->nombre);
    printf("Tamaño: %d x %d | Solución: %s\n\n", 
           lab->filas, lab->columnas,
           lab->tieneSolucion ? "SÍ" : "NO");
    
    // Leyenda
    printf(COLOR_VERDE "E" COLOR_RESET "=Entrada  ");
    printf(COLOR_ROJO "S" COLOR_RESET "=Salida  ");
    printf("' '=Camino  ");
    printf(COLOR_BLANCO "█" COLOR_RESET "=Pared\n\n");
    
    // Borde superior
    printf("  ");
    for (int j = 0; j < lab->columnas; j++) {
        printf(COLOR_BLANCO "██" COLOR_RESET);
    }
    printf(COLOR_BLANCO "█\n" COLOR_RESET);
    
    // Contenido del laberinto
    for (int i = 0; i < lab->filas; i++) {
        printf("  " COLOR_BLANCO "█" COLOR_RESET);
        for (int j = 0; j < lab->columnas; j++) {
            switch (lab->celdas[i][j]) {
                case INICIO:
                    printf(COLOR_VERDE "E " COLOR_RESET);
                    break;
                case FINAL:
                    printf(COLOR_ROJO "S " COLOR_RESET);
                    break;
                case CAMINO:
                    printf("  ");
                    break;
                case PARED:
                    printf(COLOR_BLANCO "█ " COLOR_RESET);
                    break;
                case VISITADO:
                    printf(COLOR_AMARILLO ". " COLOR_RESET);
                    break;
                case SOLUCION:
                    printf(COLOR_VERDE "· " COLOR_RESET);
                    break;
                default:
                    printf("  ");
                    break;
            }
        }
        printf(COLOR_BLANCO "█\n" COLOR_RESET);
    }
    
    // Borde inferior
    printf("  ");
    for (int j = 0; j < lab->columnas; j++) {
        printf(COLOR_BLANCO "██" COLOR_RESET);
    }
    printf(COLOR_BLANCO "█\n" COLOR_RESET);
}

void imprimirLaberintoConSolucion(Laberinto* lab, int* camino, int longitud) {
    printf(COLOR_CYAN "\n=== LABERINTO CON SOLUCIÓN ===\n" COLOR_RESET);
    
    // Crear copia temporal con la solución marcada
    Laberinto temp = *lab;
    
    // Marcar camino de solución
    for (int i = 0; i < longitud; i++) {
        int nodo = camino[i];
        int fila = nodo / COLUMNAS;
        int col = nodo % COLUMNAS;
        
        if (temp.celdas[fila][col] != INICIO && temp.celdas[fila][col] != FINAL) {
            temp.celdas[fila][col] = SOLUCION;
        }
    }
    
    imprimirLaberinto(&temp);
    printf("\nLongitud del camino solución: %d pasos\n", longitud - 1);
}

// ==================== RESOLUCION ====================

int* resolverLaberinto(Laberinto* lab, int algoritmo, int* longitud) {
    // Convertir laberinto a grafo
    GrafoMatriz grafo;
    inicializarGrafoMatriz(&grafo, FILAS * COLUMNAS);
    
    // Crear conexiones basadas en caminos
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (lab->celdas[i][j] != PARED) {
                int nodo = i * COLUMNAS + j;
                
                // Conectar con vecinos válidos
                if (i > 0 && lab->celdas[i-1][j] != PARED) {
                    agregarAristaMatriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                }
                if (j > 0 && lab->celdas[i][j-1] != PARED) {
                    agregarAristaMatriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                }
            }
        }
    }
    
    int inicio = 0; // (0,0)
    int fin = FILAS * COLUMNAS - 1; // (FILAS-1, COLUMNAS-1)
    static int camino[MAX_NODOS];
    
    switch (algoritmo) {
        case 1: { // Dijkstra
            int distancia;
            int* anterior = dijkstra(&grafo, inicio, fin, &distancia);
            
            // Reconstruir camino
            *longitud = 0;
            int nodo = fin;
            while (nodo != -1) {
                camino[*longitud] = nodo;
                (*longitud)++;
                nodo = anterior[nodo];
            }
            
            // Invertir camino
            for (int i = 0; i < *longitud / 2; i++) {
                int temp = camino[i];
                camino[i] = camino[*longitud - i - 1];
                camino[*longitud - i - 1] = temp;
            }
            break;
        }
        
        case 2: { // BFS
            int* anterior = bfs(&grafo, inicio);
            
            // Reconstruir camino
            *longitud = 0;
            int nodo = fin;
            while (nodo != -1) {
                camino[*longitud] = nodo;
                (*longitud)++;
                nodo = anterior[nodo];
            }
            
            // Invertir camino
            for (int i = 0; i < *longitud / 2; i++) {
                int temp = camino[i];
                camino[i] = camino[*longitud - i - 1];
                camino[*longitud - i - 1] = temp;
            }
            break;
        }
        
        default:
            *longitud = 0;
            break;
    }
    
    liberarGrafoMatriz(&grafo);
    return camino;
}

int verificarSolucion(Laberinto* lab) {
    int longitud;
    int* camino = resolverLaberinto(lab, 1, &longitud); // Usar Dijkstra
    (void)camino; // Evitar warning de variable no usada
    return (longitud > 0);
}

int contarCaminosPosibles(Laberinto* lab) {
    // Implementación simplificada - en realidad sería DFS contando caminos
    int count = 0;
    
    // Para no complicar, devolvemos un estimado basado en el número de caminos
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (lab->celdas[i][j] == CAMINO) {
                count++;
            }
        }
    }
    
    return count / 10; // Estimación muy aproximada
}

// ==================== FUNCIONES AUXILIARES ====================

int obtenerNombreAleatorio(char* buffer) {
    srand(time(NULL));
    int indice = rand() % NUM_MEMES;
    strcpy(buffer, memesAnarquismo[indice]);
    return indice;
}

void convertirCoordenadas(int nodo, int* fila, int* columna) {
    *fila = nodo / COLUMNAS;
    *columna = nodo % COLUMNAS;
}

int convertirANodo(int fila, int columna) {
    return fila * COLUMNAS + columna;
}

int esValida(int fila, int columna) {
    return (fila >= 0 && fila < FILAS && columna >= 0 && columna < COLUMNAS);
}

void animarSolucion(Laberinto* lab, int* camino, int longitud) {
    printf(COLOR_MAGENTA "\n=== ANIMANDO SOLUCIÓN ===\n" COLOR_RESET);
    
    for (int paso = 0; paso < longitud; paso++) {
        // Limpiar pantalla (ANSI escape code)
        printf("\033[2J\033[H");
        
        // Crear copia temporal con el camino hasta este paso
        Laberinto temp = *lab;
        
        for (int i = 0; i <= paso; i++) {
            int nodo = camino[i];
            int fila = nodo / COLUMNAS;
            int col = nodo % COLUMNAS;
            
            if (temp.celdas[fila][col] != INICIO && temp.celdas[fila][col] != FINAL) {
                temp.celdas[fila][col] = SOLUCION;
            }
        }
        
        imprimirLaberinto(&temp);
        printf("\nPaso %d/%d\n", paso + 1, longitud);
        
        // Pequeña pausa para la animación
        usleep(100000); // 100ms
    }
}