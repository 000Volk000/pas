#!/bin/bash

read -p "Introduzca un número (1 <= x < 10) : " num

if [ 1 -le $num ];
then
    if [ $num -lt 10 ];
    then
        echo "El número $num es correcto !"
    else
        echo "Fuera de rango !"
    fi
else
    echo "Fuera de rango !"
fi