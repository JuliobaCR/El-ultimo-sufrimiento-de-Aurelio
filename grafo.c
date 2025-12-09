/*
 * grafo.c
 * Implementación de operaciones básicas de grafos.
 * Incluye matrices, listas y generación de grafos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "grafo.h"

// ==================== GENERACIÓN DE GRAFOS CON CAMINO ====================

/*
 * generar_grafo_con_camino
 * Crea un grafo con camino principal garantizado.
 * Añade conexiones aleatorias para caminos alternativos.
 */
void generar_grafo_con_camino(grafo_matriz* grafo) {
    srand(time(NULL));                    // Inicializa generador aleatorio
    
    int num_nodos = grafo->num_nodos;     // Obtiene tamaño del grafo
    
    // Crea camino principal del nodo 0 al último
    for (int i = 0; i < num_nodos - 1; i++) {
        int peso = rand() % 10 + 1;       // Peso aleatorio 1-10
        agregar_arista_matriz(grafo, i, i + 1, peso);
    }
    
    // Añade conexiones aleatorias adicionales
    for (int i = 0; i < num_nodos; i++) {
        // Intenta hasta 3 conexiones extra por nodo
        for (int k = 0; k < 3; k++) {
            // Vecino cercano (±2 posiciones)
            int posible_vecino = i + (rand() % 5) - 2;
            
            // Verifica que el vecino sea válido
            if (posible_vecino >= 0 && posible_vecino < num_nodos && 
                posible_vecino != i && grafo->matriz[i][posible_vecino] == 0) {
                
                // 40% de probabilidad de conectar
                if (rand() % 100 < 40) {
                    int peso = rand() % 15 + 1;    // Peso 1-15
                    agregar_arista_matriz(grafo, i, posible_vecino, peso);
                }
            }
        }
    }
    
    // Añade conexiones diagonales aleatorias
    for (int i = 0; i < num_nodos / 4; i++) {
        int nodo1 = rand() % num_nodos;    // Nodo aleatorio 1
        int nodo2 = rand() % num_nodos;    // Nodo aleatorio 2
        
        // Verifica que sean diferentes y no conectados
        if (nodo1 != nodo2 && grafo->matriz[nodo1][nodo2] == 0) {
            // 20% de probabilidad de conectar
            if (rand() % 100 < 20) {
                int peso = rand() % 20 + 5;    // Peso 5-24
                agregar_arista_matriz(grafo, nodo1, nodo2, peso);
            }
        }
    }
}

// ==================== GRAFO CON MATRIZ ====================

/*
 * inicializar_grafo_matriz
 * Configura un grafo con matriz de adyacencia.
 * Establece todas las conexiones en cero.
 */
void inicializar_grafo_matriz(grafo_matriz* grafo, int num_nodos) {
    grafo->num_nodos = num_nodos;    // Asigna número de nodos
    
    // Inicializa toda la matriz en cero
    for (int i = 0; i < MAX_NODOS; i++) {
        for (int j = 0; j < MAX_NODOS; j++) {
            grafo->matriz[i][j] = 0;    // Sin conexiones
        }
    }
}

/*
 * agregar_arista_matriz
 * Añade una arista bidireccional a la matriz.
 * Verifica índices válidos antes de agregar.
 */
void agregar_arista_matriz(grafo_matriz* grafo, int origen, int destino, int peso) {
    // Verifica que los índices sean válidos
    if (origen >= 0 && origen < grafo->num_nodos &&
        destino >= 0 && destino < grafo->num_nodos) {
        grafo->matriz[origen][destino] = peso;    // Conexión origen→destino
        grafo->matriz[destino][origen] = peso;    // Conexión destino→origen
    }
}

/*
 * eliminar_arista_matriz
 * Elimina una arista de la matriz.
 * Establece ambos sentidos en cero.
 */
void eliminar_arista_matriz(grafo_matriz* grafo, int origen, int destino) {
    // Verifica índices válidos
    if (origen >= 0 && origen < grafo->num_nodos &&
        destino >= 0 && destino < grafo->num_nodos) {
        grafo->matriz[origen][destino] = 0;    // Elimina conexión
        grafo->matriz[destino][origen] = 0;    // Elimina conexión inversa
    }
}

/*
 * obtener_peso_matriz
 * Obtiene el peso de una arista en la matriz.
 * Retorna cero si no existe conexión.
 */
