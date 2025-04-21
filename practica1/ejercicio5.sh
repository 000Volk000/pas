#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Uso: ./ejercicio5.sh <directorio>"
    exit -1
fi

if [ ! -d "$1" ]
then
    echo "Error: El directorio especificado no existe."
    exit -1
fi

# find -type f es para que solo busque ficheros y no directorios
archivos=$(find "$1" -type f)

info_archivos=()

for x in $archivos
do
    nombre=$(basename "$x")
    # realpath = ruta absoluta
    ruta=$(realpath "$x")
    # %Y para fecha de última modificación del archivo
    fecha_mod=$(stat -c %Y "$x")
    # %s para el tamaño del archivo en bytes
    tamano=$(stat -c %s "$x")
    # %a para los permisos del archivo (-rw-r--r--)
    permisos=$(stat -c %A "$x")

    info_archivos+=("$nombre\t$ruta\t$fecha_mod\t$tamano bytes\t$permisos")
done

for x in "${info_archivos[@]}"
do
    echo -e "$x" >> tmp
done

sort -k3n tmp >> tmp

# Configura el separador de campos (IFS) para leer líneas completas sin dividirlas por espacios o tabulaciones.
while IFS= read -r line
do
    # -e para permitir interpretar caracteres de escape.
    echo -e "$line"
# Lee línea por línea desde el archivo `tmp` y ejecuta el bloque de código para cada línea.
# Despues de llegar al borde de la locura es la unica solución que he encontrado sin usar funciones que no hemos dado, gracias stack overflow
done < tmp

rm tmp