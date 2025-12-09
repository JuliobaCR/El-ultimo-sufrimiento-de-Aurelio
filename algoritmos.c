/*
 * algoritmos.c
 * Implementación de algoritmos de grafos.
 * Incluye Dijkstra, Prim, Kruskal y otros.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "algoritmos.h"
#include "config.h"

// ==================== DIJKSTRA CON ESTADOS ====================

/*
 * dijkstra_con_estados
 * Implementa Dijkstra mostrando estados intermedios.
 */
int* dijkstra_con_estados(grafo_matriz* grafo, int inicio, int fin, 
                         int* distancia_total, int mostrar_estados) {
    static int anterior[MAX_NODOS];        // Array de nodos anteriores
    int distancia[MAX_NODOS];              // Distancias mínimas
    int visitado[MAX_NODOS] = {0};         // Nodos procesados
    int iteracion = 0;                     // Contador de iteraciones
    
    // Inicializa arrays
    for (int i = 0; i < grafo->num_nodos; i++) {
        distancia[i] = INFINITO;    // Distancia infinita inicial
        anterior[i] = -1;           // Sin nodo anterior
    }
    
    distancia[inicio] = 0;    // Distancia al inicio es cero
    
    // Muestra estado inicial si se solicita
    if (mostrar_estados) {
        printf("\n" COLOR_CYAN "=== INICIO ALGORITMO DIJKSTRA ===\n" COLOR_RESET);
        printf("Nodo inicial: %d, Nodo final: %d\n\n", inicio, fin);
        imprimir_estado_dijkstra(grafo->num_nodos, distancia, anterior, 
                                visitado, -1, iteracion);
    }
    
    // Procesa todos los nodos
    for (int count = 0; count < grafo->num_nodos - 1; count++) {
        iteracion++;    // Incrementa contador de iteración
        
        // Encuentra nodo no visitado con distancia mínima
        int min = INFINITO;
        int u = -1;     // Nodo seleccionado
        
        for (int v = 0; v < grafo->num_nodos; v++) {
            if (!visitado[v] && distancia[v] < min) {
                min = distancia[v];    // Actualiza mínimo
                u = v;                 // Nodo con distancia mínima
            }
        }
        
        // Termina si no hay nodos o se llegó al destino
        if (u == -1 || u == fin) {
            if (mostrar_estados) {
                printf("\n" COLOR_VERDE "=== TERMINANDO TEMPRANO ===\n" COLOR_RESET);
                printf("Nodo actual: %d, Nodo destino alcanzado: %s\n", 
                       u, (u == fin) ? "SÍ" : "NO");
            }
            break;    // Termina el algoritmo
        }
        
        visitado[u] = 1;    // Marca nodo como procesado
        
        // Muestra información de iteración
        if (mostrar_estados) {
            printf("\n" COLOR_MAGENTA "=== ITERACIÓN %d ===\n" COLOR_RESET, iteracion);
            printf("Nodo seleccionado (distancia mínima): %d\n", u);
            printf("Distancia acumulada hasta %d: %d\n", u, distancia[u]);
        }
        
        // Actualiza distancias de vecinos
        int vecinos_actualizados = 0;    // Contador de actualizaciones
        
        for (int v = 0; v < grafo->num_nodos; v++) {
            // Si hay arista y nodo no visitado
            if (!visitado[v] && grafo->matriz[u][v] != 0) {
                int nueva_distancia = distancia[u] + grafo->matriz[u][v];
                
                // Encontró camino más corto
                if (nueva_distancia < distancia[v]) {
                    distancia[v] = nueva_distancia;    // Actualiza distancia
                    anterior[v] = u;                   // Actualiza anterior
                    vecinos_actualizados++;            // Incrementa contador
                    
                    // Muestra actualización
                    if (mostrar_estados) {
                        printf("  → Actualizando nodo %d: %d + %d = %d\n",
                               v, distancia[u], grafo->matriz[u][v], nueva_distancia);
                    }
                }
            }
        }
        
        // Muestra estado después de iteración
        if (mostrar_estados) {
            if (vecinos_actualizados == 0) {
                printf("  No se actualizaron vecinos en esta iteración\n");
            }
            printf("\nEstado después de iteración %d:\n", iteracion);
            imprimir_estado_dijkstra(grafo->num_nodos, distancia, anterior, 
                                    visitado, u, iteracion);
            
            printf("\nPresione Enter para continuar...");
            getchar();    // Pausa para visualización
        }
    }
    
    // Calcula distancia total
    *distancia_total = (distancia[fin] == INFINITO) ? -1 : distancia[fin];
    
    // Muestra resultado final
    if (mostrar_estados) {
        printf("\n" COLOR_CYAN "=== FINAL DEL ALGORITMO ===\n" COLOR_RESET);
        printf("Distancia final al nodo %d: ", fin);
        if (*distancia_total == -1) {
            printf("INFINITO (no alcanzable)\n");
        } else {
            printf("%d\n", *distancia_total);
        }
        
        // Reconstruye camino si existe
        if (*distancia_total != -1) {
            printf("\nReconstruyendo camino...\n");
            reconstruir_camino_detallado(anterior, inicio, fin, *distancia_total);
        }
    }
    
    return anterior;    // Retorna array de anteriores
}