int obtener_peso_matriz(grafo_matriz* grafo, int origen, int destino) {
    // Verifica índices válidos
    if (origen >= 0 && origen < grafo->num_nodos &&
        destino >= 0 && destino < grafo->num_nodos) {
        return grafo->matriz[origen][destino];    // Retorna peso
    }
    return 0;    // Retorna cero si índices inválidos
}

/*
 * imprimir_matriz_adyacencia
 * Muestra la matriz de adyacencia formateada.
 * Usa colores para resaltar conexiones.
 */
void imprimir_matriz_adyacencia(grafo_matriz* grafo) {
    printf("\n" COLOR_CYAN "=== MATRIZ DE ADYACENCIA ===\n" COLOR_RESET);
    
    // Imprime encabezado de columnas
    printf("   ");
    for (int i = 0; i < 10 && i < grafo->num_nodos; i++) {
        printf("%3d", i);    // Números de columna
    }
    if (grafo->num_nodos > 10) printf(" ...");    // Indica truncamiento
    
    printf("\n");    // Nueva línea después del encabezado
    
    // Imprime filas de la matriz
    for (int i = 0; i < 10 && i < grafo->num_nodos; i++) {
        printf("%2d ", i);    // Número de fila
        
        for (int j = 0; j < 10 && j < grafo->num_nodos; j++) {
            if (grafo->matriz[i][j] > 0) {
                // Conexión existente (verde)
                printf(COLOR_VERDE "%3d" COLOR_RESET, grafo->matriz[i][j]);
            } else {
                // Sin conexión (punto)
                printf("  .");
            }
        }
        
        if (grafo->num_nodos > 10) printf(" ...");    // Indica truncamiento
        printf("\n");    // Nueva línea por fila
    }
    
    // Mensaje para matrices grandes
    if (grafo->num_nodos > 10) {
        printf("...\n");
        printf("(Matriz completa: %d x %d)\n", grafo->num_nodos, grafo->num_nodos);
    }
}

/*
 * liberar_grafo_matriz
 * Libera recursos de un grafo en matriz.
 * En matriz estática, solo resetea el contador.
 */
void liberar_grafo_matriz(grafo_matriz* grafo) {
    grafo->num_nodos = 0;    // Resetea contador de nodos
}

// ==================== GRAFO CON LISTA ====================

/*
 * inicializar_grafo_lista
 * Configura un grafo con listas de adyacencia.
 * Inicializa todas las listas como NULL.
 */
void inicializar_grafo_lista(grafo_lista* grafo, int num_nodos) {
    grafo->num_nodos = num_nodos;    // Asigna número de nodos
    
    // Inicializa todas las listas como vacías
    for (int i = 0; i < MAX_NODOS; i++) {
        grafo->lista[i] = NULL;    // Lista vacía
    }
}

/*
 * agregar_arista_lista
 * Añade una arista bidireccional a las listas.
 * Crea nodos en ambas listas de adyacencia.
 */
void agregar_arista_lista(grafo_lista* grafo, int origen, int destino, int peso) {
    // Verifica índices válidos
    if (origen < 0 || origen >= grafo->num_nodos ||
        destino < 0 || destino >= grafo->num_nodos) {
        return;    // Índices inválidos, no hace nada
    }
    
    // Agrega a lista del nodo origen
    nodo_lista* nuevo = (nodo_lista*)malloc(sizeof(nodo_lista));
    nuevo->destino = destino;          // Nodo destino
    nuevo->peso = peso;                // Peso de la arista
    nuevo->siguiente = grafo->lista[origen];  // Inserta al inicio
    grafo->lista[origen] = nuevo;      // Actualiza cabeza de lista
    
    // Agrega a lista del nodo destino (grafo no dirigido)
    nuevo = (nodo_lista*)malloc(sizeof(nodo_lista));
    nuevo->destino = origen;           // Nodo origen
    nuevo->peso = peso;                // Mismo peso
    nuevo->siguiente = grafo->lista[destino];  // Inserta al inicio
    grafo->lista[destino] = nuevo;     // Actualiza cabeza de lista
}

/*
 * eliminar_arista_lista
 * Elimina una arista de las listas de adyacencia.
 * Busca y elimina en ambas direcciones.
 */
