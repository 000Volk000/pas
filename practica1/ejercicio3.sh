#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Debes proporcionar la ruta del directorio home donde buscar las claves privadas de SSH."
    exit -1
fi

cont=0

for x in "$1"/*
do
    if [ -d "$x" ]
    then
        # Comprobar si existe el directorio .ssh y el archivo id_rsa
        if [ -d ""$x/.ssh"" ] && [ -f """$x/.ssh"/id_rsa"" ]
        then
            # stat -c "%a" da los permisos en octal
            if [ $(stat -c "%a" "$x") -gt 700 ] && [ $(stat -c "%a" ""$x/.ssh"") -gt 700 ] && [ $(stat -c "%a" """$x/.ssh"/id_rsa"") -gt 600 ]
            then
                # Basename es el nombre de la carpeta actual de la ruta completa
                echo "El usuario $(basename $x) tiene una clave privada de SSH en ""$x/.ssh"/id_rsa" que no está protegida. La clave debe ser accesible únicamente por el propietario."
                cont=$((cont + 1))

                echo "Tu clave privada de SSH no está correctamente protegida. Por favor, ajusta los permisos." > """$x/Desktop"/clave_insegura.txt""
            fi
        fi
    fi
done

echo ""
echo "Hay "$cont"/"$(ls -1 "$1" | wc -l)" claves privadas no seguras"