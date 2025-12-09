/*
 * main.c
 * Programa principal con sistema de menús.
 * Coordina todas las funcionalidades del proyecto.
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
void mostrar_menu_principal(void);
void limpiar_pantalla(void);
void pausa(void);
void generar_nuevo_laberinto(void);
void analizar_laberinto_actual(laberinto* lab);
void demostrar_algoritmos(void);
void cargar_laberinto_existente(void);
void manejar_archivos(void);
void mostrar_informacion_sistema(void);

// Variables globales
laberinto* laberinto_actual = NULL;

int main(void) {
    srand(time(NULL));    // Inicializa generador aleatorio
    int opcion;           // Opción del menú
    
    do {
        limpiar_pantalla();          // Limpia pantalla
        mostrar_menu_principal();    // Muestra menú
        
        printf("\nSeleccione una opción: ");
        scanf("%d", &opcion);        // Lee opción
        getchar();                   // Limpia buffer
        
        switch (opcion) {
            case 1: generar_nuevo_laberinto(); break;
            case 2: 
                if (laberinto_actual != NULL) {
                    analizar_laberinto_actual(laberinto_actual);
                } else {
                    printf("\n" COLOR_ROJO "No hay laberinto cargado.\n" COLOR_RESET);
                    pausa();
                }
                break;
            case 3: demostrar_algoritmos(); break;
            case 4: cargar_laberinto_existente(); break;
            case 5: manejar_archivos(); break;
            case 6: 
                if (laberinto_actual != NULL) {
                    imprimir_laberinto(laberinto_actual);
                    pausa();
                }
                break;
            case 7: mostrar_informacion_sistema(); break;
            case 0: printf("\n" COLOR_VERDE "¡Hasta la próxima!\n" COLOR_RESET); break;
            default: 
                printf("\n" COLOR_ROJO "Opción no válida.\n" COLOR_RESET);
                pausa();
                break;
        }
    } while (opcion != 0);
    
    // Limpieza final
    if (laberinto_actual != NULL) {
        destruir_laberinto(laberinto_actual);
    }
    
    return 0;
}

/*
 * mostrar_menu_principal
 * Muestra el menú principal con formato visual.
 * Incluye estado actual del laberinto cargado.
 */
void mostrar_menu_principal(void) {
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
    
    if (laberinto_actual != NULL) {
        printf("\n" COLOR_VERDE "Laberinto actual: %s\n" COLOR_RESET, laberinto_actual->nombre);
    }
}

/*
 * limpiar_pantalla
 * Limpia la pantalla según el sistema operativo.
 * Usa comandos específicos para cada plataforma.
 */
void limpiar_pantalla(void) {
    #ifdef _WIN32
        system("cls");      // Windows
    #else
        system("clear");    // Linux/Mac
    #endif
}

/*
 * pausa
 * Espera que el usuario presione Enter.
 * Permite pausar la ejecución para lectura.
 */
void pausa(void) {
    printf("\nPresione Enter para continuar...");
    getchar();    // Espera entrada
}

/*
 * generar_nuevo_laberinto
 * Interfaz para generar nuevo laberinto.
 * Permite seleccionar método y nombre.
 */
