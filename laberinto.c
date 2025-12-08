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

// Variables globales para generación
#define MURO 1
#define VACIO 0
#define FRONTERA 2

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
        case 4:
            generarDesdeGrafo(lab);
            break;
        default:
            generarAleatorio(lab);
            break;
    }
    
    // Asegurar entrada y salida
    lab->celdas[0][0] = INICIO;
    lab->celdas[FILAS-1][COLUMNAS-1] = FINAL;
    
    // Asegurar que entrada y salida estén conectadas
    if (lab->celdas[0][1] == PARED) lab->celdas[0][1] = CAMINO;
    if (lab->celdas[1][0] == PARED) lab->celdas[1][0] = CAMINO;
    if (lab->celdas[FILAS-1][COLUMNAS-2] == PARED) lab->celdas[FILAS-1][COLUMNAS-2] = CAMINO;
    if (lab->celdas[FILAS-2][COLUMNAS-1] == PARED) lab->celdas[FILAS-2][COLUMNAS-1] = CAMINO;
    
    // Verificar si tiene solución
    lab->tieneSolucion = verificarSolucion(lab);
    
    // Contar pasos de solución si existe
    if (lab->tieneSolucion) {
        int longitud;
        int* camino = resolverLaberinto(lab, 1, &longitud);
        if (camino && longitud > 0) {
            lab->pasosSolucion = longitud - 1;
        }
    }
    
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
    
    // Algoritmo simplificado: crear un camino principal y luego agregar ramificaciones
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            // Inicialmente todo es pared
            lab->celdas[i][j] = PARED;
        }
    }
    
    // Crear un camino principal desde entrada hasta salida
    int i = 0, j = 0;
    while (i < FILAS - 1 || j < COLUMNAS - 1) {
        lab->celdas[i][j] = CAMINO;
        
        if (i < FILAS - 1 && j < COLUMNAS - 1) {
            // Puede ir hacia abajo o derecha
            if (rand() % 2 == 0) {
                i++;
            } else {
                j++;
            }
        } else if (i < FILAS - 1) {
            i++;
        } else {
            j++;
        }
    }
    lab->celdas[FILAS-1][COLUMNAS-1] = CAMINO;
    
    // Agregar caminos secundarios
    for (int k = 0; k < FILAS * COLUMNAS / 3; k++) {
        int f = rand() % FILAS;
        int c = rand() % COLUMNAS;
        
        if (lab->celdas[f][c] == PARED) {
            // Verificar si está al lado de un camino existente
            if ((f > 0 && lab->celdas[f-1][c] == CAMINO) ||
                (f < FILAS-1 && lab->celdas[f+1][c] == CAMINO) ||
                (c > 0 && lab->celdas[f][c-1] == CAMINO) ||
                (c < COLUMNAS-1 && lab->celdas[f][c+1] == CAMINO)) {
                lab->celdas[f][c] = CAMINO;
            }
        }
    }
}

void generarPerfecto(Laberinto* lab) {
    // Algoritmo de laberinto perfecto usando DFS
    srand(time(NULL));
    
    // Inicializar todo como pared
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;
        }
    }
    
    // Marcar celdas pares como potenciales caminos (para crear estructura de cuadrícula)
    for (int i = 1; i < FILAS-1; i += 2) {
        for (int j = 1; j < COLUMNAS-1; j += 2) {
            lab->celdas[i][j] = CAMINO;
        }
    }
    
    // Pila para DFS
    int pila[FILAS * COLUMNAS][2];
    int tope = 0;
    
    // Empezar desde (1,1)
    int fila = 1;
    int col = 1;
    pila[tope][0] = fila;
    pila[tope][1] = col;
    tope++;
    
    int visitado[FILAS][COLUMNAS] = {0};
    visitado[fila][col] = 1;
    
    int direcciones[4][2] = {{0, 2}, {2, 0}, {0, -2}, {-2, 0}};
    
    while (tope > 0) {
        // Obtener celda actual
        fila = pila[tope-1][0];
        col = pila[tope-1][1];
        
        // Obtener vecinos no visitados
        int vecinos[4];
        int numVecinos = 0;
        
        for (int d = 0; d < 4; d++) {
            int nf = fila + direcciones[d][0];
            int nc = col + direcciones[d][1];
            
            if (nf >= 1 && nf < FILAS-1 && nc >= 1 && nc < COLUMNAS-1 &&
                lab->celdas[nf][nc] == CAMINO && !visitado[nf][nc]) {
                vecinos[numVecinos++] = d;
            }
        }
        
        if (numVecinos > 0) {
            // Elegir un vecino aleatorio
            int dir = vecinos[rand() % numVecinos];
            int nf = fila + direcciones[dir][0];
            int nc = col + direcciones[dir][1];
            
            // Quitar la pared entre ellos
            lab->celdas[fila + direcciones[dir][0]/2][col + direcciones[dir][1]/2] = CAMINO;
            
            // Marcar como visitado y agregar a la pila
            visitado[nf][nc] = 1;
            pila[tope][0] = nf;
            pila[tope][1] = nc;
            tope++;
        } else {
            // Backtrack
            tope--;
        }
    }
}

