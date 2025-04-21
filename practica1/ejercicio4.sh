#!/bin/bash

if [ $# -lt 1 ] || [ $# -gt 2 ]
then
    echo "Uso: $0 <longitud de la cadena> [tipo de cadena]"
    exit -1
fi

longitud=$1
tipo=$2


if [ $# -eq 1 ]
then
    correct=false
    while ! $correct
    do
        echo -n "Introduce el tipo de cadena (alfanum, alfa o num): "
        read tipo
        if [ "$tipo" = "alfanum" ] || [ "$tipo" = "alfa" ] || [ "$tipo" = "num" ]
        then
            correct=true
        else
            echo "Tipo de cadena inválido. Inténtalo de nuevo."
        fi
    done
fi

if [ "$tipo" = "alfa" ]
then
    caracteres="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    else if [ "$tipo" = "num" ]
        then
            caracteres="0123456789"
            else if [ "$tipo" = "alfanum" ]
                then
                    caracteres="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
                fi
        fi
fi

cadena=""

for i in $(seq $longitud)
do
    # # para obtener la longitud de la cadena
    rand=$((RANDOM % ${#caracteres}))
    # {var:num:cant} Cojes cant cantidad de letras desde num de la cadena var
    cadena=""$cadena""${caracteres:$rand:1}""
done

echo "$cadena"