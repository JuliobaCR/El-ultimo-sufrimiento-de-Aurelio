/*
 * laberinto.c
 * Funciones para generación y visualización de laberintos.
 * Convierte grafos en representaciones visuales interactivas.
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

// ==================== CREACIÓN Y DESTRUCCIÓN ====================

/*
 * crear_laberinto_segun_tipo
 * Crea un laberinto usando el método especificado.
 * Asigna nombre aleatorio o personalizado.
 */
laberinto* crear_laberinto_segun_tipo(int tipo_generacion, const char* nombre) {
    laberinto* lab = (laberinto*)malloc(sizeof(laberinto));  // Asigna memoria
    if (!lab) return NULL;  // Verifica asignación
    
    // Configura dimensiones básicas
    lab->filas = FILAS;
    lab->columnas = COLUMNAS;
    lab->tiene_solucion = 0;
    lab->pasos_solucion = 0;
    
    // Asigna nombre al laberinto
    if (nombre == NULL || strlen(nombre) == 0) {
        obtener_nombre_aleatorio(lab->nombre);  // Nombre aleatorio
    } else {
        strncpy(lab->nombre, nombre, MAX_NOMBRE - 1);  // Nombre personalizado
        lab->nombre[MAX_NOMBRE - 1] = '\0';  // Asegura terminación
    }
    
    // Inicializa todas las celdas como paredes
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;  // Celda como pared
        }
    }
    
    // Genera según el tipo seleccionado
    switch (tipo_generacion) {
        case 1: generar_aleatorio(lab); break;              // Aleatorio simple
        case 2: generar_perfecto(lab); break;               // Perfecto (sin ciclos)
        case 3: generar_con_backtracking(lab); break;       // Backtracking
        case 4: generar_desde_grafo(lab); break;            // Desde grafo
        default: generar_aleatorio(lab); break;             // Por defecto
    }
    
    // Establece entrada y salida
    lab->celdas[0][0] = INICIO;                   // Entrada en (0,0)
    lab->celdas[FILAS-1][COLUMNAS-1] = FINAL;     // Salida en última celda
    
    // Asegura que entrada y salida estén conectadas
    if (lab->celdas[0][1] == PARED) lab->celdas[0][1] = CAMINO;      // Derecha de entrada
    if (lab->celdas[1][0] == PARED) lab->celdas[1][0] = CAMINO;      // Abajo de entrada
    if (lab->celdas[FILAS-1][COLUMNAS-2] == PARED) 
        lab->celdas[FILAS-1][COLUMNAS-2] = CAMINO; // Izquierda de salida
    if (lab->celdas[FILAS-2][COLUMNAS-1] == PARED) 
        lab->celdas[FILAS-2][COLUMNAS-1] = CAMINO; // Arriba de salida
    
    // Verifica si tiene solución
    lab->tiene_solucion = verificar_solucion(lab);
    
    // Calcula pasos de solución si existe
    if (lab->tiene_solucion) {
        int longitud;  // Longitud del camino
        int* camino = resolver_laberinto(lab, 1, &longitud);  // Usa Dijkstra
        if (camino && longitud > 0) {
            lab->pasos_solucion = longitud - 1;  // Pasos totales
        }
    }
    
    return lab;  // Retorna laberinto creado
}

/*
 * destruir_laberinto
 * Libera memoria de un laberinto.
 * Simple wrapper para free().
 */
void destruir_laberinto(laberinto* lab) {
    if (lab) {         // Si no es NULL
        free(lab);     // Libera memoria
    }
}

// ==================== GENERACIÓN DE LABERINTOS ====================

/*
 * generar_aleatorio
 * Genera laberinto usando método aleatorio simple.
 * Crea un camino principal (tronco) y múltiples ramas como un árbol.
 */