/*
 * imprimir_estado_dijkstra
 * Muestra tabla con estado actual del algoritmo.
 * Formato tabular con colores para diferentes estados.
 */
void imprimir_estado_dijkstra(int num_nodos, int distancia[], int anterior[], 
                              int visitado[], int nodo_actual, int iteracion) {
    printf("┌─────────────────────────────────────────────────────┐\n");
    printf("│ %-51s │\n", (iteracion == 0) ? "ESTADO INICIAL" : "ESTADO INTERMEDIO");
    printf("├─────┬───────────┬────────────┬──────────┬───────────┤\n");
    printf("│ Nodo│ Visitado  │ Distancia  │ Anterior │ Estado    │\n");
    printf("├─────┼───────────┼────────────┼──────────┼───────────┤\n");
    
    // Muestra primeros 15 nodos (para no saturar pantalla)
    for (int i = 0; i < num_nodos && i < 15; i++) {
        char visitado_str[10];     // String para visitado
        char distancia_str[12];    // String para distancia
        char anterior_str[10];     // String para anterior
        char estado_str[15];       // String para estado
        
        // Convierte valores a strings
        sprintf(visitado_str, "%s", visitado[i] ? "SÍ" : "NO");
        
        if (distancia[i] == INFINITO) {
            sprintf(distancia_str, "INF");    // Infinito
        } else {
            sprintf(distancia_str, "%d", distancia[i]);    // Distancia numérica
        }
        
        sprintf(anterior_str, "%d", anterior[i]);    // Nodo anterior
        if (anterior[i] == -1) strcpy(anterior_str, "-");    // Sin anterior
        
        // Determina estado del nodo
        if (i == nodo_actual) {
            sprintf(estado_str, COLOR_VERDE "ACTUAL" COLOR_RESET);    // Nodo actual
        } else if (visitado[i]) {
            sprintf(estado_str, "PROCESADO");    // Ya procesado
        } else if (distancia[i] != INFINITO) {
            sprintf(estado_str, "EN COLA");      // En cola de procesamiento
        } else {
            sprintf(estado_str, "NO VISITADO");  // Sin visitar
        }
        
        // Imprime fila de la tabla
        printf("│ %3d │ %-9s │ %-10s │ %-8s │ %-9s │\n", 
               i, visitado_str, distancia_str, anterior_str, estado_str);
    }
    
    // Mensaje para grafos grandes
    if (num_nodos > 15) {
        printf("├─────┴───────────┴────────────┴──────────┴───────────┤\n");
        printf("│ ... (mostrando 15 de %d nodos)                     │\n", num_nodos);
    }
    
    printf("└─────────────────────────────────────────────────────┘\n");
}

