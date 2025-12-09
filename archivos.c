/*
 * archivos.c
 * Funciones para manejo de archivos de laberintos.
 * Permite guardar, cargar y exportar laberintos.
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

/*
 * guardar_laberinto
 * Guarda un laberinto en archivo de texto.
 * Crea directorio si no existe y formatea datos.
 */
int guardar_laberinto(laberinto* lab, const char* nombre_archivo) {
    FILE* archivo;                         // Puntero a archivo
    char nombre_completo[MAX_NOMBRE + 50]; // Nombre completo con ruta
    
    crear_directorio_laberintos();  // Asegura que exista directorio
    
    // Construye nombre completo del archivo
    if (nombre_archivo == NULL) {
        // Nombre por defecto (nombre del laberinto)
        sprintf(nombre_completo, "%s%s.txt", DIRECTORIO_LABERINTOS, lab->nombre);
    } else {
        // Nombre personalizado
        sprintf(nombre_completo, "%s%s", DIRECTORIO_LABERINTOS, nombre_archivo);
        if (strstr(nombre_archivo, ".txt") == NULL) {
            strcat(nombre_completo, ".txt");  // Añade extensión si falta
        }
    }
    
    // Abre archivo para escritura
    archivo = fopen(nombre_completo, "w");
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo %s\n", nombre_completo);
        return 0;  // Error
    }
    
    // Escribe metadatos
    fprintf(archivo, "# LABERINTO ANÁRQUICO\n");
    fprintf(archivo, "NOMBRE: %s\n", lab->nombre);
    fprintf(archivo, "FILAS: %d\n", lab->filas);
    fprintf(archivo, "COLUMNAS: %d\n", lab->columnas);
    fprintf(archivo, "TIENE_SOLUCION: %d\n", lab->tiene_solucion);
    fprintf(archivo, "PASOS_SOLUCION: %d\n", lab->pasos_solucion);
    fprintf(archivo, "DATA:\n");
    
    // Escribe matriz del laberinto
    for (int i = 0; i < lab->filas; i++) {
        for (int j = 0; j < lab->columnas; j++) {
            fprintf(archivo, "%d", lab->celdas[i][j]);  // Valor de celda
            if (j < lab->columnas - 1) {
                fprintf(archivo, " ");  // Espacio entre valores
            }
        }
        fprintf(archivo, "\n");  // Nueva línea por fila
    }
    
    fclose(archivo);  // Cierra archivo
    printf("Laberinto guardado en: %s\n", nombre_completo);
    return 1;  // Éxito
}

/*
 * cargar_laberinto
 * Carga un laberinto desde archivo de texto.
 * Verifica formato y estructura del archivo.
 */