void generar_aleatorio(laberinto* lab) {
    srand(time(NULL));  // Inicializa generador aleatorio
    
    // Inicializa todas las celdas como paredes
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;  // Todo pared
        }
    }
    
    // ===== PASO 1: Crear tronco principal desde entrada a salida =====
    // Usaremos una lista para guardar el camino principal
    int camino_principal[FILAS * COLUMNAS][2];
    int longitud_principal = 0;
    
    int i = 0, j = 0;
    
    while (i < FILAS - 1 || j < COLUMNAS - 1) {
        // Guardar posición actual en el camino principal
        camino_principal[longitud_principal][0] = i;
        camino_principal[longitud_principal][1] = j;
        longitud_principal++;
        
        lab->celdas[i][j] = CAMINO;  // Marcar como camino
        
        // Decidir dirección con tendencia hacia la salida
        int movimientos_posibles[4][2] = {{1,0}, {0,1}, {-1,0}, {0,-1}};  // abajo, derecha, arriba, izquierda
        int mejores_movimientos[2];
        int num_mejores = 0;
        
        // Calcular distancia a la salida para cada posible movimiento
        for (int d = 0; d < 4; d++) {
            int ni = i + movimientos_posibles[d][0];
            int nj = j + movimientos_posibles[d][1];
            
            // Verificar límites y que no sea pared (para no pisar nuestro propio camino)
            if (ni >= 0 && ni < FILAS && nj >= 0 && nj < COLUMNAS && 
                lab->celdas[ni][nj] == PARED) {
                
                // Calcular distancia Manhattan a la salida
                int distancia = abs(ni - (FILAS-1)) + abs(nj - (COLUMNAS-1));
                
                // Los movimientos que nos acercan a la salida son mejores
                if (distancia < abs(i - (FILAS-1)) + abs(j - (COLUMNAS-1))) {
                    if (num_mejores < 2) {
                        mejores_movimientos[num_mejores] = d;
                        num_mejores++;
                    }
                }
            }
        }
        
        // Elegir movimiento
        if (num_mejores > 0) {
            // 80% de elegir un movimiento que nos acerque a la salida
            if (rand() % 100 < 80) {
                int d = mejores_movimientos[rand() % num_mejores];
                i += movimientos_posibles[d][0];
                j += movimientos_posibles[d][1];
            } else {
                // 20% de movimiento aleatorio
                int d = rand() % 4;
                i += movimientos_posibles[d][0];
                j += movimientos_posibles[d][1];
                // Asegurar que no salga de los límites
                if (i < 0) i = 0;
                if (i >= FILAS) i = FILAS - 1;
                if (j < 0) j = 0;
                if (j >= COLUMNAS) j = COLUMNAS - 1;
            }
        } else {
            // Si no hay movimientos buenos, moverse aleatoriamente
            int d;
            do {
                d = rand() % 4;
                int ni = i + movimientos_posibles[d][0];
                int nj = j + movimientos_posibles[d][1];
                if (ni >= 0 && ni < FILAS && nj >= 0 && nj < COLUMNAS) {
                    i = ni;
                    j = nj;
                    break;
                }
            } while (1);
        }
        
        // Evitar ciclos muy cortos
        if (longitud_principal > 10) {
            int misma_celda = 0;
            for (int k = longitud_principal - 10; k < longitud_principal - 1; k++) {
                if (camino_principal[k][0] == i && camino_principal[k][1] == j) {
                    misma_celda = 1;
                    break;
                }
            }
            if (misma_celda) {
                // Retroceder un poco
                i = camino_principal[longitud_principal - 5][0];
                j = camino_principal[longitud_principal - 5][1];
            }
        }
    }
    
    // Agregar la última celda (salida)
    camino_principal[longitud_principal][0] = i;
    camino_principal[longitud_principal][1] = j;
    longitud_principal++;
    lab->celdas[i][j] = CAMINO;
    
    // ===== PASO 2: Crear ramas desde el tronco principal =====
    int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};  // arriba, abajo, izquierda, derecha
    
    // Crear ramas desde cada celda del camino principal (excepto extremos)
    for (int idx = 1; idx < longitud_principal - 1; idx++) {
        int f = camino_principal[idx][0];
        int c = camino_principal[idx][1];
        
        // 30% de probabilidad de crear una rama desde esta celda
        if (rand() % 100 < 30) {
            // Elegir dirección para la rama (no puede ser la dirección del tronco)
            int direcciones_posibles[4];
            int num_direcciones = 0;
            
            // Encontrar dirección del tronco en este punto
            int dir_tronco = -1;
            if (idx > 0) {
                int f_prev = camino_principal[idx-1][0];
                int c_prev = camino_principal[idx-1][1];
                for (int d = 0; d < 4; d++) {
                    if (f + dirs[d][0] == f_prev && c + dirs[d][1] == c_prev) {
                        dir_tronco = d;
                        break;
                    }
                }
            }
            
            // Todas las direcciones excepto la del tronco (si se conoce)
            for (int d = 0; d < 4; d++) {
                if (d != dir_tronco) {
                    direcciones_posibles[num_direcciones] = d;
                    num_direcciones++;
                }
            }
            
            if (num_direcciones > 0) {
                int dir_rama = direcciones_posibles[rand() % num_direcciones];
                
                // Crear rama de longitud aleatoria (2-5 celdas)
                int longitud_rama = 2 + rand() % 4;
                int rama_f[10], rama_c[10];  // Máximo 10 celdas por rama
                int longitud_rama_real = 0;
                
                int rf = f;
                int rc = c;
                
                for (int paso = 0; paso < longitud_rama; paso++) {
                    rf += dirs[dir_rama][0];
                    rc += dirs[dir_rama][1];
                    
                    // Verificar límites y que sea pared
                    if (rf < 0 || rf >= FILAS || rc < 0 || rc >= COLUMNAS || 
                        lab->celdas[rf][rc] != PARED) {
                        break;
                    }
                    
                    // Verificar que no toque otro camino (excepto el punto de origen)
                    int toca_otro_camino = 0;
                    for (int d = 0; d < 4; d++) {
                        int nf = rf + dirs[d][0];
                        int nc = rc + dirs[d][1];
                        
                        if (nf >= 0 && nf < FILAS && nc >= 0 && nc < COLUMNAS) {
                            // No contar el punto de origen de la rama
                            if (!(nf == f && nc == c)) {
                                if (lab->celdas[nf][nc] == CAMINO) {
                                    toca_otro_camino = 1;
                                    break;
                                }
                            }
                        }
                    }
                    
                    if (toca_otro_camino) {
                        break;
                    }
                    
                    // Guardar celda de la rama
                    rama_f[longitud_rama_real] = rf;
                    rama_c[longitud_rama_real] = rc;
                    longitud_rama_real++;
                }
                
                // Dibujar la rama si tiene al menos 1 celda
                if (longitud_rama_real > 0) {
                    // 80% de probabilidad de dibujar la rama completa
                    if (rand() % 100 < 80) {
                        for (int r = 0; r < longitud_rama_real; r++) {
                            lab->celdas[rama_f[r]][rama_c[r]] = CAMINO;
                            
                            // 20% de probabilidad de crear sub-ramas desde esta rama
                            if (rand() % 100 < 20 && r < longitud_rama_real - 1) {
                                // Crear una sub-rama desde este punto
                                int subdirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
                                int subdir = rand() % 4;
                                
                                // No puede ser la dirección opuesta a la rama principal
                                if (!(subdirs[subdir][0] == -dirs[dir_rama][0] && 
                                      subdirs[subdir][1] == -dirs[dir_rama][1])) {
                                    
                                    int sublongitud = 1 + rand() % 3;
                                    int srf = rama_f[r];
                                    int src = rama_c[r];
                                    
                                    for (int sp = 0; sp < sublongitud; sp++) {
                                        srf += subdirs[subdir][0];
                                        src += subdirs[subdir][1];
                                        
                                        if (srf < 0 || srf >= FILAS || src < 0 || src >= COLUMNAS || 
                                            lab->celdas[srf][src] != PARED) {
                                            break;
                                        }
                                        
                                        // Verificar que no toque otro camino
                                        int toca = 0;
                                        for (int d = 0; d < 4; d++) {
                                            int nf = srf + dirs[d][0];
                                            int nc = src + dirs[d][1];
                                            
                                            if (nf >= 0 && nf < FILAS && nc >= 0 && nc < COLUMNAS) {
                                                // No contar el punto de origen
                                                if (!(nf == rama_f[r] && nc == rama_c[r])) {
                                                    if (lab->celdas[nf][nc] == CAMINO) {
                                                        toca = 1;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                        
                                        if (toca) {
                                            break;
                                        }
                                        
                                        lab->celdas[srf][src] = CAMINO;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // ===== PASO 3: Crear algunos caminos cortos adicionales (hormigas perdidas) =====
    for (int h = 0; h < FILAS * COLUMNAS / 20; h++) {  // 5% del total
        // Buscar una celda que sea camino
        int intentos = 0;
        int f, c;
        
        do {
            f = rand() % FILAS;
            c = rand() % COLUMNAS;
            intentos++;
        } while (lab->celdas[f][c] != CAMINO && intentos < 100);
        
        if (lab->celdas[f][c] == CAMINO) {
            // Intentar crear un pequeño camino desde aquí
            int dir = rand() % 4;
            int longitud = 1 + rand() % 2;  // Muy corto (1-2 celdas)
            
            int nf = f;
            int nc = c;
            
            for (int p = 0; p < longitud; p++) {
                nf += dirs[dir][0];
                nc += dirs[dir][1];
                
                if (nf < 0 || nf >= FILAS || nc < 0 || nc >= COLUMNAS || 
                    lab->celdas[nf][nc] != PARED) {
                    break;
                }
                
                // Verificar que no toque otro camino (excepto el origen)
                int toca = 0;
                for (int d = 0; d < 4; d++) {
                    int vf = nf + dirs[d][0];
                    int vc = nc + dirs[d][1];
                    
                    if (vf >= 0 && vf < FILAS && vc >= 0 && vc < COLUMNAS) {
                        if (!(vf == f && vc == c)) {
                            if (lab->celdas[vf][vc] == CAMINO) {
                                toca = 1;
                                break;
                            }
                        }
                    }
                }
                
                if (toca) {
                    break;
                }
                
                lab->celdas[nf][nc] = CAMINO;
            }
        }
    }
    
    // ===== PASO 4: Asegurar conectividad básica =====
    // Verificar que no haya celdas camino aisladas
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLUMNAS; c++) {
            if (lab->celdas[f][c] == CAMINO) {
                // No verificar entrada y salida
                if ((f == 0 && c == 0) || (f == FILAS-1 && c == COLUMNAS-1)) {
                    continue;
                }
                
                int vecinos = 0;
                if (f > 0 && lab->celdas[f-1][c] == CAMINO) vecinos++;
                if (f < FILAS-1 && lab->celdas[f+1][c] == CAMINO) vecinos++;
                if (c > 0 && lab->celdas[f][c-1] == CAMINO) vecinos++;
                if (c < COLUMNAS-1 && lab->celdas[f][c+1] == CAMINO) vecinos++;
                
                if (vecinos == 0) {
                    // Conectar con el vecino más cercano
                    if (f > 0) lab->celdas[f-1][c] = CAMINO;
                    else if (f < FILAS-1) lab->celdas[f+1][c] = CAMINO;
                    else if (c > 0) lab->celdas[f][c-1] = CAMINO;
                    else if (c < COLUMNAS-1) lab->celdas[f][c+1] = CAMINO;
                }
            }
        }
    }
}


/*
 * generar_desde_grafo
 * Genera laberinto usando algoritmo de Prim para grafos aleatorios.
 * Crea laberinto con múltiples caminos posibles y algunos ciclos.
 */
void generar_desde_grafo(laberinto* lab) {
    srand(time(NULL));  // Inicializa generador
    
    // Inicializa todo como paredes
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;  // Pared por defecto
        }
    }
    
    // Crear estructura para grafo
    int visitado[FILAS][COLUMNAS] = {0};  // Matriz de visitados
    
    // Usar celdas impares como nodos del grafo (como en perfecto)
    for (int i = 1; i < FILAS-1; i += 2) {
        for (int j = 1; j < COLUMNAS-1; j += 2) {
            lab->celdas[i][j] = CAMINO;  // Celda de camino (nodo del grafo)
        }
    }
    
    // Lista de paredes (aristas del grafo)
    int paredes[FILAS * COLUMNAS * 4][3];  // [fila, columna, dirección]
    int num_paredes = 0;
    
    // Inicializar con paredes entre nodos
    for (int i = 1; i < FILAS-1; i += 2) {
        for (int j = 1; j < COLUMNAS-1; j += 2) {
            // Pared derecha
            if (j + 2 < COLUMNAS-1) {
                paredes[num_paredes][0] = i;
                paredes[num_paredes][1] = j + 1;
                paredes[num_paredes][2] = 0;  // 0 = derecha
                num_paredes++;
            }
            // Pared abajo
            if (i + 2 < FILAS-1) {
                paredes[num_paredes][0] = i + 1;
                paredes[num_paredes][1] = j;
                paredes[num_paredes][2] = 1;  // 1 = abajo
                num_paredes++;
            }
        }
    }
    
    // Barajar las paredes aleatoriamente
    for (int i = 0; i < num_paredes; i++) {
        int j = rand() % num_paredes;
        int temp[3];
        memcpy(temp, paredes[i], sizeof(temp));
        memcpy(paredes[i], paredes[j], sizeof(temp));
        memcpy(paredes[j], temp, sizeof(temp));
    }
    
    // Algoritmo de Kruskal simplificado para grafos aleatorios
    int conjunto[FILAS][COLUMNAS];  // Para unir conjuntos
    int siguiente_conjunto = 1;
    
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            conjunto[i][j] = 0;
        }
    }
    
    // Asignar conjuntos iniciales a los nodos
    for (int i = 1; i < FILAS-1; i += 2) {
        for (int j = 1; j < COLUMNAS-1; j += 2) {
            conjunto[i][j] = siguiente_conjunto++;
        }
    }
    
    // Procesar paredes en orden aleatorio
    for (int p = 0; p < num_paredes; p++) {
        int fila_pared = paredes[p][0];
        int col_pared = paredes[p][1];
        int direccion = paredes[p][2];
        
        int nodo1_fila, nodo1_col, nodo2_fila, nodo2_col;
        
        if (direccion == 0) {  // Pared vertical (derecha)
            nodo1_fila = fila_pared;
            nodo1_col = col_pared - 1;
            nodo2_fila = fila_pared;
            nodo2_col = col_pared + 1;
        } else {  // Pared horizontal (abajo)
            nodo1_fila = fila_pared - 1;
            nodo1_col = col_pared;
            nodo2_fila = fila_pared + 1;
            nodo2_col = col_pared;
        }
        
        // Verificar que ambos nodos existen
        if (nodo1_fila >= 1 && nodo1_fila < FILAS-1 && nodo1_col >= 1 && nodo1_col < COLUMNAS-1 &&
            nodo2_fila >= 1 && nodo2_fila < FILAS-1 && nodo2_col >= 1 && nodo2_col < COLUMNAS-1) {
            
            // Si están en conjuntos diferentes, unirlos
            if (conjunto[nodo1_fila][nodo1_col] != conjunto[nodo2_fila][nodo2_col]) {
                // Quitar la pared
                lab->celdas[fila_pared][col_pared] = CAMINO;
                
                // Unir conjuntos
                int conjunto_viejo = conjunto[nodo2_fila][nodo2_col];
                int conjunto_nuevo = conjunto[nodo1_fila][nodo1_col];
                
                for (int i = 1; i < FILAS-1; i += 2) {
                    for (int j = 1; j < COLUMNAS-1; j += 2) {
                        if (conjunto[i][j] == conjunto_viejo) {
                            conjunto[i][j] = conjunto_nuevo;
                        }
                    }
                }
            } else {
                // Ocasionalmente permitir ciclos (30% de probabilidad)
                if (rand() % 100 < 30) {
                    lab->celdas[fila_pared][col_pared] = CAMINO;
                }
            }
        }
    }
    
    // Conectar entrada y salida al grafo
    // Conectar entrada (0,0)
    if (lab->celdas[1][0] == PARED && lab->celdas[0][1] == PARED) {
        lab->celdas[1][0] = CAMINO;  // Conectar hacia abajo
    }
    
    // Conectar salida (FILAS-1, COLUMNAS-1)
    if (lab->celdas[FILAS-2][COLUMNAS-1] == PARED && lab->celdas[FILAS-1][COLUMNAS-2] == PARED) {
        lab->celdas[FILAS-1][COLUMNAS-2] = CAMINO;  // Conectar hacia izquierda
    }
    
    // Añadir algunos caminos adicionales aleatorios para más complejidad
    for (int extra = 0; extra < FILAS * COLUMNAS / 10; extra++) {
        int f = 1 + 2 * (rand() % ((FILAS-2)/2));
        int c = 1 + 2 * (rand() % ((COLUMNAS-2)/2));
        
        // Añadir camino en una dirección aleatoria si es posible
        int dirs[4][2] = {{-2,0}, {2,0}, {0,-2}, {0,2}};
        int dir = rand() % 4;
        int nf = f + dirs[dir][0];
        int nc = c + dirs[dir][1];
        
        if (nf >= 1 && nf < FILAS-1 && nc >= 1 && nc < COLUMNAS-1) {
            // Verificar si ya hay conexión
            int pared_fila = f + dirs[dir][0]/2;
            int pared_col = c + dirs[dir][1]/2;
            
            if (lab->celdas[pared_fila][pared_col] == PARED) {
                // 50% de probabilidad de añadir este camino extra
                if (rand() % 100 < 50) {
                    lab->celdas[pared_fila][pared_col] = CAMINO;
                }
            }
        }
    }
}

/*
 * generar_perfecto
 * Genera laberinto perfecto usando DFS.
 * Garantiza un único camino entre cualquier par de nodos.
 */
void generar_perfecto(laberinto* lab) {
    srand(time(NULL));  // Inicializa generador
    
    // Inicializa todo como paredes
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;  // Pared por defecto
        }
    }
    
    // Marca celdas en posiciones impares como caminos potenciales
    for (int i = 1; i < FILAS-1; i += 2) {
        for (int j = 1; j < COLUMNAS-1; j += 2) {
            lab->celdas[i][j] = CAMINO;  // Celda de camino
        }
    }
    
    int pila[FILAS * COLUMNAS][2];  // Pila para DFS
    int tope = 0;                   // Índice del tope
    
    // Empieza desde (1,1)
    int fila = 1;
    int col = 1;
    pila[tope][0] = fila;  // Guarda fila
    pila[tope][1] = col;   // Guarda columna
    tope++;                // Incrementa tope
    
    int visitado[FILAS][COLUMNAS] = {0};  // Matriz de visitados
    visitado[fila][col] = 1;              // Marca como visitado
    
    int direcciones[4][2] = {{0, 2}, {2, 0}, {0, -2}, {-2, 0}};  // Movimientos (2 pasos)
    
    while (tope > 0) {  // Mientras haya celdas en pila
        fila = pila[tope-1][0];  // Obtiene fila del tope
        col = pila[tope-1][1];   // Obtiene columna del tope
        
        // Encuentra vecinos no visitados
        int vecinos[4];
        int num_vecinos = 0;
        
        for (int d = 0; d < 4; d++) {
            int nf = fila + direcciones[d][0];  // Nueva fila
            int nc = col + direcciones[d][1];   // Nueva columna
            
            // Verifica límites y si no está visitado
            if (nf >= 1 && nf < FILAS-1 && nc >= 1 && nc < COLUMNAS-1 &&
                lab->celdas[nf][nc] == CAMINO && !visitado[nf][nc]) {
                vecinos[num_vecinos++] = d;  // Agrega dirección
            }
        }
        
        if (num_vecinos > 0) {  // Si hay vecinos disponibles
            int dir = vecinos[rand() % num_vecinos];  // Dirección aleatoria
            int nf = fila + direcciones[dir][0];      // Nueva fila
            int nc = col + direcciones[dir][1];       // Nueva columna
            
            // Quita la pared entre las celdas
            lab->celdas[fila + direcciones[dir][0]/2][col + direcciones[dir][1]/2] = CAMINO;
            
            visitado[nf][nc] = 1;          // Marca como visitado
            pila[tope][0] = nf;            // Guarda nueva fila
            pila[tope][1] = nc;            // Guarda nueva columna
            tope++;                        // Incrementa tope
        } else {
            tope--;  // Backtrack: retrocede
        }
    }
}

/*
 * generar_con_backtracking
 * Genera laberinto usando backtracking recursivo.
 * Implementación clásica de generación de laberintos.
 */
void generar_con_backtracking(laberinto* lab) {
    srand(time(NULL));  // Inicializa generador
    
    // Inicializa todo como paredes
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;  // Pared por defecto
        }
    }
    
    // Calcula dimensiones internas (celdas impares)
    int filas_interiores = (FILAS - 1) / 2;
    int cols_interiores = (COLUMNAS - 1) / 2;
    
    int maze[filas_interiores][cols_interiores];  // Laberinto interno
    
    // Inicializa laberinto interno
    for (int i = 0; i < filas_interiores; i++) {
        for (int j = 0; j < cols_interiores; j++) {
            maze[i][j] = 0;  // 0 = no visitado
        }
    }
    
    int pila[filas_interiores * cols_interiores][2];  // Pila para backtracking
    int tope = 0;                                     // Índice del tope
    
    // Empieza desde (0,0)
    int x = 0, y = 0;
    maze[x][y] = 1;            // Marca como visitado
    pila[tope][0] = x;         // Guarda x
    pila[tope][1] = y;         // Guarda y
    tope++;                    // Incrementa tope
    
    int direcciones[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};  // 4 direcciones
    
    while (tope > 0) {  // Mientras haya celdas en pila
        x = pila[tope-1][0];  // Obtiene x del tope
        y = pila[tope-1][1];  // Obtiene y del tope
        
        // Encuentra vecinos no visitados
        int vecinos[4];
        int num_vecinos = 0;
        
        for (int d = 0; d < 4; d++) {
            int nx = x + direcciones[d][0];  // Nuevo x
            int ny = y + direcciones[d][1];  // Nuevo y
            
            // Verifica límites y si no está visitado
            if (nx >= 0 && nx < filas_interiores && ny >= 0 && ny < cols_interiores &&
                maze[nx][ny] == 0) {
                vecinos[num_vecinos++] = d;  // Agrega dirección
            }
        }
        
        if (num_vecinos > 0) {  // Si hay vecinos disponibles
            int dir = vecinos[rand() % num_vecinos];  // Dirección aleatoria
            int nx = x + direcciones[dir][0];         // Nuevo x
            int ny = y + direcciones[dir][1];         // Nuevo y
            
            maze[nx][ny] = 1;  // Marca como visitado
            
            // Convierte coordenadas internas a reales
            int real_x1 = x * 2 + 1;
            int real_y1 = y * 2 + 1;
            int real_x2 = nx * 2 + 1;
            int real_y2 = ny * 2 + 1;
            
            // Crea caminos en el laberinto real
            lab->celdas[real_x1][real_y1] = CAMINO;                         // Celda actual
            lab->celdas[real_x2][real_y2] = CAMINO;                         // Celda vecina
            lab->celdas[(real_x1 + real_x2) / 2][(real_y1 + real_y2) / 2] = CAMINO;  // Pared entre
            
            pila[tope][0] = nx;  // Guarda nuevo x
            pila[tope][1] = ny;  // Guarda nuevo y
            tope++;              // Incrementa tope
        } else {
            tope--;  // Backtrack: retrocede
        }
    }
    
    // Copia caminos del laberinto interno al real
    for (int i = 0; i < filas_interiores; i++) {
        for (int j = 0; j < cols_interiores; j++) {
            if (maze[i][j] == 1) {
                lab->celdas[i * 2 + 1][j * 2 + 1] = CAMINO;  // Convierte a camino real
            }
        }
    }
}



// ==================== VISUALIZACIÓN ====================

/*
 * imprimir_laberinto
 * Muestra el laberinto en formato gráfico.
 * Usa caracteres especiales y colores ANSI.
 */
void imprimir_laberinto(laberinto* lab) {
    printf(COLOR_CYAN "\n=== LABERINTO: %s ===\n" COLOR_RESET, lab->nombre);
    printf("Tamaño: %d x %d | Solución: %s | Pasos solución: %d\n\n", 
           lab->filas, lab->columnas,
           lab->tiene_solucion ? "SÍ" : "NO",
           lab->pasos_solucion);
    
    // Leyenda de símbolos
    printf(COLOR_VERDE "E" COLOR_RESET "=Entrada  ");
    printf(COLOR_ROJO "S" COLOR_RESET "=Salida  ");
    printf("  =Camino  ");
    printf(COLOR_BLANCO "*️⃣" COLOR_RESET "=Pared\n\n");
    
    // Borde superior
    printf("  ");
    for (int j = 0; j < lab->columnas; j++) {
        printf(COLOR_BLANCO "*️⃣*️⃣" COLOR_RESET);  // Dos caracteres por celda
    }
    printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);
    
    // Contenido del laberinto
    for (int i = 0; i < lab->filas; i++) {
        printf("  " COLOR_BLANCO "*️⃣" COLOR_RESET);  // Borde izquierdo
        
        for (int j = 0; j < lab->columnas; j++) {
            // Selecciona símbolo según tipo de celda
            switch (lab->celdas[i][j]) {
                case INICIO:
                    printf(COLOR_VERDE "E " COLOR_RESET);  // Entrada (verde)
                    break;
                case FINAL:
                    printf(COLOR_ROJO "S " COLOR_RESET);   // Salida (rojo)
                    break;
                case CAMINO:
                    printf("  ");                         // Camino (espacio)
                    break;
                case PARED:
                    printf(COLOR_BLANCO "*️⃣ " COLOR_RESET); // Pared (blanco)
                    break;
                case VISITADO:
                    printf(COLOR_AMARILLO ". " COLOR_RESET); // Visitado (amarillo)
                    break;
                case SOLUCION:
                    printf(COLOR_VERDE "· " COLOR_RESET);   // Solución (verde)
                    break;
                default:
                    printf("  ");                         // Por defecto
                    break;
            }
        }
        
        printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);  // Borde derecho
    }
    
    // Borde inferior
    printf("  ");
    for (int j = 0; j < lab->columnas; j++) {
        printf(COLOR_BLANCO "*️⃣*️⃣" COLOR_RESET);  // Dos caracteres por celda
    }
    printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);
}

