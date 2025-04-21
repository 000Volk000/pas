#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Argumentos erroneos. Uso: ./ejercicio2.sh <fichero_libros>."
	exit -1
fi

# cut -d '.' -f2: Divide la cadena usando el carácter '.' como delimitador (-d '.') y selecciona el segundo campo (-f2).
extension=$(echo $1 | cut -d '.' -f2)

if [ $extension != "txt" ]
then
	echo "Introduzca un archivo .txt"
	exit -1
fi

# /^[[:space:]]*$/d: Elimina las líneas que están vacías o contienen solo espacios.
# ^[[:space:]]*$: Expresión regular que indica líneas que comienzan (^) y terminan ($) con cero o más espacios ([[:space:]]*).
# /^Título: /! s/^/| -> /: Para las líneas que no comienzan con "Título: " (!/^Título: /), añade "| -> " al inicio de la línea (s/^/| -> /).
# /^Título: /: Expresión regular que busca líneas que comienzan (^) con "Título: ".
# s/^/| -> /: Sustituye el inicio de la línea (^) por "| -> ".
sed -E ' /^[[:space:]]*$/d
	/^Título: /! s/^/| -> /' "$1"