/*
 * reconstruir_camino_detallado
 * Reconstruye y muestra el camino encontrado.
 * Muestra cada paso con formato tabular.
 */
void reconstruir_camino_detallado(int* anterior, int inicio, int fin, 
                                  int distancia_total) {
    int camino[MAX_NODOS];    // Array para camino
    int longitud = 0;         // Longitud del camino
    int nodo = fin;           // Comienza desde el final
    
    printf("\n" COLOR_VERDE "=== RECONSTRUCCIÓN DEL CAMINO ===\n" COLOR_RESET);
    
    // Reconstruye camino en orden inverso
    while (nodo != -1) {
        camino[longitud++] = nodo;    // Agrega nodo al camino
        nodo = anterior[nodo];        // Retrocede al anterior
    }
    
    // Información del camino encontrado
    printf("Camino encontrado (longitud: %d pasos, distancia total: %d):\n", 
           longitud - 1, distancia_total);
    printf("\nRuta: ");
    
    // Imprime ruta completa
    for (int i = longitud - 1; i >= 0; i--) {
        printf("%d", camino[i]);    // Imprime nodo
        if (i > 0) {
            printf(" → ");          // Separador
            if ((longitud - i) % 8 == 0) printf("\n       ");    // Salto de línea
        }
    }
    
    printf("\n\nPasos detallados:\n");
    printf("┌──────┬──────────────┬─────────────────────────────┐\n");
    printf("│ Paso │ Nodo actual  │ Camino acumulado           │\n");
    printf("├──────┼──────────────┼─────────────────────────────┤\n");
    
    char camino_parcial[100] = "";    // String para camino parcial
    
    // Imprime cada paso del camino
    for (int i = longitud - 1; i >= 0; i--) {
        char temp[10];
        sprintf(temp, "%d", camino[i]);    // Convierte nodo a string
        
        // Construye camino parcial
        if (strlen(camino_parcial) == 0) {
            strcpy(camino_parcial, temp);    // Primer nodo
        } else {
            strcat(camino_parcial, " → ");   // Agrega separador
            strcat(camino_parcial, temp);    // Agrega nodo
        }
        
        // Imprime fila de la tabla
        printf("│ %4d │ %12d │ %-27s │\n", 
               longitud - i, camino[i], camino_parcial);
    }
    printf("└──────┴──────────────┴─────────────────────────────┘\n");
}

// ==================== ALGORITMO PRIM ====================

/*
 * prim
 * Implementa algoritmo de Prim para árbol generador mínimo.
 * Encuentra árbol que conecta todos los nodos con mínimo peso.
 */