/*
 * imprimir_laberinto_con_solucion
 * Muestra laberinto con camino solución resaltado.
 * Copia temporalmente el laberinto para marcar solución.
 */
void imprimir_laberinto_con_solucion(laberinto* lab, int* camino, int longitud) {
    printf(COLOR_CYAN "\n=== LABERINTO CON SOLUCIÓN ===\n" COLOR_RESET);
    
    int temp[FILAS][COLUMNAS];  // Matriz temporal
    
    // Copia el laberinto original
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            temp[i][j] = lab->celdas[i][j];  // Copia valor
        }
    }
    
    // Marca el camino de solución
    for (int i = 0; i < longitud; i++) {
        int nodo = camino[i];                     // Nodo del camino
        int fila = nodo / COLUMNAS;               // Calcula fila
        int col = nodo % COLUMNAS;                // Calcula columna
        
        // Marca como solución (excepto entrada/salida)
        if (temp[fila][col] != INICIO && temp[fila][col] != FINAL) {
            temp[fila][col] = SOLUCION;  // Celda de solución
        }
    }
    
    printf("Tamaño: %d x %d\n\n", lab->filas, lab->columnas);
    
    // Leyenda de símbolos
    printf(COLOR_VERDE "E" COLOR_RESET "=Entrada  ");
    printf(COLOR_ROJO "S" COLOR_RESET "=Salida  ");
    printf(COLOR_VERDE "·" COLOR_RESET "=Solución  ");
    printf(COLOR_BLANCO "*️⃣" COLOR_RESET "=Pared\n\n");
    
    // Borde superior
    printf("  ");
    for (int j = 0; j < lab->columnas; j++) {
        printf(COLOR_BLANCO "*️⃣*️⃣" COLOR_RESET);  // Dos caracteres por celda
    }
    printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);
    
    // Contenido del laberinto con solución
    for (int i = 0; i < lab->filas; i++) {
        printf("  " COLOR_BLANCO "*️⃣" COLOR_RESET);  // Borde izquierdo
        
        for (int j = 0; j < lab->columnas; j++) {
            // Selecciona símbolo según tipo de celda
            switch (temp[i][j]) {
                case INICIO:
                    printf(COLOR_VERDE "E " COLOR_RESET);  // Entrada
                    break;
                case FINAL:
                    printf(COLOR_ROJO "S " COLOR_RESET);   // Salida
                    break;
                case CAMINO:
                    printf("  ");                         // Camino normal
                    break;
                case PARED:
                    printf(COLOR_BLANCO "*️⃣ " COLOR_RESET); // Pared
                    break;
                case VISITADO:
                    printf(COLOR_AMARILLO ". " COLOR_RESET); // Visitado
                    break;
                case SOLUCION:
                    printf(COLOR_VERDE "· " COLOR_RESET);   // Solución
                    break;
                default:
                    printf("  ");                         // Por defecto
                    break;
            }
        }
        
        printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);  // Borde derecho
    }
    
    // Borde inferior
    printf("  ");
    for (int j = 0; j < lab->columnas; j++) {
        printf(COLOR_BLANCO "*️⃣*️⃣" COLOR_RESET);  // Dos caracteres por celda
    }
    printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);
    
    printf("\nLongitud del camino solución: %d pasos\n", longitud - 1);
}