laberinto* cargar_laberinto(const char* nombre_archivo) {
    FILE* archivo;                         // Puntero a archivo
    char nombre_completo[MAX_NOMBRE + 50]; // Nombre completo con ruta
    char linea[MAX_LINEA];                 // Buffer para línea
    laberinto* lab = NULL;                 // Laberinto a cargar
    
    // Construye nombre completo
    sprintf(nombre_completo, "%s%s", DIRECTORIO_LABERINTOS, nombre_archivo);
    if (strstr(nombre_archivo, ".txt") == NULL) {
        strcat(nombre_completo, ".txt");  // Añade extensión si falta
    }
    
    // Verifica si el archivo existe
    if (!existe_laberinto(nombre_archivo)) {
        printf("Error: El archivo %s no existe\n", nombre_completo);
        return NULL;
    }
    
    // Abre archivo para lectura
    archivo = fopen(nombre_completo, "r");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", nombre_completo);
        return NULL;
    }
    
    // Asigna memoria para laberinto
    lab = (laberinto*)malloc(sizeof(laberinto));
    if (!lab) {
        fclose(archivo);
        return NULL;
    }
    
    int leyendo_data = 0;  // Flag para sección DATA
    int fila_actual = 0;   // Fila actual en lectura
    
    // Lee archivo línea por línea
    while (fgets(linea, MAX_LINEA, archivo)) {
        linea[strcspn(linea, "\n")] = 0;  // Elimina salto de línea
        
        if (strstr(linea, "DATA:") != NULL) {
            leyendo_data = 1;  // Comienza sección DATA
            continue;
        }
        
        if (!leyendo_data) {
            // Parsea metadatos
            if (strstr(linea, "NOMBRE:") != NULL) {
                strcpy(lab->nombre, linea + 8);  // Copia nombre
            } else if (strstr(linea, "FILAS:") != NULL) {
                lab->filas = atoi(linea + 6);  // Convierte a entero
            } else if (strstr(linea, "COLUMNAS:") != NULL) {
                lab->columnas = atoi(linea + 9);  // Convierte a entero
            } else if (strstr(linea, "TIENE_SOLUCION:") != NULL) {
                lab->tiene_solucion = atoi(linea + 15);  // Convierte a entero
            } else if (strstr(linea, "PASOS_SOLUCION:") != NULL) {
                lab->pasos_solucion = atoi(linea + 15);  // Convierte a entero
            }
        } else if (fila_actual < lab->filas) {
            // Lee datos del laberinto
            char* token = strtok(linea, " ");  // Primer token
            int columna = 0;  // Columna actual
            
            while (token != NULL && columna < lab->columnas) {
                lab->celdas[fila_actual][columna] = atoi(token);  // Convierte a entero
                token = strtok(NULL, " ");  // Siguiente token
                columna++;  // Incrementa columna
            }
            fila_actual++;  // Incrementa fila
        }
    }
    
    fclose(archivo);  // Cierra archivo
    
    // Verifica integridad del archivo
    if (fila_actual != lab->filas) {
        printf("Error: Archivo corrupto o incompleto\n");
        free(lab);  // Libera memoria
        return NULL;
    }
    
    printf("Laberinto cargado exitosamente: %s\n", lab->nombre);
    return lab;  // Retorna laberinto cargado
}

/*
 * existe_laberinto
 * Verifica si un archivo de laberinto existe.
 * Usa stat() para verificar existencia.
 */
int existe_laberinto(const char* nombre_archivo) {
    char nombre_completo[MAX_NOMBRE + 50];  // Nombre completo con ruta
    struct stat buffer;                     // Estructura para stat
    
    sprintf(nombre_completo, "%s%s", DIRECTORIO_LABERINTOS, nombre_archivo);
    if (strstr(nombre_archivo, ".txt") == NULL) {
        strcat(nombre_completo, ".txt");  // Añade extensión si falta
    }
    
    // Retorna 1 si existe, 0 si no
    return (stat(nombre_completo, &buffer) == 0);
}

/*
 * listar_laberintos_guardados
 * Muestra lista de laberintos guardados en directorio.
 * Filtra archivos .txt y muestra nombres sin extensión.
 */
void listar_laberintos_guardados(void) {
    DIR* directorio;                // Puntero a directorio
    struct dirent* entrada;         // Entrada de directorio
    int contador = 0;               // Contador de archivos
    
    crear_directorio_laberintos();  // Asegura que exista directorio
    
    directorio = opendir(DIRECTORIO_LABERINTOS);
    if (directorio == NULL) {
        printf("No se pudo abrir el directorio %s\n", DIRECTORIO_LABERINTOS);
        return;
    }
    
    printf(COLOR_CYAN "\n=== LABERINTOS GUARDADOS ===\n" COLOR_RESET);
    
    // Lee todas las entradas del directorio
    while ((entrada = readdir(directorio)) != NULL) {
        if (strstr(entrada->d_name, ".txt") != NULL) {  // Solo archivos .txt
            char nombre_sin_ext[MAX_NOMBRE];  // Buffer para nombre sin extensión
            
            // Copia nombre sin extensión .txt
            strncpy(nombre_sin_ext, entrada->d_name, strlen(entrada->d_name) - 4);
            nombre_sin_ext[strlen(entrada->d_name) - 4] = '\0';  // Terminación
            
            printf("%d. %s\n", ++contador, nombre_sin_ext);  // Muestra con número
        }
    }
    
    if (contador == 0) {
        printf("No hay laberintos guardados.\n");
    } else {
        printf("\nTotal: %d laberintos\n", contador);
    }
    
    closedir(directorio);  // Cierra directorio
}

