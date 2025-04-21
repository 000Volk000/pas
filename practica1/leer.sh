#!/bin/bash

echo -n "Introduce nombre del fichero: "
read fichero
echo "Fichero $fichero:"
echo
cat $fichero