// ==================== RESOLUCIÓN ====================

/*
 * resolver_laberinto
 * Convierte laberinto a grafo y aplica algoritmo de resolución.
 * Soporta Dijkstra y BFS para encontrar caminos.
 */
int* resolver_laberinto(laberinto* lab, int algoritmo, int* longitud) {
    grafo_matriz grafo;  // Grafo para representación
    inicializar_grafo_matriz(&grafo, FILAS * COLUMNAS);  // Inicializa grafo
    
    // Convierte laberinto a grafo (conexiones entre celdas adyacentes)
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (lab->celdas[i][j] != PARED) {  // Si no es pared
                int nodo = i * COLUMNAS + j;   // Índice del nodo
                
                // Conexión con vecino arriba
                if (i > 0 && lab->celdas[i-1][j] != PARED) {
                    agregar_arista_matriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                }
                // Conexión con vecino izquierda
                if (j > 0 && lab->celdas[i][j-1] != PARED) {
                    agregar_arista_matriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                }
                // Conexión con vecino abajo
                if (i < FILAS-1 && lab->celdas[i+1][j] != PARED) {
                    agregar_arista_matriz(&grafo, nodo, (i+1)*COLUMNAS + j, 1);
                }
                // Conexión con vecino derecha
                if (j < COLUMNAS-1 && lab->celdas[i][j+1] != PARED) {
                    agregar_arista_matriz(&grafo, nodo, i*COLUMNAS + (j+1), 1);
                }
            }
        }
    }
    
    int inicio = 0;                          // Entrada en (0,0)
    int fin = FILAS * COLUMNAS - 1;          // Salida en última posición
    static int camino[MAX_NODOS];            // Array para camino
    *longitud = 0;                           // Inicializa longitud
    
    switch (algoritmo) {
        case 1: { // Dijkstra
            int distancia;  // Distancia total
            int* anterior = dijkstra_con_estados(&grafo, inicio, fin, &distancia, 0);
            
            if (distancia == -1 || anterior == NULL) {  // Si no hay camino
                printf("No hay camino desde inicio a fin\n");
                break;
            }
            
            // Reconstruye camino desde fin hasta inicio
            int nodo = fin;
            while (nodo != -1) {
                camino[*longitud] = nodo;  // Agrega nodo al camino
                (*longitud)++;             // Incrementa longitud
                nodo = anterior[nodo];     // Retrocede al anterior
            }
            
            // Invierte el camino (fin→inicio a inicio→fin)
            for (int i = 0; i < *longitud / 2; i++) {
                int temp = camino[i];  // Guarda temporal
                camino[i] = camino[*longitud - i - 1];  // Intercambia
                camino[*longitud - i - 1] = temp;       // Completa intercambio
            }
            break;
        }
        
        case 2: { // BFS
            int* anterior = bfs(&grafo, inicio);  // BFS desde inicio
            
            if (anterior == NULL) {  // Si error en BFS
                printf("Error en BFS\n");
                break;
            }
            
            // Reconstruye camino desde fin hasta inicio
            int nodo = fin;
            while (nodo != -1 && anterior[nodo] != -1) {
                camino[*longitud] = nodo;  // Agrega nodo al camino
                (*longitud)++;             // Incrementa longitud
                nodo = anterior[nodo];     // Retrocede al anterior
            }
            
            // Agrega el nodo inicial si se encontró camino
            if (*longitud > 0) {
                camino[*longitud] = inicio;  // Agrega inicio
                (*longitud)++;               // Incrementa longitud
            }
            
            if (*longitud == 0) {  // Si no se encontró camino
                printf("No hay camino desde inicio a fin\n");
                break;
            }
            
            // Invierte el camino
            for (int i = 0; i < *longitud / 2; i++) {
                int temp = camino[i];  // Guarda temporal
                camino[i] = camino[*longitud - i - 1];  // Intercambia
                camino[*longitud - i - 1] = temp;       // Completa intercambio
            }
            break;
        }
        
        default:
            printf("Algoritmo no válido\n");  // Algoritmo no soportado
            *longitud = 0;                    // Longitud cero
            break;
    }
    
    liberar_grafo_matriz(&grafo);  // Libera grafo
    
    if (*longitud > 0) {
        return camino;  // Retorna camino si existe
    } else {
        return NULL;    // Retorna NULL si no hay camino
    }
}

