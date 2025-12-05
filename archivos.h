/*
 * ARCHIVOS.H
 * Manejo de archivos para laberintos
 */

#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include "laberinto.h"

// Funciones de archivos
int guardarLaberinto(Laberinto* lab, const char* nombreArchivo);
Laberinto* cargarLaberinto(const char* nombreArchivo);
int existeLaberinto(const char* nombreArchivo);
void listarLaberintosGuardados();
int eliminarLaberinto(const char* nombreArchivo);
void exportarLaberintoSVG(Laberinto* lab, const char* nombreArchivo);
void exportarEstadisticas(Laberinto* lab, const char* nombreArchivo);

// Funciones auxiliares
void crearDirectorioLaberintos();
char* generarNombreArchivo(const char* nombreLaberinto);
int validarArchivoLaberinto(const char* nombreArchivo);

#endif // ARCHIVOS_H