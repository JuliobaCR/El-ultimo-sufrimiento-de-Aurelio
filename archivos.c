/*
 * ARCHIVOS.C
 * Implementación de manejo de archivos
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include "archivos.h"
#include "config.h"

// ==================== FUNCIONES PRINCIPALES ====================

int guardarLaberinto(Laberinto* lab, const char* nombreArchivo) {
    FILE* archivo;
    char nombreCompleto[MAX_NOMBRE + 50];
    
    // Crear directorio si no existe
    crearDirectorioLaberintos();
    
    // Generar nombre completo
    if (nombreArchivo == NULL) {
        sprintf(nombreCompleto, "%s%s.txt", DIRECTORIO_LABERINTOS, lab->nombre);
    } else {
        sprintf(nombreCompleto, "%s%s", DIRECTORIO_LABERINTOS, nombreArchivo);
        if (strstr(nombreArchivo, ".txt") == NULL) {
            strcat(nombreCompleto, ".txt");
        }
    }
    
    // Abrir archivo
    archivo = fopen(nombreCompleto, "w");
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo %s\n", nombreCompleto);
        return 0;
    }
    
    // Escribir metadatos
    fprintf(archivo, "# LABERINTO ANARQUICO\n");
    fprintf(archivo, "NOMBRE: %s\n", lab->nombre);
    fprintf(archivo, "FILAS: %d\n", lab->filas);
    fprintf(archivo, "COLUMNAS: %d\n", lab->columnas);
    fprintf(archivo, "TIENE_SOLUCION: %d\n", lab->tieneSolucion);
    fprintf(archivo, "PASOS_SOLUCION: %d\n", lab->pasosSolucion);
    fprintf(archivo, "DATA:\n");
    
    // Escribir laberinto
    for (int i = 0; i < lab->filas; i++) {
        for (int j = 0; j < lab->columnas; j++) {
            fprintf(archivo, "%d", lab->celdas[i][j]);
            if (j < lab->columnas - 1) {
                fprintf(archivo, " ");
            }
        }
        fprintf(archivo, "\n");
    }
    
    fclose(archivo);
    printf("Laberinto guardado en: %s\n", nombreCompleto);
    return 1;
}

Laberinto* cargarLaberinto(const char* nombreArchivo) {
    FILE* archivo;
    char nombreCompleto[MAX_NOMBRE + 50];
    char linea[MAX_LINEA];
    Laberinto* lab = NULL;
    
    // Construir nombre completo
    sprintf(nombreCompleto, "%s%s", DIRECTORIO_LABERINTOS, nombreArchivo);
    if (strstr(nombreArchivo, ".txt") == NULL) {
        strcat(nombreCompleto, ".txt");
    }
    
    // Verificar si existe
    if (!existeLaberinto(nombreArchivo)) {
        printf("Error: El archivo %s no existe\n", nombreCompleto);
        return NULL;
    }
    
    // Abrir archivo
    archivo = fopen(nombreCompleto, "r");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", nombreCompleto);
        return NULL;
    }
    
    // Crear laberinto
    lab = (Laberinto*)malloc(sizeof(Laberinto));
    if (!lab) {
        fclose(archivo);
        return NULL;
    }
    
    // Leer metadatos
    int leyendoData = 0;
    int filaActual = 0;
    
    while (fgets(linea, MAX_LINEA, archivo)) {
        // Eliminar salto de línea
        linea[strcspn(linea, "\n")] = 0;
        
        if (strstr(linea, "DATA:") != NULL) {
            leyendoData = 1;
            continue;
        }
        
        if (!leyendoData) {
            // Parsear metadatos
            if (strstr(linea, "NOMBRE:") != NULL) {
                strcpy(lab->nombre, linea + 8);
            } else if (strstr(linea, "FILAS:") != NULL) {
                lab->filas = atoi(linea + 6);
            } else if (strstr(linea, "COLUMNAS:") != NULL) {
                lab->columnas = atoi(linea + 9);
            } else if (strstr(linea, "TIENE_SOLUCION:") != NULL) {
                lab->tieneSolucion = atoi(linea + 15);
            } else if (strstr(linea, "PASOS_SOLUCION:") != NULL) {
                lab->pasosSolucion = atoi(linea + 15);
            }
        } else if (filaActual < lab->filas) {
            // Leer datos del laberinto
            char* token = strtok(linea, " ");
            int columna = 0;
            
            while (token != NULL && columna < lab->columnas) {
                lab->celdas[filaActual][columna] = atoi(token);
                token = strtok(NULL, " ");
                columna++;
            }
            filaActual++;
        }
    }
    
    fclose(archivo);
    
    // Verificar integridad
    if (filaActual != lab->filas) {
        printf("Error: Archivo corrupto o incompleto\n");
        free(lab);
        return NULL;
    }
    
    printf("Laberinto cargado exitosamente: %s\n", lab->nombre);
    return lab;
}

int existeLaberinto(const char* nombreArchivo) {
    char nombreCompleto[MAX_NOMBRE + 50];
    struct stat buffer;
    
    sprintf(nombreCompleto, "%s%s", DIRECTORIO_LABERINTOS, nombreArchivo);
    if (strstr(nombreArchivo, ".txt") == NULL) {
        strcat(nombreCompleto, ".txt");
    }
    
    return (stat(nombreCompleto, &buffer) == 0);
}

void listarLaberintosGuardados() {
    DIR* directorio;
    struct dirent* entrada;
    int contador = 0;
    
    // Crear directorio si no existe
    crearDirectorioLaberintos();
    
    directorio = opendir(DIRECTORIO_LABERINTOS);
    if (directorio == NULL) {
        printf("No se pudo abrir el directorio %s\n", DIRECTORIO_LABERINTOS);
        return;
    }
    
    printf(COLOR_CYAN "\n=== LABERINTOS GUARDADOS ===\n" COLOR_RESET);
    
    while ((entrada = readdir(directorio)) != NULL) {
        if (strstr(entrada->d_name, ".txt") != NULL) {
            char nombreSinExt[MAX_NOMBRE];
            strncpy(nombreSinExt, entrada->d_name, strlen(entrada->d_name) - 4);
            nombreSinExt[strlen(entrada->d_name) - 4] = '\0';
            
            printf("%d. %s\n", ++contador, nombreSinExt);
        }
    }
    
    if (contador == 0) {
        printf("No hay laberintos guardados.\n");
    } else {
        printf("\nTotal: %d laberintos\n", contador);
    }
    
    closedir(directorio);
}

int eliminarLaberinto(const char* nombreArchivo) {
    char nombreCompleto[MAX_NOMBRE + 50];
    
    sprintf(nombreCompleto, "%s%s", DIRECTORIO_LABERINTOS, nombreArchivo);
    if (strstr(nombreArchivo, ".txt") == NULL) {
        strcat(nombreCompleto, ".txt");
    }
    
    if (remove(nombreCompleto) == 0) {
        printf("Laberinto eliminado: %s\n", nombreArchivo);
        return 1;
    } else {
        printf("Error al eliminar el laberinto: %s\n", nombreArchivo);
        return 0;
    }
}

// ==================== EXPORTACION ====================

void exportarLaberintoSVG(Laberinto* lab, const char* nombreArchivo) {
    FILE* archivo;
    char nombreCompleto[MAX_NOMBRE + 50];
    int tamCelda = 20;
    
    sprintf(nombreCompleto, "%s%s.svg", DIRECTORIO_LABERINTOS, nombreArchivo);
    
    archivo = fopen(nombreCompleto, "w");
    if (archivo == NULL) {
        printf("Error al crear archivo SVG\n");
        return;
    }
    
    // Encabezado SVG
    fprintf(archivo, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(archivo, "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n",
            lab->columnas * tamCelda + 10, lab->filas * tamCelda + 10);
    
    // Fondo
    fprintf(archivo, "<rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#f0f0f0\"/>\n",
            lab->columnas * tamCelda + 10, lab->filas * tamCelda + 10);
    
    // Dibujar laberinto
    for (int i = 0; i < lab->filas; i++) {
        for (int j = 0; j < lab->columnas; j++) {
            int x = j * tamCelda + 5;
            int y = i * tamCelda + 5;
            
            switch (lab->celdas[i][j]) {
                case PARED:
                    fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#333333\"/>\n",
                            x, y, tamCelda, tamCelda);
                    break;
                case CAMINO:
                    fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#ffffff\"/>\n",
                            x, y, tamCelda, tamCelda);
                    break;
                case INICIO:
                    fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#4CAF50\"/>\n",
                            x, y, tamCelda, tamCelda);
                    fprintf(archivo, "<text x=\"%d\" y=\"%d\" font-size=\"12\" text-anchor=\"middle\" dy=\".3em\">E</text>\n",
                            x + tamCelda/2, y + tamCelda/2);
                    break;
                case FINAL:
                    fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#F44336\"/>\n",
                            x, y, tamCelda, tamCelda);
                    fprintf(archivo, "<text x=\"%d\" y=\"%d\" font-size=\"12\" text-anchor=\"middle\" dy=\".3em\">S</text>\n",
                            x + tamCelda/2, y + tamCelda/2);
                    break;
            }
            
            // Bordes
            fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"none\" stroke=\"#cccccc\" stroke-width=\"1\"/>\n",
                    x, y, tamCelda, tamCelda);
        }
    }
    
    fprintf(archivo, "</svg>\n");
    fclose(archivo);
    
    printf("Laberinto exportado a SVG: %s\n", nombreCompleto);
}

void exportarEstadisticas(Laberinto* lab, const char* nombreArchivo) {
    FILE* archivo;
    char nombreCompleto[MAX_NOMBRE + 50];
    
    sprintf(nombreCompleto, "%s%s_estadisticas.txt", DIRECTORIO_LABERINTOS, nombreArchivo);
    
    archivo = fopen(nombreCompleto, "w");
    if (archivo == NULL) {
        printf("Error al crear archivo de estadísticas\n");
        return;
    }
    
    // Calcular estadísticas
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
    
    // Escribir estadísticas
    fprintf(archivo, "ESTADÍSTICAS DEL LABERINTO: %s\n", lab->nombre);
    fprintf(archivo, "================================\n\n");
    fprintf(archivo, "Tamaño: %d x %d\n", lab->filas, lab->columnas);
    fprintf(archivo, "Total de celdas: %d\n", lab->filas * lab->columnas);
    fprintf(archivo, "Paredes: %d (%.1f%%)\n", paredes, 100 - porcentajeCaminos);
    fprintf(archivo, "Caminos: %d (%.1f%%)\n", caminos, porcentajeCaminos);
    fprintf(archivo, "Tiene solución: %s\n", lab->tieneSolucion ? "Sí" : "No");
    fprintf(archivo, "Pasos en solución: %d\n", lab->pasosSolucion);
    fprintf(archivo, "Fecha de generación: %s", ctime(&(time_t){time(NULL)}));
    
    fclose(archivo);
    printf("Estadísticas exportadas: %s\n", nombreCompleto);
}

// ==================== FUNCIONES AUXILIARES ====================

void crearDirectorioLaberintos() {
    struct stat st = {0};
    
    if (stat(DIRECTORIO_LABERINTOS, &st) == -1) {
        mkdir(DIRECTORIO_LABERINTOS, 0700);
    }
}

char* generarNombreArchivo(const char* nombreLaberinto) {
    static char nombreArchivo[MAX_NOMBRE + 50];
    time_t ahora = time(NULL);
    struct tm* t = localtime(&ahora);
    
    sprintf(nombreArchivo, "%s_%04d%02d%02d_%02d%02d%02d",
            nombreLaberinto,
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    
    return nombreArchivo;
}

int validarArchivoLaberinto(const char* nombreArchivo) {
    FILE* archivo;
    char nombreCompleto[MAX_NOMBRE + 50];
    
    sprintf(nombreCompleto, "%s%s", DIRECTORIO_LABERINTOS, nombreArchivo);
    
    archivo = fopen(nombreCompleto, "r");
    if (archivo == NULL) {
        return 0;
    }
    
    // Verificar si tiene el formato correcto
    char linea[MAX_LINEA];
    int tieneData = 0, tieneNombre = 0;
    
    while (fgets(linea, MAX_LINEA, archivo)) {
        if (strstr(linea, "NOMBRE:") != NULL) tieneNombre = 1;
        if (strstr(linea, "DATA:") != NULL) tieneData = 1;
    }
    
    fclose(archivo);
    
    return (tieneNombre && tieneData);
}