arista_prim* prim(grafo_matriz* grafo, int* num_aristas) {
    static arista_prim arbol[MAX_NODOS - 1];    // Array para árbol
    int clave[MAX_NODOS];                       // Claves mínimas
    int padre[MAX_NODOS];                       // Padres en árbol
    int en_arbol[MAX_NODOS] = {0};              // Nodos en árbol
    
    // Inicializa arrays
    for (int i = 0; i < grafo->num_nodos; i++) {
        clave[i] = INFINITO;    // Clave infinita inicial
        padre[i] = -1;          // Sin padre
    }
    
    clave[0] = 0;    // Clave del nodo inicial es cero
    padre[0] = -1;   // Nodo inicial no tiene padre
    
    // Construye árbol generador mínimo
    for (int count = 0; count < grafo->num_nodos - 1; count++) {
        // Encuentra nodo con clave mínima no en árbol
        int min = INFINITO;
        int u = -1;
        
        for (int v = 0; v < grafo->num_nodos; v++) {
            if (!en_arbol[v] && clave[v] < min) {
                min = clave[v];    // Actualiza mínimo
                u = v;             // Nodo con clave mínima
            }
        }
        
        if (u == -1) break;    // Termina si no hay nodos
        
        en_arbol[u] = 1;    // Agrega nodo al árbol
        
        // Actualiza claves de vecinos
        for (int v = 0; v < grafo->num_nodos; v++) {
            // Si hay arista, nodo no en árbol y peso menor
            if (grafo->matriz[u][v] != 0 && !en_arbol[v] && 
                grafo->matriz[u][v] < clave[v]) {
                clave[v] = grafo->matriz[u][v];    // Actualiza clave
                padre[v] = u;                      // Actualiza padre
            }
        }
    }
    
    // Recolecta aristas del árbol
    *num_aristas = 0;    // Inicializa contador
    
    for (int i = 1; i < grafo->num_nodos; i++) {
        if (padre[i] != -1) {    // Si tiene padre válido
            arbol[*num_aristas].origen = padre[i];               // Origen
            arbol[*num_aristas].destino = i;                     // Destino
            arbol[*num_aristas].peso = grafo->matriz[padre[i]][i]; // Peso
            (*num_aristas)++;                                    // Incrementa contador
        }
    }
    
    return arbol;    // Retorna árbol generador mínimo
}

// ==================== ALGORITMO KRUSKAL ====================

/*
 * comparar_aristas
 * Función de comparación para qsort.
 * Compara aristas por peso para ordenar.
 */
int comparar_aristas(const void* a, const void* b) {
    arista* arista_a = (arista*)a;    // Convierte puntero a
    arista* arista_b = (arista*)b;    // Convierte puntero b
    return arista_a->peso - arista_b->peso;    // Retorna diferencia
}

/*
 * kruskal
 * Implementa algoritmo de Kruskal para árbol generador mínimo.
 * Usa conjuntos disjuntos para evitar ciclos.
 */
arista* kruskal(grafo_matriz* grafo, int* num_aristas) {
    arista* arbol = malloc(MAX_ARISTAS * sizeof(arista));    // Array dinámico
    if (!arbol) return NULL;    // Verifica asignación
    
    conjunto_disjunto cd;                    // Conjuntos disjuntos
    arista aristas[MAX_ARISTAS];             // Array de aristas
    int total_aristas = 0;                   // Contador total
    
    // Recolecta todas las aristas del grafo
    for (int i = 0; i < grafo->num_nodos; i++) {
        for (int j = i + 1; j < grafo->num_nodos; j++) {
            if (grafo->matriz[i][j] > 0) {    // Si hay arista
                aristas[total_aristas].origen = i;        // Origen
                aristas[total_aristas].destino = j;       // Destino
                aristas[total_aristas].peso = grafo->matriz[i][j]; // Peso
                total_aristas++;                          // Incrementa contador
            }
        }
    }
    
    // Ordena aristas por peso (ascendente)
    qsort(aristas, total_aristas, sizeof(arista), comparar_aristas);
    
    // Inicializa conjuntos disjuntos
    for (int i = 0; i < grafo->num_nodos; i++) {
        hacer_conjunto(&cd, i);    // Cada nodo en su propio conjunto
    }
    
    // Construye árbol generador mínimo
    *num_aristas = 0;    // Inicializa contador
    
    for (int i = 0; i < total_aristas && *num_aristas < grafo->num_nodos - 1; i++) {
        int origen = aristas[i].origen;     // Origen de arista
        int destino = aristas[i].destino;   // Destino de arista
        
        // Si están en conjuntos diferentes (no crea ciclo)
        if (encontrar(&cd, origen) != encontrar(&cd, destino)) {
            arbol[*num_aristas] = aristas[i];    // Agrega arista al árbol
            (*num_aristas)++;                    // Incrementa contador
            unir(&cd, origen, destino);          // Une conjuntos
        }
    }
    
    return arbol;    // Retorna árbol generador mínimo
}

// ==================== BÚSQUEDA EN ANCHURA ====================