/*
 * eliminar_laberinto
 * Elimina un archivo de laberinto.
 * Pide confirmación antes de eliminar.
 */
int eliminar_laberinto(const char* nombre_archivo) {
    char nombre_completo[MAX_NOMBRE + 50];  // Nombre completo con ruta
    
    sprintf(nombre_completo, "%s%s", DIRECTORIO_LABERINTOS, nombre_archivo);
    if (strstr(nombre_archivo, ".txt") == NULL) {
        strcat(nombre_completo, ".txt");  // Añade extensión si falta
    }
    
    // Intenta eliminar archivo
    if (remove(nombre_completo) == 0) {
        printf("Laberinto eliminado: %s\n", nombre_archivo);
        return 1;  // Éxito
    } else {
        printf("Error al eliminar el laberinto: %s\n", nombre_archivo);
        return 0;  // Error
    }
}

// ==================== EXPORTACIÓN ====================

/*
 * exportar_laberinto_svg
 * Exporta laberinto a formato SVG (vectorial).
 * Crea imagen escalable con colores definidos.
 */
void exportar_laberinto_svg(laberinto* lab, const char* nombre_archivo) {
    FILE* archivo;                         // Puntero a archivo
    char nombre_completo[MAX_NOMBRE + 50]; // Nombre completo con ruta
    int tam_celda = 20;                    // Tamaño de celda en píxeles
    
    sprintf(nombre_completo, "%s%s.svg", DIRECTORIO_LABERINTOS, nombre_archivo);
    
    archivo = fopen(nombre_completo, "w");
    if (archivo == NULL) {
        printf("Error al crear archivo SVG\n");
        return;
    }
    
    // Encabezado SVG
    fprintf(archivo, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(archivo, "<svg width=\"%d\" height=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n",
            lab->columnas * tam_celda + 10, lab->filas * tam_celda + 10);
    
    // Fondo
    fprintf(archivo, "<rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#f0f0f0\"/>\n",
            lab->columnas * tam_celda + 10, lab->filas * tam_celda + 10);
    
    // Dibuja cada celda del laberinto
    for (int i = 0; i < lab->filas; i++) {
        for (int j = 0; j < lab->columnas; j++) {
            int x = j * tam_celda + 5;  // Posición X
            int y = i * tam_celda + 5;  // Posición Y
            
            // Color según tipo de celda
            switch (lab->celdas[i][j]) {
                case PARED:
                    fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#333333\"/>\n",
                            x, y, tam_celda, tam_celda);
                    break;
                case CAMINO:
                    fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#ffffff\"/>\n",
                            x, y, tam_celda, tam_celda);
                    break;
                case INICIO:
                    fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#4CAF50\"/>\n",
                            x, y, tam_celda, tam_celda);
                    fprintf(archivo, "<text x=\"%d\" y=\"%d\" font-size=\"12\" text-anchor=\"middle\" dy=\".3em\">E</text>\n",
                            x + tam_celda/2, y + tam_celda/2);
                    break;
                case FINAL:
                    fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#F44336\"/>\n",
                            x, y, tam_celda, tam_celda);
                    fprintf(archivo, "<text x=\"%d\" y=\"%d\" font-size=\"12\" text-anchor=\"middle\" dy=\".3em\">S</text>\n",
                            x + tam_celda/2, y + tam_celda/2);
                    break;
            }
            
            // Borde de celda
            fprintf(archivo, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"none\" stroke=\"#cccccc\" stroke-width=\"1\"/>\n",
                    x, y, tam_celda, tam_celda);
        }
    }
    
    fprintf(archivo, "</svg>\n");  // Cierra SVG
    fclose(archivo);               // Cierra archivo
    
    printf("Laberinto exportado a SVG: %s\n", nombre_completo);
}

/*
 * exportar_estadisticas
 * Exporta estadísticas del laberinto a archivo de texto.
 * Incluye medidas, porcentajes y fecha de generación.
 */