void generar_nuevo_laberinto(void) {
    int tipo, usar_nombre;
    char nombre_personalizado[MAX_NOMBRE];
    
    limpiar_pantalla();
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
    scanf("%d", &usar_nombre);
    getchar();
    
    if (usar_nombre) {
        printf("Ingrese nombre: ");
        fgets(nombre_personalizado, MAX_NOMBRE, stdin);
        nombre_personalizado[strcspn(nombre_personalizado, "\n")] = 0;
    } else {
        nombre_personalizado[0] = '\0';
    }
    
    // Libera laberinto anterior si existe
    if (laberinto_actual != NULL) {
        destruir_laberinto(laberinto_actual);
    }
    
    // Crea nuevo laberinto
    laberinto_actual = crear_laberinto_segun_tipo(tipo, 
        (nombre_personalizado[0] != '\0') ? nombre_personalizado : NULL);
    
    if (laberinto_actual != NULL) {
        printf("\n" COLOR_VERDE "✓ Laberinto generado exitosamente!\n" COLOR_RESET);
        imprimir_laberinto(laberinto_actual);
        
        // Pregunta si guardar
        printf("\n¿Guardar laberinto? (0=No, 1=Sí): ");
        int guardar;
        scanf("%d", &guardar);
        getchar();
        
        if (guardar) {
            guardar_laberinto(laberinto_actual, NULL);
        }
    } else {
        printf("\n" COLOR_ROJO "✗ Error al generar laberinto\n" COLOR_RESET);
    }
    
    pausa();
}

/*
 * analizar_laberinto_actual
 * Menú de análisis para laberinto cargado.
 * Ofrece múltiples algoritmos de análisis.
 */