void generarConBacktracking(Laberinto* lab) {
    srand(time(NULL));
    
    // Inicializar todo como pared
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;
        }
    }
    
    // Usaremos celdas en posiciones impares para caminos
    int filas_interiores = (FILAS - 1) / 2;
    int cols_interiores = (COLUMNAS - 1) / 2;
    
    // Matriz para el algoritmo de backtracking
    int maze[filas_interiores][cols_interiores];
    for (int i = 0; i < filas_interiores; i++) {
        for (int j = 0; j < cols_interiores; j++) {
            maze[i][j] = 0; // 0 = no visitado, 1 = visitado
        }
    }
    
    // Pila para backtracking
    int pila[filas_interiores * cols_interiores][2];
    int tope = 0;
    
    // Empezar desde (0,0)
    int x = 0, y = 0;
    maze[x][y] = 1;
    pila[tope][0] = x;
    pila[tope][1] = y;
    tope++;
    
    int direcciones[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    
    while (tope > 0) {
        // Obtener celda actual
        x = pila[tope-1][0];
        y = pila[tope-1][1];
        
        // Obtener vecinos no visitados
        int vecinos[4];
        int numVecinos = 0;
        
        for (int d = 0; d < 4; d++) {
            int nx = x + direcciones[d][0];
            int ny = y + direcciones[d][1];
            
            if (nx >= 0 && nx < filas_interiores && ny >= 0 && ny < cols_interiores &&
                maze[nx][ny] == 0) {
                vecinos[numVecinos++] = d;
            }
        }
        
        if (numVecinos > 0) {
            // Elegir un vecino aleatorio
            int dir = vecinos[rand() % numVecinos];
            int nx = x + direcciones[dir][0];
            int ny = y + direcciones[dir][1];
            
            // Marcar como visitado
            maze[nx][ny] = 1;
            
            // Quitar pared en el laberinto real
            int real_x1 = x * 2 + 1;
            int real_y1 = y * 2 + 1;
            int real_x2 = nx * 2 + 1;
            int real_y2 = ny * 2 + 1;
            
            lab->celdas[real_x1][real_y1] = CAMINO;
            lab->celdas[real_x2][real_y2] = CAMINO;
            lab->celdas[(real_x1 + real_x2) / 2][(real_y1 + real_y2) / 2] = CAMINO;
            
            // Agregar a la pila
            pila[tope][0] = nx;
            pila[tope][1] = ny;
            tope++;
        } else {
            // Backtrack
            tope--;
        }
    }
    
    // Rellenar los caminos en el laberinto real
    for (int i = 0; i < filas_interiores; i++) {
        for (int j = 0; j < cols_interiores; j++) {
            if (maze[i][j] == 1) {
                lab->celdas[i * 2 + 1][j * 2 + 1] = CAMINO;
            }
        }
    }
}

