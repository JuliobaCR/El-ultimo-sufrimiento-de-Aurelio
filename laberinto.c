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
 * Crea camino principal y luego añade ramificaciones.
 */
void generar_aleatorio(laberinto* lab) {
    srand(time(NULL));  // Inicializa generador aleatorio
    
    // Inicializa todas las celdas como paredes
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;  // Todo pared
        }
    }
    
    int i = 0, j = 0;  // Coordenadas actuales
    
    // Crea camino principal de entrada a salida
    while (i < FILAS - 1 || j < COLUMNAS - 1) {
        lab->celdas[i][j] = CAMINO;  // Marca como camino
        
        // Decide dirección (abajo o derecha)
        if (i < FILAS - 1 && j < COLUMNAS - 1) {
            if (rand() % 2 == 0) {  // 50% probabilidad
                i++;  // Abajo
            } else {
                j++;  // Derecha
            }
        } else if (i < FILAS - 1) {  // Solo puede ir abajo
            i++;
        } else {                     // Solo puede ir derecha
            j++;
        }
    }
    lab->celdas[FILAS-1][COLUMNAS-1] = CAMINO;  // Última celda
    
    // Añade caminos secundarios aleatorios
    for (int k = 0; k < FILAS * COLUMNAS / 3; k++) {
        int f = rand() % FILAS;     // Fila aleatoria
        int c = rand() % COLUMNAS;  // Columna aleatoria
        
        // Si es pared y está junto a un camino
        if (lab->celdas[f][c] == PARED) {
            if ((f > 0 && lab->celdas[f-1][c] == CAMINO) ||
                (f < FILAS-1 && lab->celdas[f+1][c] == CAMINO) ||
                (c > 0 && lab->celdas[f][c-1] == CAMINO) ||
                (c < COLUMNAS-1 && lab->celdas[f][c+1] == CAMINO)) {
                lab->celdas[f][c] = CAMINO;  // Convierte a camino
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

/*
 * generar_desde_grafo
 * Genera laberinto usando algoritmo de Prim.
 * Crea laberinto con múltiples caminos posibles.
 */
void generar_desde_grafo(laberinto* lab) {
    srand(time(NULL));  // Inicializa generador
    
    // Inicializa todo como paredes
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lab->celdas[i][j] = PARED;  // Pared por defecto
        }
    }
    
    int visitado[FILAS][COLUMNAS] = {0};  // Matriz de visitados
    
    // Lista de fronteras (celdas a procesar)
    int fronteras[FILAS * COLUMNAS * 2][2];
    int num_fronteras = 0;
    
    // Empieza desde celda central (1,1)
    int start_fila = 1;
    int start_col = 1;
    
    lab->celdas[start_fila][start_col] = CAMINO;  // Marca como camino
    visitado[start_fila][start_col] = 1;          // Marca como visitado
    
    int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};  // 4 direcciones
    
    // Agrega fronteras iniciales (2 pasos de distancia)
    for (int d = 0; d < 4; d++) {
        int nf = start_fila + dirs[d][0] * 2;  // Dos pasos en dirección d
        int nc = start_col + dirs[d][1] * 2;
        
        if (nf >= 1 && nf < FILAS-1 && nc >= 1 && nc < COLUMNAS-1) {
            fronteras[num_fronteras][0] = nf;  // Guarda fila
            fronteras[num_fronteras][1] = nc;  // Guarda columna
            num_fronteras++;                   // Incrementa contador
        }
    }
    
    while (num_fronteras > 0) {  // Mientras haya fronteras
        int idx = rand() % num_fronteras;  // Índice aleatorio
        int fila = fronteras[idx][0];      // Fila de frontera
        int col = fronteras[idx][1];       // Columna de frontera
        
        // Elimina esta frontera (swap con última)
        fronteras[idx][0] = fronteras[num_fronteras-1][0];
        fronteras[idx][1] = fronteras[num_fronteras-1][1];
        num_fronteras--;  // Reduce contador
        
        if (!visitado[fila][col]) {  // Si no está visitada
            lab->celdas[fila][col] = CAMINO;  // Convierte a camino
            visitado[fila][col] = 1;          // Marca como visitado
            
            // Encuentra vecinos que ya sean caminos
            int vecinos_camino[4];
            int num_vecinos_camino = 0;
            
            for (int d = 0; d < 4; d++) {
                int nf = fila + dirs[d][0];  // Vecino en dirección d
                int nc = col + dirs[d][1];
                
                // Verifica que sea camino y visitado
                if (nf >= 0 && nf < FILAS && nc >= 0 && nc < COLUMNAS &&
                    lab->celdas[nf][nc] == CAMINO && visitado[nf][nc]) {
                    vecinos_camino[num_vecinos_camino++] = d;  // Agrega dirección
                }
            }
            
            if (num_vecinos_camino > 0) {  // Si hay vecinos caminos
                int dir = vecinos_camino[rand() % num_vecinos_camino];  // Dirección aleatoria
                int pared_fila = fila + dirs[dir][0];  // Fila de pared
                int pared_col = col + dirs[dir][1];    // Columna de pared
                
                lab->celdas[pared_fila][pared_col] = CAMINO;  // Quita pared
            }
            
            // Agrega nuevas fronteras (2 pasos de distancia)
            for (int d = 0; d < 4; d++) {
                int nf = fila + dirs[d][0] * 2;  // Dos pasos
                int nc = col + dirs[d][1] * 2;
                
                if (nf >= 1 && nf < FILAS-1 && nc >= 1 && nc < COLUMNAS-1 &&
                    !visitado[nf][nc]) {
                    
                    // Verifica si ya está en la lista
                    int ya_en_lista = 0;
                    for (int i = 0; i < num_fronteras; i++) {
                        if (fronteras[i][0] == nf && fronteras[i][1] == nc) {
                            ya_en_lista = 1;  // Ya está en lista
                            break;
                        }
                    }
                    
                    if (!ya_en_lista) {  // Si no está en lista
                        fronteras[num_fronteras][0] = nf;  // Guarda fila
                        fronteras[num_fronteras][1] = nc;  // Guarda columna
                        num_fronteras++;                   // Incrementa contador
                    }
                }
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
            usleep(200000);  // 200ms para Unix/Linux (200,000 microsegundos)
        #endif
    }
}