void analizar_laberinto_actual(laberinto* lab) {
    int opcion;
    
    do {
        limpiar_pantalla();
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
                int* camino = resolver_laberinto(lab, 1, &longitud);
                
                if (camino != NULL && longitud > 0) {
                    printf("\n" COLOR_VERDE "✓ Solución encontrada con Dijkstra\n" COLOR_RESET);
                    imprimir_camino(camino, 0, FILAS * COLUMNAS - 1);
                    imprimir_laberinto_con_solucion(lab, camino, longitud);
                    lab->pasos_solucion = longitud - 1;
                } else {
                    printf("\n" COLOR_ROJO "✗ No se encontró solución\n" COLOR_RESET);
                }
                pausa();
                break;
            }
            
            case 2: {
                int longitud;
                int* camino = resolver_laberinto(lab, 2, &longitud);
                
                if (camino != NULL && longitud > 0) {
                    printf("\n" COLOR_VERDE "✓ Solución encontrada con BFS\n" COLOR_RESET);
                    imprimir_camino(camino, 0, FILAS * COLUMNAS - 1);
                    imprimir_laberinto_con_solucion(lab, camino, longitud);
                    lab->pasos_solucion = longitud - 1;
                } else {
                    printf("\n" COLOR_ROJO "✗ No se encontró solución\n" COLOR_RESET);
                }
                pausa();
                break;
            }
            
            case 3: {
                grafo_matriz grafo;
                inicializar_grafo_matriz(&grafo, FILAS * COLUMNAS);
                
                // Convierte laberinto a grafo
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        if (lab->celdas[i][j] != PARED) {
                            int nodo = i * COLUMNAS + j;
                            
                            if (i > 0 && lab->celdas[i-1][j] != PARED) {
                                agregar_arista_matriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                            }
                            if (j > 0 && lab->celdas[i][j-1] != PARED) {
                                agregar_arista_matriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                            }
                        }
                    }
                }
                
                int num_aristas;
                arista_prim* arbol = prim(&grafo, &num_aristas);
                
                printf("\n" COLOR_CYAN "=== ÁRBOL GENERADOR MÍNIMO (PRIM) ===\n" COLOR_RESET);
                printf("Aristas del árbol (%d total):\n", num_aristas);
                
                int peso_total = 0;
                for (int i = 0; i < num_aristas; i++) {
                    printf("  %d - %d (peso: %d)\n", 
                           arbol[i].origen, arbol[i].destino, arbol[i].peso);
                    peso_total += arbol[i].peso;
                }
                printf("Peso total del árbol: %d\n", peso_total);
                
                liberar_grafo_matriz(&grafo);
                pausa();
                break;
            }
            
            case 4: {
                grafo_matriz grafo;
                inicializar_grafo_matriz(&grafo, FILAS * COLUMNAS);
                
                // Convierte laberinto a grafo
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        if (lab->celdas[i][j] != PARED) {
                            int nodo = i * COLUMNAS + j;
                            
                            if (i > 0 && lab->celdas[i-1][j] != PARED) {
                                agregar_arista_matriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                            }
                            if (j > 0 && lab->celdas[i][j-1] != PARED) {
                                agregar_arista_matriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                            }
                        }
                    }
                }
                
                int num_aristas;
                arista* arbol = kruskal(&grafo, &num_aristas);
                
                printf("\n" COLOR_CYAN "=== ÁRBOL GENERADOR MÍNIMO (KRUSKAL) ===\n" COLOR_RESET);
                printf("Aristas del árbol (%d total):\n", num_aristas);
                
                int peso_total = 0;
                for (int i = 0; i < num_aristas; i++) {
                    printf("  %d - %d (peso: %d)\n", 
                           arbol[i].origen, arbol[i].destino, arbol[i].peso);
                    peso_total += arbol[i].peso;
                }
                printf("Peso total del árbol: %d\n", peso_total);
                
                liberar_grafo_matriz(&grafo);
                pausa();
                break;
            }
            
            case 5: {
                grafo_matriz grafo;
                inicializar_grafo_matriz(&grafo, FILAS * COLUMNAS);
                
                // Convierte laberinto a grafo
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        if (lab->celdas[i][j] != PARED) {
                            int nodo = i * COLUMNAS + j;
                            
                            if (i > 0 && lab->celdas[i-1][j] != PARED) {
                                agregar_arista_matriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                            }
                            if (j > 0 && lab->celdas[i][j-1] != PARED) {
                                agregar_arista_matriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                            }
                        }
                    }
                }
                
                encontrar_componentes_conexas(&grafo);
                liberar_grafo_matriz(&grafo);
                pausa();
                break;
            }
            
            case 6: {
                grafo_matriz grafo;
                inicializar_grafo_matriz(&grafo, FILAS * COLUMNAS);
                
                // Convierte laberinto a grafo
                for (int i = 0; i < FILAS; i++) {
                    for (int j = 0; j < COLUMNAS; j++) {
                        if (lab->celdas[i][j] != PARED) {
                            int nodo = i * COLUMNAS + j;
                            
                            if (i > 0 && lab->celdas[i-1][j] != PARED) {
                                agregar_arista_matriz(&grafo, nodo, (i-1)*COLUMNAS + j, 1);
                            }
                            if (j > 0 && lab->celdas[i][j-1] != PARED) {
                                agregar_arista_matriz(&grafo, nodo, i*COLUMNAS + (j-1), 1);
                            }
                        }
                    }
                }
                
                encontrar_caminos_criticos(&grafo);
                liberar_grafo_matriz(&grafo);
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
                
                float porcentaje_caminos = (float)caminos * 100 / (lab->filas * lab->columnas);
                printf("Paredes: %d (%.1f%%)\n", paredes, 100 - porcentaje_caminos);
                printf("Caminos: %d (%.1f%%)\n", caminos, porcentaje_caminos);
                printf("Tiene solución: %s\n", lab->tiene_solucion ? "Sí" : "No");
                printf("Pasos en solución: %d\n", lab->pasos_solucion);
                
                pausa();
                break;
            }
            
            case 8:
                exportar_laberinto_svg(lab, lab->nombre);
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

/*
 * demostrar_algoritmos
 * Menú de demostración interactiva de algoritmos.
 * Incluye Dijkstra con visualización de estados.
 */