void generarDesdeGrafo(Laberinto* lab) {
    // Generar un laberinto usando Prim's algorithm
    srand(time(NULL));
    
    // Inicializar todo como pared
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;
        }
    }
    
    // Matriz para celdas visitadas
    int visitado[FILAS][COLUMNAS] = {0};
    
    // Lista de fronteras
    int fronteras[FILAS * COLUMNAS * 2][2];
    int numFronteras = 0;
    
    // Empezar desde una celda aleatoria (pero asegurar que incluya entrada)
    int start_fila = 1; //rand() % (FILAS-2) + 1;
    int start_col = 1; //rand() % (COLUMNAS-2) + 1;
    
    lab->celdas[start_fila][start_col] = CAMINO;
    visitado[start_fila][start_col] = 1;
    
    // Agregar fronteras iniciales
    int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    
    for (int d = 0; d < 4; d++) {
        int nf = start_fila + dirs[d][0] * 2;
        int nc = start_col + dirs[d][1] * 2;
        
        if (nf >= 1 && nf < FILAS-1 && nc >= 1 && nc < COLUMNAS-1) {
            fronteras[numFronteras][0] = nf;
            fronteras[numFronteras][1] = nc;
            numFronteras++;
        }
    }
    
    while (numFronteras > 0) {
        // Elegir una frontera aleatoria
        int idx = rand() % numFronteras;
        int fila = fronteras[idx][0];
        int col = fronteras[idx][1];
        
        // Eliminar esta frontera
        fronteras[idx][0] = fronteras[numFronteras-1][0];
        fronteras[idx][1] = fronteras[numFronteras-1][1];
        numFronteras--;
        
        if (!visitado[fila][col]) {
            // Marcar como visitado y hacer camino
            lab->celdas[fila][col] = CAMINO;
            visitado[fila][col] = 1;
            
            // Encontrar un vecino que ya sea camino
            int vecinosCamino[4];
            int numVecinosCamino = 0;
            
            for (int d = 0; d < 4; d++) {
                int nf = fila + dirs[d][0];
                int nc = col + dirs[d][1];
                
                if (nf >= 0 && nf < FILAS && nc >= 0 && nc < COLUMNAS &&
                    lab->celdas[nf][nc] == CAMINO && visitado[nf][nc]) {
                    vecinosCamino[numVecinosCamino++] = d;
                }
            }
            
            if (numVecinosCamino > 0) {
                // Conectar con un vecino aleatorio que ya sea camino
                int dir = vecinosCamino[rand() % numVecinosCamino];
                int pared_fila = fila + dirs[dir][0];
                int pared_col = col + dirs[dir][1];
                
                lab->celdas[pared_fila][pared_col] = CAMINO;
            }
            
            // Agregar nuevas fronteras
            for (int d = 0; d < 4; d++) {
                int nf = fila + dirs[d][0] * 2;
                int nc = col + dirs[d][1] * 2;
                
                if (nf >= 1 && nf < FILAS-1 && nc >= 1 && nc < COLUMNAS-1 &&
                    !visitado[nf][nc]) {
                    // Verificar si ya está en la lista
                    int yaEnLista = 0;
                    for (int i = 0; i < numFronteras; i++) {
                        if (fronteras[i][0] == nf && fronteras[i][1] == nc) {
                            yaEnLista = 1;
                            break;
                        }
                    }
                    
                    if (!yaEnLista) {
                        fronteras[numFronteras][0] = nf;
                        fronteras[numFronteras][1] = nc;
                        numFronteras++;
                    }
                }
            }
        }
    }
}

// ==================== VISUALIZACION ====================

