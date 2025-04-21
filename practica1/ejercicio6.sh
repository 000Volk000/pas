#!/bin/bash

generar_indice() {
    dir=$1
    indice="$dir/index.html"

    echo "<ul>" > "$indice"

    for elemento in "$dir"/*
    do
        if [ -f "$elemento" ]
        then
            echo "<li>$(basename "$elemento")</li>" >> "$indice"
        elif [ -d "$elemento" ]
        then
            subdir=$(basename "$elemento")
            echo "<li><a href=\"$subdir/index.html\">$subdir</a></li>" >> "$indice"
        fi
    done

    echo "</ul>" >> "$indice"
    echo "Se ha creado el fichero $indice con el contenido del directorio $dir."

    for elemento in "$dir"/*
    do
        if [ -d "$elemento" ]
        then
            generar_indice "$elemento"
        fi
    done
}

if [ $# -ne 1 ]; then
    echo "Uso: $0 <directorio>"
    exit -1
fi

if [ ! -d "$1" ]; then
    echo "Error: El directorio especificado no existe."
    exit -1
fi

generar_indice "$1"