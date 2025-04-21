#!/bin/bash

if [ $# -eq 0 ];
then
    arg=.
elif [ $# -ne 1 ];
then
    echo "No pongas tantos parametros, pon maximo 1"
    exit -1
else
    arg=$1
fi

primer=1
for x in "$arg"/*
do
    if [ $primer -eq 1 ];
    then
        ant=$(cat $x | wc -l) # wc (word count) -l (lines)
        primer=0
    else
        if [ $ant -ne $(cat $x | wc -l) ];
        then
            echo "Los archivos tienen distintas lineas entre si"
            exit -1
        fi
    fi
done

for linea in $(seq $ant)
do
cont=0
    for y in "$arg"/*
    do
        if [ $(head -n "$linea" "$y" | tail -n 1) -eq 1 ]
        then
            ((cont=cont+1))
        fi
    done
echo "Asistieron $cont personas a la sesión $linea"
done