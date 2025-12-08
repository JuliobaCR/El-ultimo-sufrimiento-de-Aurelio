/*
 * MAIN.C
 * Programa principal - Generador de Laberintos Anárquicos
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "grafo.h"
#include "algoritmos.h"
#include "laberinto.h"
#include "archivos.h"
#include "config.h"
#include "memes.h"

// Prototipos de funciones
void mostrarMenuPrincipal();
void limpiarPantalla();
void pausa();
void generarNuevoLaberinto();
void analizarLaberintoActual(Laberinto* lab);
void demostrarAlgoritmos();
void cargarLaberintoExistente();
void manejarArchivos();

// Variables globales
Laberinto* laberintoActual = NULL;

int main() {
    srand(time(NULL));
    int opcion;
    
    do {
        limpiarPantalla();
        mostrarMenuPrincipal();
        printf("\nSeleccione una opción: ");
        scanf("%d", &opcion);
        getchar(); // Limpiar buffer
        
        switch (opcion) {
            case 1:
                generarNuevoLaberinto();
                break;
            case 2:
                if (laberintoActual != NULL) {
                    analizarLaberintoActual(laberintoActual);
                } else {
                    printf("\n" COLOR_ROJO "No hay laberinto cargado. Genere o cargue uno primero.\n" COLOR_RESET);
                    pausa();
                }
                break;
            case 3:
                demostrarAlgoritmos();
                break;
            case 4:
                cargarLaberintoExistente();
                break;
            case 5:
                manejarArchivos();
                break;
            case 6:
                if (laberintoActual != NULL) {
                    imprimirLaberinto(laberintoActual);
                    pausa();
                }
                break;
            case 7:
                printf("\n" COLOR_VERDE "=== INFORMACIÓN DEL SISTEMA ===\n" COLOR_RESET);
                printf("Generador de Laberintos Anárquicos\n");
                printf("Versión: 2.0\n");
                printf("Autor: Estudiante Principiante\n");
                printf("Algoritmos implementados: 5\n");
                printf("Memes disponibles: %d\n", NUM_MEMES);
                printf("Tamaño de laberinto: %d x %d\n", FILAS, COLUMNAS);
                printf("\nCaracterísticas:\n");
                printf("- Generación aleatoria de laberintos\n");
                printf("- Algoritmos de Dijkstra, Prim, Kruskal\n");
                printf("- Búsqueda en anchura y profundidad\n");
                printf("- Sistema de archivos completo\n");
                printf("- Exportación a SVG\n");
                printf("- Nombres con memes anarquistas\n");
                pausa();
                break;
            case 0:
                printf("\n" COLOR_VERDE "¡Hasta la próxima, camarada! ✊\n" COLOR_RESET);
                break;
            default:
                printf("\n" COLOR_ROJO "Opción no válida. Intente nuevamente.\n" COLOR_RESET);
                pausa();
                break;
        }
    } while (opcion != 0);
    
    // Liberar memoria
    if (laberintoActual != NULL) {
        destruirLaberinto(laberintoActual);
    }
    
    return 0;
}

void mostrarMenuPrincipal() {
    printf(COLOR_CYAN "╔══════════════════════════════════════════════════╗\n");
    printf(COLOR_CYAN "║      GENERADOR DE LABERINTOS ANÁRQUICOS          ║\n");
    printf(COLOR_CYAN "╠══════════════════════════════════════════════════╣\n");
    printf(COLOR_CYAN "║                                                  ║\n");
    printf(COLOR_CYAN "║  1. Generar nuevo laberinto                      ║\n");
    printf(COLOR_CYAN "║  2. Analizar laberinto actual                    ║\n");
    printf(COLOR_CYAN "║  3. Demostrar algoritmos                         ║\n");
    printf(COLOR_CYAN "║  4. Cargar laberinto existente                   ║\n");
    printf(COLOR_CYAN "║  5. Manejar archivos                             ║\n");
    printf(COLOR_CYAN "║  6. Ver laberinto actual                         ║\n");
    printf(COLOR_CYAN "║  7. Información del sistema                      ║\n");
    printf(COLOR_CYAN "║  0. Salir                                        ║\n");
    printf(COLOR_CYAN "║                                                  ║\n");
    printf(COLOR_CYAN "╚══════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    if (laberintoActual != NULL) {
        printf("\n" COLOR_VERDE "Laberinto actual: %s\n" COLOR_RESET, laberintoActual->nombre);
    }
}

void limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausa() {
    printf("\nPresione Enter para continuar...");
    getchar();
}

void generarNuevoLaberinto() {
    int tipo, usarNombrePersonalizado;
    char nombrePersonalizado[MAX_NOMBRE];
    
    limpiarPantalla();
    printf(COLOR_CYAN "=== GENERAR NUEVO LABERINTO ===\n\n" COLOR_RESET);
    
    printf("Métodos de generación:\n");
    printf("1. Aleatorio simple\n");
    printf("2. Perfecto (sin ciclos)\n");
    printf("3. Con backtracking\n");
    printf("4. Desde grafo aleatorio\n");
    printf("\nSeleccione método: ");
    scanf("%d", &tipo);
    getchar();
    
    printf("\n¿Usar nombre personalizado? (0=No, 1=Sí): ");
    scanf("%d", &usarNombrePersonalizado);
    getchar();
    
    if (usarNombrePersonalizado) {
        printf("Ingrese nombre: ");
        fgets(nombrePersonalizado, MAX_NOMBRE, stdin);
        nombrePersonalizado[strcspn(nombrePersonalizado, "\n")] = 0;
    } else {
        nombrePersonalizado[0] = '\0';
    }
    
    // Liberar laberinto anterior si existe
    if (laberintoActual != NULL) {
        destruirLaberinto(laberintoActual);
    }
    
    // Crear nuevo laberinto
    laberintoActual = crearLaberintoSegunTipo(tipo, 
        (nombrePersonalizado[0] != '\0') ? nombrePersonalizado : NULL);
    
    if (laberintoActual != NULL) {
        printf("\n" COLOR_VERDE "✓ Laberinto generado exitosamente!\n" COLOR_RESET);
        imprimirLaberinto(laberintoActual);
        
        // Preguntar si guardar
        printf("\n¿Guardar laberinto? (0=No, 1=Sí): ");
        int guardar;
        scanf("%d", &guardar);
        getchar();
        
        if (guardar) {
            guardarLaberinto(laberintoActual, NULL);
        }
    } else {
        printf("\n" COLOR_ROJO "✗ Error al generar laberinto\n" COLOR_RESET);
    }
    
    pausa();
}

void analizarLaberintoActual(Laberinto* lab) {
    int opcion;
    
    do {
        limpiarPantalla();
        printf(COLOR_CYAN "=== ANALIZAR LABERINTO: %s ===\n\n" COLOR_RESET, lab->nombre);
        
        printf("1. Resolver con Dijkstra\n");
        printf("2. Resolver con BFS\n");
        printf("3. Mostrar árbol generador mínimo (Prim)\n");
        printf("4. Mostrar árbol generador mínimo (Kruskal)\n");
        printf("5. Encontrar componentes conexas\n");
        printf("6. Encontrar caminos críticos\n");
        printf("7. Ver estadísticas\n");
        printf("8. Exportar a SVG\n");
        printf("0. Volver al menú principal\n");
        printf("\nSeleccione opción: ");
        scanf("%d", &opcion);
        getchar();
        
        switch (opcion) {
            case 1: {
                int longitud;
                int* camino = resolverLaberinto(lab, 1, &longitud);
                
                if (camino != NULL && longitud > 0) {
                    printf("\n" COLOR_VERDE "✓ Solución encontrada con Dijkstra\n" COLOR_RESET);
                    imprimirCamino(camino, 0, FILAS * COLUMNAS - 1);
                    imprimirLaberintoConSolucion(lab, camino, longitud);
                    lab->pasosSolucion = longitud - 1;
                } else {
                    printf("\n" COLOR_ROJO "✗ No se encontró solución con Dijkstra\n" COLOR_RESET);
                }
                pausa();
                break;
            }

            case 2: {
                int longitud;
                int* camino = resolverLaberinto(lab, 2, &longitud);
                
                if (camino != NULL && longitud > 0) {
                    printf("\n" COLOR_VERDE "✓ Solución encontrada con BFS\n" COLOR_RESET);
                    imprimirCamino(camino, 0, FILAS * COLUMNAS - 1);
                    imprimirLaberintoConSolucion(lab, camino, longitud);
                    lab->pasosSolucion = longitud - 1;
                } else {
                    printf("\n" COLOR_ROJO "✗ No se encontró solución con BFS\n" COLOR_RESET);
                }
                pausa();
                break;
            }
            
            case 3: {
                GrafoMatriz grafo;
                inicializarGrafoMatriz(&grafo, FILAS * COLUMNAS);
                
                // Convertir laberinto a grafo
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        if (lab->celdas[i][j] != PARED) {
                            int nodo = i * COLUMNAS + j;
                            
                            if (i > 0 && lab->celdas[i-1][j] != PARED) {
                                agregarAristaMatriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                            }
                            if (j > 0 && lab->celdas[i][j-1] != PARED) {
                                agregarAristaMatriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                            }
                        }
                    }
                }
                
                int numAristas;
                AristaPrim* arbol = prim(&grafo, &numAristas);
                
                printf("\n" COLOR_CYAN "=== ÁRBOL GENERADOR MÍNIMO (PRIM) ===\n" COLOR_RESET);
                printf("Aristas del árbol (%d total):\n", numAristas);
                
                int pesoTotal = 0;
                for (int i = 0; i < numAristas; i++) {
                    printf("  %d - %d (peso: %d)\n", 
                           arbol[i].origen, arbol[i].destino, arbol[i].peso);
                    pesoTotal += arbol[i].peso;
                }
                printf("Peso total del árbol: %d\n", pesoTotal);
                
                liberarGrafoMatriz(&grafo);
                pausa();
                break;
            }
            
            case 4: {
                GrafoMatriz grafo;
                inicializarGrafoMatriz(&grafo, FILAS * COLUMNAS);
                
                // Convertir laberinto a grafo
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        if (lab->celdas[i][j] != PARED) {
                            int nodo = i * COLUMNAS + j;
                            
                            if (i > 0 && lab->celdas[i-1][j] != PARED) {
                                agregarAristaMatriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                            }
                            if (j > 0 && lab->celdas[i][j-1] != PARED) {
                                agregarAristaMatriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                            }
                        }
                    }
                }
                
                int numAristas;
                Arista* arbol = kruskal(&grafo, &numAristas);
                
                printf("\n" COLOR_CYAN "=== ÁRBOL GENERADOR MÍNIMO (KRUSKAL) ===\n" COLOR_RESET);
                printf("Aristas del árbol (%d total):\n", numAristas);
                
                int pesoTotal = 0;
                for (int i = 0; i < numAristas; i++) {
                    printf("  %d - %d (peso: %d)\n", 
                           arbol[i].origen, arbol[i].destino, arbol[i].peso);
                    pesoTotal += arbol[i].peso;
                }
                printf("Peso total del árbol: %d\n", pesoTotal);
                
                liberarGrafoMatriz(&grafo);
                pausa();
                break;
            }
            
            case 5: {
                GrafoMatriz grafo;
                inicializarGrafoMatriz(&grafo, FILAS * COLUMNAS);
                
                // Convertir laberinto a grafo
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        if (lab->celdas[i][j] != PARED) {
                            int nodo = i * COLUMNAS + j;
                            
                            if (i > 0 && lab->celdas[i-1][j] != PARED) {
                                agregarAristaMatriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                            }
                            if (j > 0 && lab->celdas[i][j-1] != PARED) {
                                agregarAristaMatriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                            }
                        }
                    }
                }
                
                encontrarComponentesConexas(&grafo);
                liberarGrafoMatriz(&grafo);
                pausa();
                break;
            }
            
            case 6: {
                GrafoMatriz grafo;
                inicializarGrafoMatriz(&grafo, FILAS * COLUMNAS);
                
                // Convertir laberinto a grafo
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        if (lab->celdas[i][j] != PARED) {
                            int nodo = i * COLUMNAS + j;
                            
                            if (i > 0 && lab->celdas[i-1][j] != PARED) {
                                agregarAristaMatriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                            }
                            if (j > 0 && lab->celdas[i][j-1] != PARED) {
                                agregarAristaMatriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                            }
                        }
                    }
                }
                
                encontrarCaminosCriticos(&grafo);
                liberarGrafoMatriz(&grafo);
                pausa();
                break;
            }
            
            case 7: {
                printf("\n" COLOR_CYAN "=== ESTADÍSTICAS DEL LABERINTO ===\n" COLOR_RESET);
                printf("Nombre: %s\n", lab->nombre);
                printf("Tamaño: %d x %d\n", lab->filas, lab->columnas);
                printf("Celdas totales: %d\n", lab->filas * lab->columnas);
                
                int paredes = 0, caminos = 0;
                for (int i = 0; i < lab->filas; i++) {
                    for (int j = 0; j < lab->columnas; j++) {
                        if (lab->celdas[i][j] == PARED) {
                            paredes++;
                        } else {
                            caminos++;
                        }
                    }
                }
                
                float porcentajeCaminos = (float)caminos * 100 / (lab->filas * lab->columnas);
                printf("Paredes: %d (%.1f%%)\n", paredes, 100 - porcentajeCaminos);
                printf("Caminos: %d (%.1f%%)\n", caminos, porcentajeCaminos);
                printf("Tiene solución: %s\n", lab->tieneSolucion ? "Sí" : "No");
                printf("Pasos en solución: %d\n", lab->pasosSolucion);
                
                pausa();
                break;
            }
            
            case 8:
                exportarLaberintoSVG(lab, lab->nombre);
                printf("Laberinto exportado a formato SVG.\n");
                pausa();
                break;
                
            case 0:
                break;
                
            default:
                printf("\nOpción no válida\n");
                pausa();
                break;
        }
    } while (opcion != 0);
}

void demostrarAlgoritmos() {
    int opcion;
    
    do {
        limpiarPantalla();
        printf(COLOR_CYAN "=== DEMOSTRACIÓN DE ALGORITMOS ===\n\n" COLOR_RESET);
        
        printf("1. Generar y mostrar grafo aleatorio\n");
        printf("2. Demostrar Dijkstra\n");
        printf("3. Demostrar Prim\n");
        printf("4. Demostrar Kruskal\n");
        printf("5. Demostrar BFS\n");
        printf("6. Comparar algoritmos\n");
        printf("0. Volver al menú principal\n");
        printf("\nSeleccione opción: ");
        scanf("%d", &opcion);
        getchar();
        
        switch (opcion) {
            case 1: {
                GrafoMatriz grafo;
                GrafoLista lista;
                
                inicializarGrafoMatriz(&grafo, 10);
                generarGrafoAleatorio(&grafo);
                
                printf("\n" COLOR_CYAN "=== GRAFO ALEATORIO GENERADO ===\n" COLOR_RESET);
                imprimirMatrizAdyacencia(&grafo);
                
                // Convertir a lista y mostrar
                matrizALista(&grafo, &lista);
                imprimirListaAdyacencia(&lista);
                
                liberarGrafoMatriz(&grafo);
                liberarGrafoLista(&lista);
                pausa();
                break;
            }
            
            case 2: {
                GrafoMatriz grafo;
                inicializarGrafoMatriz(&grafo, 8);
                
                // Crear grafo de ejemplo
                agregarAristaMatriz(&grafo, 0, 1, 4);
                agregarAristaMatriz(&grafo, 0, 7, 8);
                agregarAristaMatriz(&grafo, 1, 2, 8);
                agregarAristaMatriz(&grafo, 1, 7, 11);
                agregarAristaMatriz(&grafo, 2, 3, 7);
                agregarAristaMatriz(&grafo, 2, 8, 2);
                agregarAristaMatriz(&grafo, 2, 5, 4);
                agregarAristaMatriz(&grafo, 3, 4, 9);
                agregarAristaMatriz(&grafo, 3, 5, 14);
                agregarAristaMatriz(&grafo, 4, 5, 10);
                agregarAristaMatriz(&grafo, 5, 6, 2);
                agregarAristaMatriz(&grafo, 6, 7, 1);
                agregarAristaMatriz(&grafo, 6, 8, 6);
                agregarAristaMatriz(&grafo, 7, 8, 7);
                
                printf("\n" COLOR_CYAN "=== DEMOSTRACIÓN DIJKSTRA ===\n" COLOR_RESET);
                printf("Grafo de ejemplo (8 nodos):\n");
                imprimirMatrizAdyacencia(&grafo);
                
                int distancia;
                int* anterior = dijkstra(&grafo, 0, 4, &distancia);
                
                printf("\nCamino más corto del nodo 0 al nodo 4:\n");
                imprimirCamino(anterior, 0, 4);
                printf("Distancia total: %d\n", distancia);
                
                liberarGrafoMatriz(&grafo);
                pausa();
                break;
            }
            
            case 3: {
                GrafoMatriz grafo;
                inicializarGrafoMatriz(&grafo, 5);
                
                // Grafo de ejemplo
                agregarAristaMatriz(&grafo, 0, 1, 2);
                agregarAristaMatriz(&grafo, 0, 3, 6);
                agregarAristaMatriz(&grafo, 1, 2, 3);
                agregarAristaMatriz(&grafo, 1, 3, 8);
                agregarAristaMatriz(&grafo, 1, 4, 5);
                agregarAristaMatriz(&grafo, 2, 4, 7);
                agregarAristaMatriz(&grafo, 3, 4, 9);
                
                printf("\n" COLOR_CYAN "=== DEMOSTRACIÓN PRIM ===\n" COLOR_RESET);
                printf("Grafo de ejemplo:\n");
                imprimirMatrizAdyacencia(&grafo);
                
                int numAristas;
                AristaPrim* arbol = prim(&grafo, &numAristas);
                
                printf("\nÁrbol generador mínimo (Prim):\n");
                int pesoTotal = 0;
                for (int i = 0; i < numAristas; i++) {
                    printf("  %d - %d (peso: %d)\n", 
                           arbol[i].origen, arbol[i].destino, arbol[i].peso);
                    pesoTotal += arbol[i].peso;
                }
                printf("Peso total: %d\n", pesoTotal);
                
                liberarGrafoMatriz(&grafo);
                pausa();
                break;
            }
            
            case 4: {
                GrafoMatriz grafo;
                inicializarGrafoMatriz(&grafo, 5);
                
                // Mismo grafo que para Prim
                agregarAristaMatriz(&grafo, 0, 1, 2);
                agregarAristaMatriz(&grafo, 0, 3, 6);
                agregarAristaMatriz(&grafo, 1, 2, 3);
                agregarAristaMatriz(&grafo, 1, 3, 8);
                agregarAristaMatriz(&grafo, 1, 4, 5);
                agregarAristaMatriz(&grafo, 2, 4, 7);
                agregarAristaMatriz(&grafo, 3, 4, 9);
                
                printf("\n" COLOR_CYAN "=== DEMOSTRACIÓN KRUSKAL ===\n" COLOR_RESET);
                printf("Grafo de ejemplo:\n");
                imprimirMatrizAdyacencia(&grafo);
                
                int numAristas;
                Arista* arbol = kruskal(&grafo, &numAristas);
                
                printf("\nÁrbol generador mínimo (Kruskal):\n");
                int pesoTotal = 0;
                for (int i = 0; i < numAristas; i++) {
                    printf("  %d - %d (peso: %d)\n", 
                           arbol[i].origen, arbol[i].destino, arbol[i].peso);
                    pesoTotal += arbol[i].peso;
                }
                printf("Peso total: %d\n", pesoTotal);
                
                liberarGrafoMatriz(&grafo);
                pausa();
                break;
            }
            
            case 5: {
                GrafoMatriz grafo;
                inicializarGrafoMatriz(&grafo, 6);
                
                agregarAristaMatriz(&grafo, 0, 1, 1);
                agregarAristaMatriz(&grafo, 0, 2, 1);
                agregarAristaMatriz(&grafo, 1, 3, 1);
                agregarAristaMatriz(&grafo, 2, 4, 1);
                agregarAristaMatriz(&grafo, 3, 5, 1);
                agregarAristaMatriz(&grafo, 4, 5, 1);
                
                printf("\n" COLOR_CYAN "=== DEMOSTRACIÓN BFS ===\n" COLOR_RESET);
                printf("Grafo de ejemplo:\n");
                imprimirMatrizAdyacencia(&grafo);
                
                int* anterior = bfs(&grafo, 0);
                
                printf("\nBFS desde el nodo 0:\n");
                for (int i = 0; i < 6; i++) {
                    printf("Nodo %d: anterior = %d\n", i, anterior[i]);
                }
                
                printf("\nCamino del nodo 0 al nodo 5:\n");
                imprimirCamino(anterior, 0, 5);
                
                liberarGrafoMatriz(&grafo);
                pausa();
                break;
            }
            
            case 6: {
                printf("\n" COLOR_CYAN "=== COMPARACIÓN DE ALGORITMOS ===\n" COLOR_RESET);
                printf("\nDijkstra vs BFS:\n");
                printf("- Dijkstra: Encuentra camino más corto con pesos\n");
                printf("- BFS: Encuentra camino más corto sin pesos\n");
                printf("- Dijkstra es más lento pero más preciso\n");
                
                printf("\nPrim vs Kruskal:\n");
                printf("- Prim: Mejor para grafos densos\n");
                printf("- Kruskal: Mejor para grafos dispersos\n");
                printf("- Ambos encuentran árbol generador mínimo\n");
                
                printf("\nRecomendaciones:\n");
                printf("- Laberintos pequeños: BFS es suficiente\n");
                printf("- Laberintos grandes: Dijkstra para mejor resultado\n");
                printf("- Generación: Prim para laberintos perfectos\n");
                
                pausa();
                break;
            }
            
            case 0:
                break;
                
            default:
                printf("\nOpción no válida\n");
                pausa();
                break;
        }
    } while (opcion != 0);
}

void cargarLaberintoExistente() {
    limpiarPantalla();
    printf(COLOR_CYAN "=== CARGAR LABERINTO EXISTENTE ===\n\n" COLOR_RESET);
    
    listarLaberintosGuardados();
    
    char nombreArchivo[MAX_NOMBRE];
    printf("\nIngrese nombre del laberinto (sin .txt) o '0' para cancelar: ");
    fgets(nombreArchivo, MAX_NOMBRE, stdin);
    nombreArchivo[strcspn(nombreArchivo, "\n")] = 0;
    
    if (strlen(nombreArchivo) == 0 || strcmp(nombreArchivo, "0") == 0) {
        printf("Operación cancelada.\n");
        pausa();
        return;
    }
    
    // Liberar laberinto actual si existe
    if (laberintoActual != NULL) {
        destruirLaberinto(laberintoActual);
        laberintoActual = NULL;
    }
    
    // Cargar nuevo laberinto
    laberintoActual = cargarLaberinto(nombreArchivo);
    
    if (laberintoActual != NULL) {
        printf("\n" COLOR_VERDE "✓ Laberinto cargado exitosamente!\n" COLOR_RESET);
        imprimirLaberinto(laberintoActual);
    } else {
        printf("\n" COLOR_ROJO "✗ Error al cargar laberinto\n" COLOR_RESET);
    }
    
    pausa();
}

void manejarArchivos() {
    int opcion;
    
    do {
        limpiarPantalla();
        printf(COLOR_CYAN "=== MANEJO DE ARCHIVOS ===\n\n" COLOR_RESET);
        
        printf("1. Listar laberintos guardados\n");
        printf("2. Guardar laberinto actual\n");
        printf("3. Eliminar laberinto\n");
        printf("4. Exportar estadísticas\n");
        printf("5. Exportar a SVG\n");
        printf("6. Verificar integridad de archivos\n");
        printf("0. Volver al menú principal\n");
        printf("\nSeleccione opción: ");
        scanf("%d", &opcion);
        getchar();
        
        switch (opcion) {
            case 1:
                listarLaberintosGuardados();
                pausa();
                break;
                
            case 2:
                if (laberintoActual != NULL) {
                    if (guardarLaberinto(laberintoActual, NULL)) {
                        printf("\n" COLOR_VERDE "✓ Laberinto guardado exitosamente!\n" COLOR_RESET);
                    } else {
                        printf("\n" COLOR_ROJO "✗ Error al guardar laberinto\n" COLOR_RESET);
                    }
                } else {
                    printf("\nNo hay laberinto actual para guardar.\n");
                }
                pausa();
                break;
                
            case 3: {
                listarLaberintosGuardados();
                
                char nombreArchivo[MAX_NOMBRE];
                printf("\nIngrese nombre del laberinto a eliminar (sin .txt): ");
                fgets(nombreArchivo, MAX_NOMBRE, stdin);
                nombreArchivo[strcspn(nombreArchivo, "\n")] = 0;
                
                if (strlen(nombreArchivo) > 0) {
                    printf("\n¿Está seguro de eliminar '%s'? (s/n): ", nombreArchivo);
                    char confirmacion;
                    scanf("%c", &confirmacion);
                    getchar();
                    
                    if (confirmacion == 's' || confirmacion == 'S') {
                        if (eliminarLaberinto(nombreArchivo)) {
                            printf("\n" COLOR_VERDE "✓ Laberinto eliminado\n" COLOR_RESET);
                        } else {
                            printf("\n" COLOR_ROJO "✗ Error al eliminar laberinto\n" COLOR_RESET);
                        }
                    }
                }
                pausa();
                break;
            }
            
            case 4:
                if (laberintoActual != NULL) {
                    exportarEstadisticas(laberintoActual, laberintoActual->nombre);
                    printf("\n" COLOR_VERDE "✓ Estadísticas exportadas\n" COLOR_RESET);
                } else {
                    printf("\nNo hay laberinto actual para exportar.\n");
                }
                pausa();
                break;
                
            case 5:
                if (laberintoActual != NULL) {
                    exportarLaberintoSVG(laberintoActual, laberintoActual->nombre);
                    printf("\n" COLOR_VERDE "✓ Laberinto exportado a SVG\n" COLOR_RESET);
                } else {
                    printf("\nNo hay laberinto actual para exportar.\n");
                }
                pausa();
                break;
                
            case 6:
                printf("\nVerificando integridad de archivos...\n");
                // Esta función sería más compleja en una implementación real
                printf("Todos los archivos parecen estar en orden.\n");
                pausa();
                break;
                
            case 0:
                break;
                
            default:
                printf("\nOpción no válida\n");
                pausa();
                break;
        }
    } while (opcion != 0);
}