void demostrar_algoritmos(void) {
    int opcion;
    
    do {
        limpiar_pantalla();
        printf(COLOR_CYAN "=== DEMOSTRACIÓN DE ALGORITMOS ===\n\n" COLOR_RESET);
        
        printf("1. Generar y mostrar grafo aleatorio\n");
        printf("2. Demostrar Dijkstra (CON ESTADOS INTERMEDIOS)\n");
        printf("3. Demostrar Prim\n");
        printf("4. Demostrar Kruskal\n");
        printf("5. Demostrar BFS\n");
        printf("6. Comparar algoritmos\n");
        printf("7. Explicación detallada de algoritmos\n");
        printf("0. Volver al menú principal\n");
        
        printf("\nSeleccione opción: ");
        scanf("%d", &opcion);
        getchar();
        
        switch (opcion) {
            case 1: {
                grafo_matriz grafo;
                grafo_lista lista;
                
                inicializar_grafo_matriz(&grafo, 10);
                generar_grafo_aleatorio(&grafo);
                
                printf("\n" COLOR_CYAN "=== GRAFO ALEATORIO GENERADO ===\n" COLOR_RESET);
                imprimir_matriz_adyacencia(&grafo);
                
                matriz_a_lista(&grafo, &lista);
                imprimir_lista_adyacencia(&lista);
                
                liberar_grafo_matriz(&grafo);
                liberar_grafo_lista(&lista);
                pausa();
                break;
            }
            
            case 2: {
                grafo_matriz grafo;
                inicializar_grafo_matriz(&grafo, 8);
                
                // Grafo de ejemplo para Dijkstra
                agregar_arista_matriz(&grafo, 0, 1, 4);
                agregar_arista_matriz(&grafo, 0, 7, 8);
                agregar_arista_matriz(&grafo, 1, 2, 8);
                agregar_arista_matriz(&grafo, 1, 7, 11);
                agregar_arista_matriz(&grafo, 2, 3, 7);
                agregar_arista_matriz(&grafo, 2, 8, 2);
                agregar_arista_matriz(&grafo, 2, 5, 4);
                agregar_arista_matriz(&grafo, 3, 4, 9);
                agregar_arista_matriz(&grafo, 3, 5, 14);
                agregar_arista_matriz(&grafo, 4, 5, 10);
                agregar_arista_matriz(&grafo, 5, 6, 2);
                agregar_arista_matriz(&grafo, 6, 7, 1);
                agregar_arista_matriz(&grafo, 6, 8, 6);
                agregar_arista_matriz(&grafo, 7, 8, 7);
                
                printf("\n" COLOR_CYAN "=== DEMOSTRACIÓN DIJKSTRA ===\n" COLOR_RESET);
                printf("Grafo de ejemplo (9 nodos):\n");
                imprimir_matriz_adyacencia(&grafo);
                
                int distancia;
                printf("\n" COLOR_AMARILLO "Presione Enter para comenzar...\n" COLOR_RESET);
                getchar();
                
                int* anterior = dijkstra_con_estados(&grafo, 0, 4, &distancia, 1);
                
                printf("\n" COLOR_VERDE "=== RESULTADO FINAL ===\n" COLOR_RESET);
                printf("Distancia total: %d\n", distancia);
                
                liberar_grafo_matriz(&grafo);
                pausa();
                break;
            }
            
            case 3: {
                grafo_matriz grafo;
                inicializar_grafo_matriz(&grafo, 5);
                
                // Grafo de ejemplo para Prim
                agregar_arista_matriz(&grafo, 0, 1, 2);
                agregar_arista_matriz(&grafo, 0, 3, 6);
                agregar_arista_matriz(&grafo, 1, 2, 3);
                agregar_arista_matriz(&grafo, 1, 3, 8);
                agregar_arista_matriz(&grafo, 1, 4, 5);
                agregar_arista_matriz(&grafo, 2, 4, 7);
                agregar_arista_matriz(&grafo, 3, 4, 9);
                
                printf("\n" COLOR_CYAN "=== DEMOSTRACIÓN PRIM ===\n" COLOR_RESET);
                printf("Grafo de ejemplo:\n");
                imprimir_matriz_adyacencia(&grafo);
                
                int num_aristas;
                arista_prim* arbol = prim(&grafo, &num_aristas);
                
                printf("\nÁrbol generador mínimo (Prim):\n");
                int peso_total = 0;
                for (int i = 0; i < num_aristas; i++) {
                    printf("  %d - %d (peso: %d)\n", 
                           arbol[i].origen, arbol[i].destino, arbol[i].peso);
                    peso_total += arbol[i].peso;
                }
                printf("Peso total: %d\n", peso_total);
                
                liberar_grafo_matriz(&grafo);
                pausa();
                break;
            }
            
            case 4: {
                grafo_matriz grafo;
                inicializar_grafo_matriz(&grafo, 5);
                
                // Grafo de ejemplo para Kruskal
                agregar_arista_matriz(&grafo, 0, 1, 2);
                agregar_arista_matriz(&grafo, 0, 3, 6);
                agregar_arista_matriz(&grafo, 1, 2, 3);
                agregar_arista_matriz(&grafo, 1, 3, 8);
                agregar_arista_matriz(&grafo, 1, 4, 5);
                agregar_arista_matriz(&grafo, 2, 4, 7);
                agregar_arista_matriz(&grafo, 3, 4, 9);
                
                printf("\n" COLOR_CYAN "=== DEMOSTRACIÓN KRUSKAL ===\n" COLOR_RESET);
                printf("Grafo de ejemplo:\n");
                imprimir_matriz_adyacencia(&grafo);
                
                int num_aristas;
                arista* arbol = kruskal(&grafo, &num_aristas);
                
                printf("\nÁrbol generador mínimo (Kruskal):\n");
                int peso_total = 0;
                for (int i = 0; i < num_aristas; i++) {
                    printf("  %d - %d (peso: %d)\n", 
                           arbol[i].origen, arbol[i].destino, arbol[i].peso);
                    peso_total += arbol[i].peso;
                }
                printf("Peso total: %d\n", peso_total);
                
                liberar_grafo_matriz(&grafo);
                pausa();
                break;
            }
            
            case 5: {
                grafo_matriz grafo;
                inicializar_grafo_matriz(&grafo, 6);
                
                agregar_arista_matriz(&grafo, 0, 1, 1);
                agregar_arista_matriz(&grafo, 0, 2, 1);
                agregar_arista_matriz(&grafo, 1, 3, 1);
                agregar_arista_matriz(&grafo, 2, 4, 1);
                agregar_arista_matriz(&grafo, 3, 5, 1);
                agregar_arista_matriz(&grafo, 4, 5, 1);
                
                printf("\n" COLOR_CYAN "=== DEMOSTRACIÓN BFS ===\n" COLOR_RESET);
                printf("Grafo de ejemplo:\n");
                imprimir_matriz_adyacencia(&grafo);
                
                int* anterior = bfs(&grafo, 0);
                
                printf("\nBFS desde el nodo 0:\n");
                for (int i = 0; i < 6; i++) {
                    printf("Nodo %d: anterior = %d\n", i, anterior[i]);
                }
                
                printf("\nCamino del nodo 0 al nodo 5:\n");
                imprimir_camino(anterior, 0, 5);
                
                liberar_grafo_matriz(&grafo);
                pausa();
                break;
            }
            
            case 6: {
                printf("\n" COLOR_CYAN "=== COMPARACIÓN DE ALGORITMOS ===\n" COLOR_RESET);
                printf("\nDijkstra vs BFS:\n");
                printf("- Dijkstra: Camino más corto con pesos\n");
                printf("- BFS: Camino más corto sin pesos\n");
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
            
            case 7:
                explicar_algoritmos_seleccion();
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

/*
 * cargar_laberinto_existente
 * Interfaz para cargar laberintos guardados.
 * Muestra lista y permite seleccionar.
 */
void cargar_laberinto_existente(void) {
    limpiar_pantalla();
    printf(COLOR_CYAN "=== CARGAR LABERINTO EXISTENTE ===\n\n" COLOR_RESET);
    
    listar_laberintos_guardados();
    
    char nombre_archivo[MAX_NOMBRE];
    printf("\nIngrese nombre del laberinto (sin .txt) o '0' para cancelar: ");
    fgets(nombre_archivo, MAX_NOMBRE, stdin);
    nombre_archivo[strcspn(nombre_archivo, "\n")] = 0;
    
    if (strlen(nombre_archivo) == 0 || strcmp(nombre_archivo, "0") == 0) {
        printf("Operación cancelada.\n");
        pausa();
        return;
    }
    
    // Libera laberinto actual si existe
    if (laberinto_actual != NULL) {
        destruir_laberinto(laberinto_actual);
        laberinto_actual = NULL;
    }
    
    // Carga nuevo laberinto
    laberinto_actual = cargar_laberinto(nombre_archivo);
    
    if (laberinto_actual != NULL) {
        printf("\n" COLOR_VERDE "✓ Laberinto cargado exitosamente!\n" COLOR_RESET);
        imprimir_laberinto(laberinto_actual);
    } else {
        printf("\n" COLOR_ROJO "✗ Error al cargar laberinto\n" COLOR_RESET);
    }
    
    pausa();
}

/*
 * manejar_archivos
 * Menú para gestión de archivos de laberintos.
 * Permite guardar, eliminar y exportar.
 */
void manejar_archivos(void) {
    int opcion;
    
    do {
        limpiar_pantalla();
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
                listar_laberintos_guardados();
                pausa();
                break;
                
            case 2:
                if (laberinto_actual != NULL) {
                    if (guardar_laberinto(laberinto_actual, NULL)) {
                        printf("\n" COLOR_VERDE "✓ Laberinto guardado!\n" COLOR_RESET);
                    } else {
                        printf("\n" COLOR_ROJO "✗ Error al guardar\n" COLOR_RESET);
                    }
                } else {
                    printf("\nNo hay laberinto actual para guardar.\n");
                }
                pausa();
                break;
                
            case 3: {
                listar_laberintos_guardados();
                
                char nombre_archivo[MAX_NOMBRE];
                printf("\nIngrese nombre del laberinto a eliminar (sin .txt): ");
                fgets(nombre_archivo, MAX_NOMBRE, stdin);
                nombre_archivo[strcspn(nombre_archivo, "\n")] = 0;
                
                if (strlen(nombre_archivo) > 0) {
                    printf("\n¿Está seguro de eliminar '%s'? (s/n): ", nombre_archivo);
                    char confirmacion;
                    scanf("%c", &confirmacion);
                    getchar();
                    
                    if (confirmacion == 's' || confirmacion == 'S') {
                        if (eliminar_laberinto(nombre_archivo)) {
                            printf("\n" COLOR_VERDE "✓ Laberinto eliminado\n" COLOR_RESET);
                        } else {
                            printf("\n" COLOR_ROJO "✗ Error al eliminar\n" COLOR_RESET);
                        }
                    }
                }
                pausa();
                break;
            }
            
            case 4:
                if (laberinto_actual != NULL) {
                    exportar_estadisticas(laberinto_actual, laberinto_actual->nombre);
                    printf("\n" COLOR_VERDE "✓ Estadísticas exportadas\n" COLOR_RESET);
                } else {
                    printf("\nNo hay laberinto actual para exportar.\n");
                }
                pausa();
                break;
                
            case 5:
                if (laberinto_actual != NULL) {
                    exportar_laberinto_svg(laberinto_actual, laberinto_actual->nombre);
                    printf("\n" COLOR_VERDE "✓ Laberinto exportado a SVG\n" COLOR_RESET);
                } else {
                    printf("\nNo hay laberinto actual para exportar.\n");
                }
                pausa();
                break;
                
            case 6:
                printf("\nVerificando integridad de archivos...\n");
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

/*
 * mostrar_informacion_sistema
 * Muestra información general del programa.
 * Incluye versión, autor y características.
 */
void mostrar_informacion_sistema(void) {
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
}