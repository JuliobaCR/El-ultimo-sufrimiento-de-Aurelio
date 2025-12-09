# GENERADOR DE LABERINTOS ANÁRQUICOS

## Descripción
Sistema completo para generación, análisis y visualización de laberintos.
Implementa algoritmos de grafos con interfaz interactiva.

## Características Principales

### 1. Generación de Laberintos
- 4 métodos diferentes
- Nombres aleatorios con memes
- Tamaño configurable

### 2. Algoritmos Implementados
- **Dijkstra con estados intermedios**: Visualización paso a paso
- **Prim**: Árbol generador mínimo para laberintos perfectos
- **Kruskal**: Alternativa a Prim para grafos dispersos
- **BFS**: Búsqueda en anchura para caminos más cortos
- Análisis de componentes conexas

### 3. Sistema de Archivos
- Guardado y carga de laberintos
- Exportación a formato SVG
- Estadísticas detalladas
- Directorio organizado

### 4. Interfaz de Usuario
- Menús jerárquicos con colores
- Visualización gráfica de laberintos
- Animación de soluciones
- Navegación intuitiva

## Estructura del Código

### Archivos Principales
- `main.c`: Programa principal y menús
- `grafo.h/c`: Estructuras y operaciones de grafos
- `algoritmos.h/c`: Implementación de algoritmos
- `laberinto.h/c`: Generación y visualización
- `archivos.h/c`: Manejo de persistencia
- `memes.h/c`: Nombres humorísticos
- `config.h`: Configuraciones globales

### Cumplimiento de Requisitos

#### 1. Dijkstra con Estados Intermedios
- Función `dijkstra_con_estados()` muestra cada iteración
- Tablas detalladas con información de nodos
- Reconstrucción paso a paso del camino
- Pausas para estudio del algoritmo

#### 2. Segundo Algoritmo (Prim)
- Implementado completamente en `prim()`
- Explicación detallada en `explicar_algoritmos_seleccion()`
- Comparación con Kruskal y BFS
- Aplicación a generación de laberintos

#### 3. Menús y Visualización
- Sistema de menús completo en `main.c`
- Laberintos mostrados con caracteres gráficos
- Colores ANSI para mejor legibilidad
- Animación de soluciones paso a paso

#### 4. Calidad de Código
- Todas las funciones en `snake_case`
- Comentarios en bloque breves (3-4 líneas)
- Comentarios en línea para código complejo
- Funciones divididas por responsabilidad
- Estilo consistente en todos los archivos

#### 5. Memes
- 25 nombres combinando anarquismo y universidad
- Temática humorística y relacionada
- Nombres aleatorios para laberintos

## Compilación y Ejecución

### Requisitos
- Compilador GCC
- Sistema operativo Linux/Unix o Windows con WSL
- Terminal con soporte ANSI colors

### Compilación
```bash
make clean
make