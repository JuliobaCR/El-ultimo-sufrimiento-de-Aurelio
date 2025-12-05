#!/bin/bash

echo "============================================="
echo "   GENERADOR DE LABERINTOS ANÁRQUICOS"
echo "============================================="
echo ""

echo "Compilando proyecto..."
make clean
make

if [ $? -eq 0 ]; then
    echo ""
    echo "Compilación exitosa!"
    echo ""
    echo "Ejecutando programa..."
    echo "============================================="
    ./laberinto
else
    echo ""
    echo "Error en la compilación."
    exit 1
fi