/*
 * bfs
 * Implementa búsqueda en anchura (Breadth-First Search).
 * Encuentra camino más corto en número de aristas.
 */
int* bfs(grafo_matriz* grafo, int inicio) {
    static int anterior[MAX_NODOS];    // Array de anteriores
    int cola[MAX_NODOS];               // Cola para BFS
    int visitado[MAX_NODOS] = {0};     // Nodos visitados
    int frente = 0, final = 0;         // Índices de cola
    
    // Inicializa array de anteriores
    for (int i = 0; i < grafo->num_nodos; i++) {
        anterior[i] = -1;    // Sin nodo anterior
    }
    
    cola[final++] = inicio;    // Agrega inicio a cola
    visitado[inicio] = 1;      // Marca como visitado
    
    while (frente < final) {   // Mientras haya nodos en cola
        int actual = cola[frente++];    // Saca nodo de cola
        
        // Procesa vecinos del nodo actual
        for (int v = 0; v < grafo->num_nodos; v++) {
            // Si hay arista y nodo no visitado
            if (grafo->matriz[actual][v] > 0 && !visitado[v]) {
                visitado[v] = 1;           // Marca como visitado
                anterior[v] = actual;      // Establece anterior
                cola[final++] = v;         // Agrega a cola
            }
        }
    }
    
    return anterior;    // Retorna array de anteriores
}

// ==================== FUNCIONES AUXILIARES ====================

/*
 * imprimir_camino
 * Imprime camino reconstruido de forma simple.
 * Muestra secuencia de nodos sin detalles.
 */
void imprimir_camino(int* anterior, int inicio, int fin) {
    if (anterior[fin] == -1) {    // Si no hay camino
        printf("No hay camino desde %d hasta %d\n", inicio, fin);
        return;
    }
    
    int camino[MAX_NODOS];    // Array para camino
    int longitud = 0;         // Longitud del camino
    int nodo = fin;           // Comienza desde el final
    
    // Reconstruye camino en orden inverso
    while (nodo != -1) {
        camino[longitud++] = nodo;    // Agrega nodo
        nodo = anterior[nodo];        // Retrocede al anterior
    }
    
    // Imprime camino en orden correcto
    printf(COLOR_VERDE "Camino encontrado (%d pasos):\n" COLOR_RESET, longitud - 1);
    
    for (int i = longitud - 1; i >= 0; i--) {
        printf("%d", camino[i]);    // Imprime nodo
        if (i > 0) {
            printf(" -> ");         // Separador
            if ((longitud - i) % 10 == 0) printf("\n              ");    // Salto línea
        }
    }
    printf("\n");
}

/*
 * crear_matriz_adyacencia
 * Crea copia dinámica de matriz de adyacencia.
 * Útil para algoritmos que modifican la matriz.
 */
int** crear_matriz_adyacencia(grafo_matriz* grafo) {
    int** matriz = (int**)malloc(grafo->num_nodos * sizeof(int*));    // Array de filas
    if (!matriz) return NULL;    // Verifica asignación
    
    // Asigna memoria para cada fila
    for (int i = 0; i < grafo->num_nodos; i++) {
        matriz[i] = (int*)malloc(grafo->num_nodos * sizeof(int));    // Fila i
        if (!matriz[i]) {    // Verifica asignación
            for (int j = 0; j < i; j++) free(matriz[j]);    // Libera anteriores
            free(matriz);                                   // Libera array
            return NULL;                                    // Retorna NULL
        }
        
        // Copia valores de la matriz original
        for (int j = 0; j < grafo->num_nodos; j++) {
            matriz[i][j] = grafo->matriz[i][j];    // Copia valor
        }
    }
    
    return matriz;    // Retorna matriz copiada
}

/*
 * liberar_matriz_adyacencia
 * Libera memoria de matriz dinámica.
 * Recibe tamaño n para liberar todas las filas.
 */
