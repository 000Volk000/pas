#!/bin/bash

if [ $# -ne 4 ];
then
    echo "Debes poner 4 argumentos:"
    echo "1. Directorio que se va a copiar"
    echo "2. Directorio donde se almacenara la copia comprimida. Si no existe, se creara"
    echo "3. Valor 0 o 1 que indicara si la copia debe comprimirse o solo archivarse"
    echo "4. Valor 0 o 1 que indicara si la copia se debera sobreescribir en caso de que ya exista."
    exit -1
fi

# -e f1 ¿Existe el fichero f1?
# -s f1 ¿f1 tiene tamaño mayor que cero?
# -f f1 ¿Es f1 un fichero normal?
# -d f1 ¿Es f1 un directorio?
# -l f1 ¿Es f1 un enlace simbólico?
# -r f1 ¿Tienes permiso de lectura sobre f1?
# -w f1 ¿Tienes permiso de escritura sobre f1?
# -x f1 ¿Tienes permiso de ejecución sobre f1?
if [ ! -d "$2" ]; then
    mkdir -p "$2"
    if [ $? -ne 0 ]; then
        echo "No se pudo crear el directorio de destino: $2"
        exit -1
    fi
fi

if [ "$3" -eq 0 ]
then
    copia=""$1"_"$USER"_$(date +%d%m%Y).tar"
else
    copia=""$1"_"$USER"_$(date +%d%m%Y).tar.gz"
fi

if [ -e "$2"/"$copia" ]; then
    if [ "$4" -eq 0 ]; then
        echo "Ya se ha realizado esta copia hoy ("$2"/"$copia")."
        echo "No se sobreescribirá la copia."
        exit -1
    else
        echo "Ya se ha realizado esta copia hoy ("$2"/"$copia")."
    fi
fi

# tar -czf: Crea un archivo comprimido en formato .tar (-c) usando compresión gzip .gz (-z) y especifica el archivo de salida (-f).
# tar -C: Cambia al directorio especificado antes de realizar la operación de archivado.
if [ "$3" -eq 0 ]; then
    tar -cf ""$2"/"$copia"" "$1"
else
    tar -czf ""$2"/"$copia"" "$1"
fi

if [ $? -eq 0 ]; then
    echo "Copia realizada en "$2"/"$copia"."
else
    echo "Error al realizar la copia."
    exit -1
fi