/*
 * verificar_solucion
 * Verifica si el laberinto tiene solución.
 * Usa Dijkstra para encontrar camino.
 */
int verificar_solucion(laberinto* lab) {
    int longitud;  // Longitud del camino
    int* camino = resolver_laberinto(lab, 1, &longitud);  // Usa Dijkstra
    return (camino != NULL && longitud > 0);  // True si tiene solución
}

/*
 * contar_caminos_posibles
 * Cuenta celdas transitables en el laberinto.
 * Incluye entrada, salida y caminos.
 */
int contar_caminos_posibles(laberinto* lab) {
    int count = 0;  // Contador de celdas transitables
    
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            // Si es transitable (camino, entrada o salida)
            if (lab->celdas[i][j] == CAMINO || 
                lab->celdas[i][j] == INICIO || 
                lab->celdas[i][j] == FINAL) {
                count++;  // Incrementa contador
            }
        }
    }
    
    return count;  // Retorna total
}

// ==================== FUNCIONES AUXILIARES ====================

/*
 * obtener_nombre_aleatorio
 * Asigna nombre aleatorio del array de memes.
 * Usa índice aleatorio para seleccionar nombre.
 */
int obtener_nombre_aleatorio(char* buffer) {
    srand(time(NULL));                    // Inicializa generador
    int indice = rand() % NUM_MEMES;      // Índice aleatorio
    strcpy(buffer, memes_anarquismo[indice]);  // Copia nombre
    return indice;                        // Retorna índice
}