void eliminar_arista_lista(grafo_lista* grafo, int origen, int destino) {
    nodo_lista* actual = grafo->lista[origen];  // Inicio de lista origen
    nodo_lista* anterior = NULL;                // Nodo anterior
    
    // Busca la arista en lista del origen
    while (actual != NULL && actual->destino != destino) {
        anterior = actual;              // Actualiza anterior
        actual = actual->siguiente;     // Avanza en la lista
    }
    
    // Si encontró la arista, la elimina
    if (actual != NULL) {
        if (anterior == NULL) {
            // Es el primer nodo
            grafo->lista[origen] = actual->siguiente;
        } else {
            // Está en medio de la lista
            anterior->siguiente = actual->siguiente;
        }
        free(actual);    // Libera memoria
    }
    
    // Busca en lista del destino (conexión inversa)
    actual = grafo->lista[destino];
    anterior = NULL;
    
    while (actual != NULL && actual->destino != origen) {
        anterior = actual;              // Actualiza anterior
        actual = actual->siguiente;     // Avanza en la lista
    }
    
    // Si encontró la arista, la elimina
    if (actual != NULL) {
        if (anterior == NULL) {
            // Es el primer nodo
            grafo->lista[destino] = actual->siguiente;
        } else {
            // Está en medio de la lista
            anterior->siguiente = actual->siguiente;
        }
        free(actual);    // Libera memoria
    }
}

/*
 * obtener_peso_lista
 * Busca el peso de una arista en lista.
 * Retorna cero si no encuentra la conexión.
 */
int obtener_peso_lista(grafo_lista* grafo, int origen, int destino) {
    nodo_lista* actual = grafo->lista[origen];  // Inicio de lista
    
    // Recorre lista buscando el destino
    while (actual != NULL) {
        if (actual->destino == destino) {
            return actual->peso;    // Encontró, retorna peso
        }
        actual = actual->siguiente;    // Siguiente nodo
    }
    
    return 0;    // No encontró, retorna cero
}

/*
 * imprimir_lista_adyacencia
 * Muestra todas las listas de adyacencia.
 * Formato: nodo -> [destino (peso)] -> ...
 */
void imprimir_lista_adyacencia(grafo_lista* grafo) {
    printf("\n" COLOR_CYAN "=== LISTA DE ADYACENCIA ===\n" COLOR_RESET);
    
    // Para cada nodo en el grafo
    for (int i = 0; i < grafo->num_nodos; i++) {
        printf(COLOR_AMARILLO "Nodo %d:" COLOR_RESET, i);    // Encabezado del nodo
        
        nodo_lista* actual = grafo->lista[i];    // Inicio de lista
        
        // Recorre todos los vecinos
        while (actual != NULL) {
            printf(" -> [%d (%d)]", actual->destino, actual->peso);
            actual = actual->siguiente;    // Siguiente vecino
        }
        printf("\n");    // Nueva línea por nodo
    }
}

/*
 * liberar_grafo_lista
 * Libera toda la memoria de las listas.
 * Recorre cada lista liberando nodos.
 */
void liberar_grafo_lista(grafo_lista* grafo) {
    // Para cada nodo en el grafo
    for (int i = 0; i < grafo->num_nodos; i++) {
        nodo_lista* actual = grafo->lista[i];    // Inicio de lista
        
        // Recorre y libera todos los nodos
        while (actual != NULL) {
            nodo_lista* temp = actual;     // Nodo a liberar
            actual = actual->siguiente;    // Avanza al siguiente
            free(temp);                    // Libera memoria
        }
        
        grafo->lista[i] = NULL;    // Lista vacía
    }
    
    grafo->num_nodos = 0;    // Resetea contador
}

// ==================== CONJUNTO DISJUNTO ====================

/*
 * hacer_conjunto
 * Inicializa un elemento en conjunto disjunto.
 * Cada elemento es su propio padre inicialmente.
 */
void hacer_conjunto(conjunto_disjunto* cd, int x) {
    cd->padre[x] = x;    // Padre es él mismo
    cd->rango[x] = 0;    // Rango inicial cero
}

/*
 * encontrar
 * Encuentra el representante de un conjunto.
 * Implementa compresión de caminos.
 */