void exportar_estadisticas(laberinto* lab, const char* nombre_archivo) {
    FILE* archivo;                         // Puntero a archivo
    char nombre_completo[MAX_NOMBRE + 50]; // Nombre completo con ruta
    
    sprintf(nombre_completo, "%s%s_estadisticas.txt", DIRECTORIO_LABERINTOS, nombre_archivo);
    
    archivo = fopen(nombre_completo, "w");
    if (archivo == NULL) {
        printf("Error al crear archivo de estadísticas\n");
        return;
    }
    
    // Calcula estadísticas
    int paredes = 0, caminos = 0;
    for (int i = 0; i < lab->filas; i++) {
        for (int j = 0; j < lab->columnas; j++) {
            if (lab->celdas[i][j] == PARED) {
                paredes++;  // Incrementa contador de paredes
            } else {
                caminos++;  // Incrementa contador de caminos
            }
        }
    }
    
    float porcentaje_caminos = (float)caminos * 100 / (lab->filas * lab->columnas);
    
    // Escribe estadísticas
    fprintf(archivo, "ESTADÍSTICAS DEL LABERINTO: %s\n", lab->nombre);
    fprintf(archivo, "================================\n\n");
    fprintf(archivo, "Tamaño: %d x %d\n", lab->filas, lab->columnas);
    fprintf(archivo, "Total de celdas: %d\n", lab->filas * lab->columnas);
    fprintf(archivo, "Paredes: %d (%.1f%%)\n", paredes, 100 - porcentaje_caminos);
    fprintf(archivo, "Caminos: %d (%.1f%%)\n", caminos, porcentaje_caminos);
    fprintf(archivo, "Tiene solución: %s\n", lab->tiene_solucion ? "Sí" : "No");
    fprintf(archivo, "Pasos en solución: %d\n", lab->pasos_solucion);
    fprintf(archivo, "Fecha de generación: %s", ctime(&(time_t){time(NULL)}));
    
    fclose(archivo);  // Cierra archivo
    printf("Estadísticas exportadas: %s\n", nombre_completo);
}

// ==================== FUNCIONES AUXILIARES ====================

/*
 * crear_directorio_laberintos
 * Crea directorio para laberintos si no existe.
 * Usa permisos 0700 (solo propietario).
 */
void crear_directorio_laberintos(void) {
    struct stat st = {0};  // Estructura para stat
    
    if (stat(DIRECTORIO_LABERINTOS, &st) == -1) {
        mkdir(DIRECTORIO_LABERINTOS, 0700);  // Crea directorio
    }
}

/*
 * generar_nombre_archivo
 * Genera nombre de archivo con timestamp.
 * Formato: nombre_YYYYMMDD_HHMMSS
 */
char* generar_nombre_archivo(const char* nombre_laberinto) {
    static char nombre_archivo[MAX_NOMBRE + 50];  // Buffer estático
    time_t ahora = time(NULL);                    // Tiempo actual
    struct tm* t = localtime(&ahora);             // Convierte a tiempo local
    
    sprintf(nombre_archivo, "%s_%04d%02d%02d_%02d%02d%02d",
            nombre_laberinto,
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    
    return nombre_archivo;  // Retorna nombre generado
}

/*
 * validar_archivo_laberinto
 * Verifica formato básico de archivo de laberinto.
 * Comprueba existencia de secciones esenciales.
 */
int validar_archivo_laberinto(const char* nombre_archivo) {
    FILE* archivo;                         // Puntero a archivo
    char nombre_completo[MAX_NOMBRE + 50]; // Nombre completo con ruta
    
    sprintf(nombre_completo, "%s%s", DIRECTORIO_LABERINTOS, nombre_archivo);
    
    archivo = fopen(nombre_completo, "r");
    if (archivo == NULL) {
        return 0;  // No se pudo abrir
    }
    
    char linea[MAX_LINEA];          // Buffer para línea
    int tiene_data = 0, tiene_nombre = 0;  // Flags para secciones
    
    // Lee archivo buscando secciones esenciales
    while (fgets(linea, MAX_LINEA, archivo)) {
        if (strstr(linea, "NOMBRE:") != NULL) tiene_nombre = 1;
        if (strstr(linea, "DATA:") != NULL) tiene_data = 1;
    }
    
    fclose(archivo);  // Cierra archivo
    
    return (tiene_nombre && tiene_data);  // Válido si tiene ambas secciones
}