void liberar_matriz_adyacencia(int** matriz, int n) {
    if (matriz) {    // Si matriz no es NULL
        for (int i = 0; i < n; i++) {
            free(matriz[i]);    // Libera cada fila
        }
        free(matriz);    // Libera array de filas
    }
}

/*
 * liberar_aristas
 * Libera memoria de array dinámico de aristas.
 * Simple wrapper para free().
 */
void liberar_aristas(arista* aristas) {
    if (aristas) {
        free(aristas);    // Libera memoria
    }
}

// ==================== ALGORITMOS ADICIONALES ====================

/*
 * encontrar_caminos_criticos
 * Encuentra caminos más largos en el grafo.
 * Usa Floyd-Warshall para todas las distancias.
 */
void encontrar_caminos_criticos(grafo_matriz* grafo) {
    printf(COLOR_CYAN "\n=== CAMINOS CRÍTICOS ===\n" COLOR_RESET);
    
    int** distancias = crear_matriz_adyacencia(grafo);    // Copia de matriz
    if (!distancias) {
        printf("Error al crear matriz de distancias\n");
        return;
    }
    
    int n = grafo->num_nodos;    // Número de nodos
    
    // Inicializa Floyd-Warshall
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Si no hay conexión directa y no es el mismo nodo
            if (i != j && distancias[i][j] == 0) {
                distancias[i][j] = INFINITO;    // Establece infinito
            }
        }
    }
    
    // Algoritmo Floyd-Warshall
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                // Si hay camino i→k y k→j
                if (distancias[i][k] != INFINITO && 
                    distancias[k][j] != INFINITO &&
                    distancias[i][k] + distancias[k][j] < distancias[i][j]) {
                    distancias[i][j] = distancias[i][k] + distancias[k][j];    // Actualiza
                }
            }
        }
    }
    
    // Encuentra camino más largo
    int max_distancia = 0;
    int inicio_critico = -1, fin_critico = -1;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Si hay camino y es el más largo encontrado
            if (distancias[i][j] != INFINITO && distancias[i][j] > max_distancia) {
                max_distancia = distancias[i][j];    // Actualiza máximo
                inicio_critico = i;                  // Actualiza inicio
                fin_critico = j;                     // Actualiza fin
            }
        }
    }
    
    // Muestra resultado
    if (inicio_critico != -1) {
        printf("Camino más largo: del nodo %d al nodo %d\n", 
               inicio_critico, fin_critico);
        printf("Distancia: %d\n", max_distancia);
    } else {
        printf("No se encontraron caminos críticos\n");
    }
    
    liberar_matriz_adyacencia(distancias, n);    // Libera memoria
}

/*
 * encontrar_componentes_conexas
 * Identifica componentes conexas del grafo.
 * Usa BFS para encontrar nodos conectados.
 */
void encontrar_componentes_conexas(grafo_matriz* grafo) {
    printf(COLOR_CYAN "\n=== COMPONENTES CONEXAS ===\n" COLOR_RESET);
    
    int visitado[MAX_NODOS] = {0};    // Nodos visitados
    int componentes = 0;              // Contador de componentes
    
    for (int i = 0; i < grafo->num_nodos; i++) {
        if (!visitado[i]) {    // Si nodo no visitado
            componentes++;      // Nueva componente
            printf("\nComponente %d: ", componentes);
            
            int cola[MAX_NODOS];               // Cola para BFS
            int frente = 0, final = 0;         // Índices de cola
            
            cola[final++] = i;    // Agrega nodo a cola
            visitado[i] = 1;      // Marca como visitado
            
            // BFS desde este nodo
            while (frente < final) {
                int actual = cola[frente++];    // Saca nodo de cola
                printf("%d ", actual);          // Imprime nodo
                
                // Procesa vecinos
                for (int v = 0; v < grafo->num_nodos; v++) {
                    // Si hay arista y nodo no visitado
                    if (grafo->matriz[actual][v] > 0 && !visitado[v]) {
                        visitado[v] = 1;           // Marca como visitado
                        cola[final++] = v;         // Agrega a cola
                    }
                }
            }
        }
    }
    
    printf("\n\nTotal de componentes conexas: %d\n", componentes);
}