int encontrar(conjunto_disjunto* cd, int x) {
    if (cd->padre[x] != x) {                          // No es el representante
        cd->padre[x] = encontrar(cd, cd->padre[x]);   // Compresión de caminos
    }
    return cd->padre[x];    // Retorna representante
}

/*
 * unir
 * Une dos conjuntos disjuntos.
 * Usa unión por rango para optimizar.
 */
void unir(conjunto_disjunto* cd, int x, int y) {
    int raiz_x = encontrar(cd, x);    // Representante de x
    int raiz_y = encontrar(cd, y);    // Representante de y
    
    // Solo une si son diferentes conjuntos
    if (raiz_x != raiz_y) {
        // Unión por rango
        if (cd->rango[raiz_x] < cd->rango[raiz_y]) {
            cd->padre[raiz_x] = raiz_y;    // Raíz_x bajo raíz_y
        } else if (cd->rango[raiz_x] > cd->rango[raiz_y]) {
            cd->padre[raiz_y] = raiz_x;    // Raíz_y bajo raíz_x
        } else {
            cd->padre[raiz_y] = raiz_x;    // Raíz_y bajo raíz_x
            cd->rango[raiz_x]++;           // Incrementa rango
        }
    }
}

// ==================== CONVERSIONES ====================

/*
 * matriz_a_lista
 * Convierte grafo de matriz a lista.
 * Solo procesa conexiones no duplicadas.
 */
void matriz_a_lista(grafo_matriz* origen, grafo_lista* destino) {
    inicializar_grafo_lista(destino, origen->num_nodos);    // Inicializa lista
    
    // Recorre matriz triangular superior
    for (int i = 0; i < origen->num_nodos; i++) {
        for (int j = i + 1; j < origen->num_nodos; j++) {
            if (origen->matriz[i][j] > 0) {    // Hay conexión
                agregar_arista_lista(destino, i, j, origen->matriz[i][j]);
            }
        }
    }
}

/*
 * lista_a_matriz
 * Convierte grafo de lista a matriz.
 * Evita duplicados procesando solo una dirección.
 */
void lista_a_matriz(grafo_lista* origen, grafo_matriz* destino) {
    inicializar_grafo_matriz(destino, origen->num_nodos);    // Inicializa matriz
    
    // Recorre todas las listas
    for (int i = 0; i < origen->num_nodos; i++) {
        nodo_lista* actual = origen->lista[i];    // Inicio de lista
        
        while (actual != NULL) {
            // Solo procesa si destino > i (evita duplicados)
            if (actual->destino > i) {
                agregar_arista_matriz(destino, i, actual->destino, actual->peso);
            }
            actual = actual->siguiente;    // Siguiente conexión
        }
    }
}

// ==================== GENERACIÓN DE GRAFOS ====================

/*
 * generar_grafo_aleatorio
 * Crea grafo aleatorio tipo cuadrícula.
 * Conecta vecinos con probabilidad configurable.
 */
void generar_grafo_aleatorio(grafo_matriz* grafo) {
    srand(time(NULL));    // Inicializa generador
    
    // Conecta cada nodo con sus vecinos ortogonales
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            int nodo_actual = i * COLUMNAS + j;    // Calcula índice
            
            // Conexión con nodo de la derecha
            if (j < COLUMNAS - 1) {
                int nodo_derecha = i * COLUMNAS + (j + 1);
                int peso = rand() % 10 + 1;    // Peso aleatorio
                
                // Probabilidad de conectar
                if (rand() % 100 < PROBABILIDAD_CAMINO) {
                    agregar_arista_matriz(grafo, nodo_actual, nodo_derecha, peso);
                }
            }
            
            // Conexión con nodo de abajo
            if (i < FILAS - 1) {
                int nodo_abajo = (i + 1) * COLUMNAS + j;
                int peso = rand() % 10 + 1;    // Peso aleatorio
                
                // Probabilidad de conectar
                if (rand() % 100 < PROBABILIDAD_CAMINO) {
                    agregar_arista_matriz(grafo, nodo_actual, nodo_abajo, peso);
                }
            }
        }
    }
    
    // Agrega conexiones diagonales aleatorias
    for (int k = 0; k < FILAS * COLUMNAS / 4; k++) {
        int nodo1 = rand() % grafo->num_nodos;    // Nodo aleatorio 1
        int nodo2 = rand() % grafo->num_nodos;    // Nodo aleatorio 2
        
        // Verifica que sean diferentes y no conectados
        if (nodo1 != nodo2 && grafo->matriz[nodo1][nodo2] == 0) {
            // Probabilidad de conectar diagonalmente
            if (rand() % 100 < PROBABILIDAD_DIAGONAL) {
                int peso = rand() % 15 + 5;    // Peso 5-19
                agregar_arista_matriz(grafo, nodo1, nodo2, peso);
            }
        }
    }
}

