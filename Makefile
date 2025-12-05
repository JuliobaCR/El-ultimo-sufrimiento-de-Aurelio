# Makefile para Generador de Laberintos Anárquicos

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 
TARGET = laberinto
OBJS = main.o grafo.o algoritmos.o laberinto.o archivos.o memes.o

# Regla principal
all: directorio $(TARGET)

# Enlazar objeto
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compilar archivos fuente
main.o: main.c grafo.h algoritmos.h laberinto.h archivos.h config.h memes.h
	$(CC) $(CFLAGS) -c main.c

grafo.o: grafo.c grafo.h config.h
	$(CC) $(CFLAGS) -c grafo.c

algoritmos.o: algoritmos.c algoritmos.h grafo.h config.h
	$(CC) $(CFLAGS) -c algoritmos.c

laberinto.o: laberinto.c laberinto.h grafo.h config.h memes.h
	$(CC) $(CFLAGS) -c laberinto.c

archivos.o: archivos.c archivos.h laberinto.h config.h
	$(CC) $(CFLAGS) -c archivos.c

memes.o: memes.c memes.h
	$(CC) $(CFLAGS) -c memes.c

# Crear directorio para laberintos
directorio:
	mkdir -p laberintos

# Limpiar archivos compilados
clean:
	rm -f $(OBJS) $(TARGET) laberintos/*.txt laberintos/*.svg

# Ejecutar el programa
run: all
	./$(TARGET)

# Depurar con valgrind
debug: all
	valgrind --leak-check=full ./$(TARGET)

.PHONY: all clean run debug directorio