/*
 * ordenacion_topologica
 * Realiza ordenación topológica del grafo.
 * Solo funciona para grafos dirigidos acíclicos.
 */
void ordenacion_topologica(grafo_matriz* grafo) {
    printf(COLOR_CYAN "\n=== ORDENACIÓN TOPOLÓGICA ===\n" COLOR_RESET);
    
    int grado_entrada[MAX_NODOS] = {0};    // Grados de entrada
    
    // Calcula grados de entrada
    for (int i = 0; i < grafo->num_nodos; i++) {
        for (int j = 0; j < grafo->num_nodos; j++) {
            if (grafo->matriz[i][j] > 0) {    // Si hay arista i→j
                grado_entrada[j]++;           // Incrementa grado de entrada de j
            }
        }
    }
    
    int cola[MAX_NODOS];               // Cola para nodos sin dependencias
    int frente = 0, final = 0;         // Índices de cola
    
    // Inicializa cola con nodos de grado 0
    for (int i = 0; i < grafo->num_nodos; i++) {
        if (grado_entrada[i] == 0) {    // Sin dependencias
            cola[final++] = i;          // Agrega a cola
        }
    }
    
    int orden[MAX_NODOS];    // Array para orden topológico
    int idx = 0;             // Índice en orden
    
    // Procesa nodos en orden topológico
    while (frente < final) {
        int actual = cola[frente++];    // Saca nodo de cola
        orden[idx++] = actual;          // Agrega al orden
        
        // Reduce grados de entrada de sus vecinos
        for (int v = 0; v < grafo->num_nodos; v++) {
            if (grafo->matriz[actual][v] > 0) {    // Si hay arista
                grado_entrada[v]--;                // Reduce grado
                if (grado_entrada[v] == 0) {       // Si ya no tiene dependencias
                    cola[final++] = v;             // Agrega a cola
                }
            }
        }
    }
    
    // Verifica si hay ciclo
    if (idx != grafo->num_nodos) {
        printf("El grafo contiene ciclos! (no es DAG)\n");
    } else {
        printf("Orden topológico: ");
        for (int i = 0; i < idx; i++) {
            printf("%d ", orden[i]);    // Imprime orden
        }
        printf("\n");
    }
}

/*
 * explicar_algoritmos_seleccion
 * Explica por qué se seleccionaron los algoritmos.
 * Describe su aplicación a laberintos y rendimiento.
 */
void explicar_algoritmos_seleccion(void) {
    limpiar_pantalla();
    
    printf(COLOR_CYAN "=== EXPLICACIÓN DE ALGORITMOS SELECCIONADOS ===\n\n" COLOR_RESET);
    
    printf(COLOR_AMARILLO "Algoritmo 1: Dijkstra\n" COLOR_RESET);
    printf("Seleccionado porque es el estándar para caminos más cortos.\n");
    printf("Se ajusta a laberintos porque encuentra ruta mínima entre puntos.\n");
    printf("Funciona bien siempre que haya solución óptima.\n\n");
    
    printf(COLOR_AMARILLO "Algoritmo 2: Prim\n" COLOR_RESET);
    printf("Seleccionado porque genera árboles generadores mínimos.\n");
    printf("Se ajusta a laberintos porque crea estructuras sin ciclos.\n");
    printf("Funciona bien para generar laberintos diversos y complejos.\n\n");
    
    printf(COLOR_AMARILLO "Comparación con otros algoritmos:\n" COLOR_RESET);
    printf("Kruskal: Similar a Prim pero para grafos dispersos.\n");
    printf("BFS: Encuentra camino más corto sin considerar pesos.\n");
    
    pausa();
}