void imprimirLaberinto(Laberinto* lab) {
    printf(COLOR_CYAN "\n=== LABERINTO: %s ===\n" COLOR_RESET, lab->nombre);
    printf("Tamaño: %d x %d | Solución: %s | Pasos solución: %d\n\n", 
           lab->filas, lab->columnas,
           lab->tieneSolucion ? "SÍ" : "NO",
           lab->pasosSolucion);
    
    // Leyenda
    printf(COLOR_VERDE "E" COLOR_RESET "=Entrada  ");
    printf(COLOR_ROJO "S" COLOR_RESET "=Salida  ");
    printf("  =Camino  ");
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
    int temp[FILAS][COLUMNAS];
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            temp[i][j] = lab->celdas[i][j];
        }
    }
    
    // Marcar camino de solución
    for (int i = 0; i < longitud; i++) {
        int nodo = camino[i];
        int fila = nodo / COLUMNAS;
        int col = nodo % COLUMNAS;
        
        if (temp[fila][col] != INICIO && temp[fila][col] != FINAL) {
            temp[fila][col] = SOLUCION;
        }
    }
    
    // Imprimir
    printf("Tamaño: %d x %d\n\n", lab->filas, lab->columnas);
    
    // Leyenda
    printf(COLOR_VERDE "E" COLOR_RESET "=Entrada  ");
    printf(COLOR_ROJO "S" COLOR_RESET "=Salida  ");
    printf(COLOR_VERDE "·" COLOR_RESET "=Solución  ");
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
            switch (temp[i][j]) {
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
                
                // Conectar con vecinos válidos (4 direcciones)
                if (i > 0 && lab->celdas[i-1][j] != PARED) {
                    agregarAristaMatriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                }
                if (j > 0 && lab->celdas[i][j-1] != PARED) {
                    agregarAristaMatriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                }
                if (i < FILAS-1 && lab->celdas[i+1][j] != PARED) {
                    agregarAristaMatriz(&grafo, nodo, (i+1)*COLUMNAS + j, 1);
                }
                if (j < COLUMNAS-1 && lab->celdas[i][j+1] != PARED) {
                    agregarAristaMatriz(&grafo, nodo, i*COLUMNAS + (j+1), 1);
                }
            }
        }
    }
    
    int inicio = 0; // (0,0)
    int fin = FILAS * COLUMNAS - 1; // (FILAS-1, COLUMNAS-1)
    static int camino[MAX_NODOS];
    *longitud = 0;
    
    switch (algoritmo) {
        case 1: { // Dijkstra
            int distancia;
            int* anterior = dijkstra(&grafo, inicio, fin, &distancia);
            
            if (distancia == -1 || anterior == NULL) {
                printf("No hay camino desde inicio a fin\n");
                break;
            }
            
            // Reconstruir camino
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
            
            if (anterior == NULL) {
                printf("Error en BFS\n");
                break;
            }
            
            // Reconstruir camino
            int nodo = fin;
            while (nodo != -1 && anterior[nodo] != -1) {
                camino[*longitud] = nodo;
                (*longitud)++;
                nodo = anterior[nodo];
            }
            
            // Agregar el nodo inicial
            if (*longitud > 0) {
                camino[*longitud] = inicio;
                (*longitud)++;
            }
            
            // Verificar si encontramos camino
            if (*longitud == 0) {
                printf("No hay camino desde inicio a fin\n");
                break;
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
            printf("Algoritmo no válido\n");
            *longitud = 0;
            break;
    }
    
    liberarGrafoMatriz(&grafo);
    
    if (*longitud > 0) {
        return camino;
    } else {
        return NULL;
    }
}

int verificarSolucion(Laberinto* lab) {
    int longitud;
    int* camino = resolverLaberinto(lab, 1, &longitud); // Usar Dijkstra
    return (camino != NULL && longitud > 0);
}

int contarCaminosPosibles(Laberinto* lab) {
    // Contar todas las celdas que son caminos
    int count = 0;
    
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (lab->celdas[i][j] == CAMINO || 
                lab->celdas[i][j] == INICIO || 
                lab->celdas[i][j] == FINAL) {
                count++;
            }
        }
    }
    
    return count;
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
    if (longitud == 0 || camino == NULL) {
        printf("No hay solución para animar\n");
        return;
    }
    
    printf(COLOR_MAGENTA "\n=== ANIMANDO SOLUCIÓN ===\n" COLOR_RESET);
    
    for (int paso = 0; paso < longitud; paso++) {
        // Limpiar pantalla (ANSI escape code)
        printf("\033[2J\033[H");
        
        // Crear copia temporal con el camino hasta este paso
        int temp[FILAS][COLUMNAS];
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                temp[i][j] = lab->celdas[i][j];
            }
        }
        
        for (int i = 0; i <= paso; i++) {
            int nodo = camino[i];
            int fila = nodo / COLUMNAS;
            int col = nodo % COLUMNAS;
            
            if (temp[fila][col] != INICIO && temp[fila][col] != FINAL) {
                temp[fila][col] = SOLUCION;
            }
        }
        
        // Imprimir
        printf("Tamaño: %d x %d | Paso: %d/%d\n\n", lab->filas, lab->columnas, paso+1, longitud);
        
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
                switch (temp[i][j]) {
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
        
        // Pequeña pausa para la animación
        usleep(200000); // 200ms
    }
}