/*
 * generar_grafo_backtracking
 * Genera laberinto usando backtracking.
 * Crea camino que visita todas las celdas.
 */
void generar_grafo_backtracking(grafo_matriz* grafo) {
    srand(time(NULL));    // Inicializa generador
    
    int visitado[MAX_NODOS] = {0};    // Array de visitados
    int pila[MAX_NODOS];              // Pila para backtracking
    int tope = 0;                     // Índice del tope
    
    // Celda inicial aleatoria
    int celda_inicial = rand() % grafo->num_nodos;
    pila[tope++] = celda_inicial;    // Empuja a la pila
    visitado[celda_inicial] = 1;     // Marca como visitado
    
    while (tope > 0) {               // Mientras haya celdas en pila
        int celda_actual = pila[tope - 1];    // Tope de la pila
        
        int i = celda_actual / COLUMNAS;      // Fila de la celda
        int j = celda_actual % COLUMNAS;      // Columna de la celda
        
        int vecinos[4];              // Array de vecinos
        int num_vecinos = 0;         // Contador de vecinos
        
        // Vecino arriba
        if (i > 0) {
            int vecino = (i - 1) * COLUMNAS + j;
            if (!visitado[vecino]) {
                vecinos[num_vecinos++] = vecino;
            }
        }
        
        // Vecino derecha
        if (j < COLUMNAS - 1) {
            int vecino = i * COLUMNAS + (j + 1);
            if (!visitado[vecino]) {
                vecinos[num_vecinos++] = vecino;
            }
        }
        
        // Vecino abajo
        if (i < FILAS - 1) {
            int vecino = (i + 1) * COLUMNAS + j;
            if (!visitado[vecino]) {
                vecinos[num_vecinos++] = vecino;
            }
        }
        
        // Vecino izquierda
        if (j > 0) {
            int vecino = i * COLUMNAS + (j - 1);
            if (!visitado[vecino]) {
                vecinos[num_vecinos++] = vecino;
            }
        }
        
        if (num_vecinos > 0) {
            // Elige vecino aleatorio
            int indice = rand() % num_vecinos;
            int vecino = vecinos[indice];
            
            // Conecta celdas
            int peso = rand() % 10 + 1;    // Peso aleatorio
            agregar_arista_matriz(grafo, celda_actual, vecino, peso);
            
            visitado[vecino] = 1;          // Marca como visitado
            pila[tope++] = vecino;         // Empuja a la pila
        } else {
            tope--;    // Backtrack: retrocede en la pila
        }
    }
}

/*
 * generar_grafo_completo
 * Crea grafo completo (todos con todos).
 * Cada par de nodos está conectado.
 */
void generar_grafo_completo(grafo_matriz* grafo) {
    // Conecta cada par de nodos una vez
    for (int i = 0; i < grafo->num_nodos; i++) {
        for (int j = i + 1; j < grafo->num_nodos; j++) {
            int peso = rand() % 20 + 1;    // Peso 1-20
            agregar_arista_matriz(grafo, i, j, peso);
        }
    }
}

/*
 * generar_grafo_cuadricula
 * Crea cuadrícula regular simple.
 * Solo conexiones ortogonales con peso 1.
 */
void generar_grafo_cuadricula(grafo_matriz* grafo) {
    // Conecta solo vecinos ortogonales
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            int nodo_actual = i * COLUMNAS + j;    // Índice actual
            
            // Conexión derecha
            if (j < COLUMNAS - 1) {
                int nodo_derecha = i * COLUMNAS + (j + 1);
                agregar_arista_matriz(grafo, nodo_actual, nodo_derecha, 1);
            }
            
            // Conexión abajo
            if (i < FILAS - 1) {
                int nodo_abajo = (i + 1) * COLUMNAS + j;
                agregar_arista_matriz(grafo, nodo_actual, nodo_abajo, 1);
            }
        }
    }
}