/*
 * convertir_coordenadas
 * Convierte índice de nodo a coordenadas (fila, columna).
 * Útil para algoritmos que trabajan con índices.
 */
void convertir_coordenadas(int nodo, int* fila, int* columna) {
    *fila = nodo / COLUMNAS;     // División entera para fila
    *columna = nodo % COLUMNAS;  // Residuo para columna
}

/*
 * convertir_a_nodo
 * Convierte coordenadas (fila, columna) a índice de nodo.
 * Fórmula inversa a convertir_coordenadas.
 */
int convertir_a_nodo(int fila, int columna) {
    return fila * COLUMNAS + columna;  // Fórmula lineal
}

/*
 * es_valida
 * Verifica si coordenadas están dentro del laberinto.
 * Útil para validar movimientos.
 */
int es_valida(int fila, int columna) {
    return (fila >= 0 && fila < FILAS &&   // Fila válida
            columna >= 0 && columna < COLUMNAS);  // Columna válida
}

/*
 * animar_solucion
 * Muestra animación paso a paso del camino solución.
 * Limpia pantalla entre cada paso para efecto animado.
 */
void animar_solucion(laberinto* lab, int* camino, int longitud) {
    if (longitud == 0 || camino == NULL) {  // Si no hay solución
        printf("No hay solución para animar\n");
        return;
    }
    
    printf(COLOR_MAGENTA "\n=== ANIMANDO SOLUCIÓN ===\n" COLOR_RESET);
    
    for (int paso = 0; paso < longitud; paso++) {
        printf("\033[2J\033[H");  // Limpia pantalla (ANSI)
        
        int temp[FILAS][COLUMNAS];  // Matriz temporal
        
        // Copia el laberinto original
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                temp[i][j] = lab->celdas[i][j];  // Copia valor
            }
        }
        
        // Marca camino hasta este paso
        for (int i = 0; i <= paso; i++) {
            int nodo = camino[i];          // Nodo del camino
            int fila = nodo / COLUMNAS;    // Calcula fila
            int col = nodo % COLUMNAS;     // Calcula columna
            
            // Marca como solución (excepto entrada/salida)
            if (temp[fila][col] != INICIO && temp[fila][col] != FINAL) {
                temp[fila][col] = SOLUCION;  // Celda de solución
            }
        }
        
        printf("Tamaño: %d x %d | Paso: %d/%d\n\n", 
               lab->filas, lab->columnas, paso+1, longitud);
        
        // Borde superior
        printf("  ");
        for (int j = 0; j < lab->columnas; j++) {
            printf(COLOR_BLANCO "*️⃣*️⃣" COLOR_RESET);  // Dos caracteres por celda
        }
        printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);
        
        // Contenido del laberinto
        for (int i = 0; i < lab->filas; i++) {
            printf("  " COLOR_BLANCO "*️⃣" COLOR_RESET);  // Borde izquierdo
            
            for (int j = 0; j < lab->columnas; j++) {
                // Selecciona símbolo según tipo de celda
                switch (temp[i][j]) {
                    case INICIO:
                        printf(COLOR_VERDE "E " COLOR_RESET);  // Entrada
                        break;
                    case FINAL:
                        printf(COLOR_ROJO "S " COLOR_RESET);   // Salida
                        break;
                    case CAMINO:
                        printf("  ");                         // Camino
                        break;
                    case PARED:
                        printf(COLOR_BLANCO "*️⃣ " COLOR_RESET); // Pared
                        break;
                    case SOLUCION:
                        printf(COLOR_VERDE "· " COLOR_RESET);   // Solución
                        break;
                    default:
                        printf("  ");                         // Por defecto
                        break;
                }
            }
            
            printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);  // Borde derecho
        }
        
        // Borde inferior
        printf("  ");
        for (int j = 0; j < lab->columnas; j++) {
            printf(COLOR_BLANCO "*️⃣*️⃣" COLOR_RESET);  // Dos caracteres por celda
        }
        printf(COLOR_BLANCO "*️⃣\n" COLOR_RESET);
        
        #ifdef _WIN32
            Sleep(200);  // 200ms para Windows
        #else
            sleep(200000);  // 200ms para Unix/Linux (200,000 microsegundos)
        #endif
    }
}
