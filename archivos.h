/*
 * ARCHIVOS.H
 * Manejo de archivos para laberintos
 */

#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "laberinto.h"

// Funciones de archivos
int guardar_laberinto(laberinto* lab, const char* nombreArchivo);
laberinto* cargar_laberinto(const char* nombreArchivo);
int existe_laberinto(const char* nombreArchivo);
void listar_laberintos_guardados();
int eliminar_laberinto(const char* nombreArchivo);
void exportar_laberinto_svg(laberinto* lab, const char* nombreArchivo);
void exportar_estadisticas(laberinto* lab, const char* nombreArchivo);

// Funciones auxiliares
void crear_directorio_laberintos();
char* generar_nombre_archivo(const char* nombreLaberinto);
int validar_archivo_laberinto(const char* nombreArchivo);

#endif // ARCHIVOS_H