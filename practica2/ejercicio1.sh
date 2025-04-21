#!/bin/bash

echo -e "\n1) Años de los libros publicados entre 1950 y 2020"
# sed -r -n -e: Usa el modo extendido de expresiones regulares (-r), no imprime líneas por defecto (-n), y aplica el script (-e).
# 's/Año: (.*$)/\1/p': Busca líneas que comiencen con "Año: ", captura el resto de la línea (.*$) y lo reemplaza por el grupo capturado (\1), imprimiendo solo las coincidencias (p).
# grep -E: Usa expresiones regulares extendidas (-E) para buscar años entre 1950 y 2020.
sed -r -n -e 's/Año: (.*$)/\1/p' libros.txt | grep -E '^(19[5-9][0-9]|20[0-1][0-9]|2020)$'

echo -e "\n2) Precios superiores a 20 euros:"
# grep .*2[0-9]: Busca líneas que contengan números que comiencen con "2" seguido de cualquier dígito (20-29).
cat libros.txt | sed -r -n -e 's/Precio: (.*$)/\1/p' | grep .*2[0-9]

echo -e "\n3) Numero de libros por genero:"
# sed -n -E: Usa expresiones regulares extendidas (-E) y no imprime líneas por defecto (-n).
# 's/\[Género: (.*)\]/\1/p': Busca líneas con "[Género: texto]" y captura el texto dentro de los corchetes.
# sort: Ordena las líneas alfabéticamente.
# uniq -c: Cuenta las ocurrencias de cada línea única.
# sed -E: Usa expresiones regulares extendidas para dar formato al conteo.
# 's/^ *([0-9]+) (.*)/\2 aparece \1 veces/': Reorganiza el conteo para mostrar el género seguido del número de ocurrencias.
sed -n -E 's/\[Género: (.*)\]/\1/p' libros.txt | sort | uniq -c | sed -E 's/^ *([0-9]+) (.*)/\2 aparece \1 veces/'

echo -e "\n4) Palabras de al menos 8 caracteres que empiezan por consonante y terminan por vocal:"
# cat libros.txt: Muestra el contenido del archivo.
# grep -o: Imprime solo las coincidencias encontradas.
# '\b[^aeiouAEIUO][a-zA-Z]\{6,\}[aeiouAEIOU]\b': Busca palabras que:
# - Empiecen por una consonante ([^aeiouAEIUO]).
# - Tengan al menos 6 caracteres intermedios (\{6,\}).
# - Terminen en una vocal ([aeiouAEIOU]).
cat libros.txt | grep -o '\b[^aeiouAEIUO][a-zA-Z]\{6,\}[aeiouAEIOU]\b'

echo -e "\n5) Lineas de autor cuyo nombre o apellido contiene una doble l:"
# sed -n -E: Igual que antes, no imprime líneas por defecto y usa expresiones regulares extendidas.
# 's/Autor: (.*)/\1/p': Busca líneas que comiencen con "Autor: " y captura el resto de la línea.
# grep -E '\b.*ll.*\b': Busca palabras que contengan "ll" en cualquier posición.
cat libros.txt | sed -n -E 's/Autor: (.*)/\1/p' | grep -E '\b.*ll.*\b'

echo -e "\n6) Titulos de libros con mas de tres palabras: "
# grep -E: Usa expresiones regulares extendidas.
# '^Título: (\S+[[:space:]]+){3,}\S+': Busca líneas que comiencen con "Título: " seguido de al menos tres palabras (\S+ es una palabra, [[:space:]]+ es un espacio).
grep -E '^Título: (\S+[[:space:]]+){3,}\S+' libros.txt

echo -e "\n7. Extraer y mostrar los titulos de libros cuyo precio termine en “,99€”."
# grep -B 3: Muestra 3 líneas antes de las coincidencias.
# 'Precio: .*,99€': Busca líneas que contengan "Precio: " seguido de cualquier texto y terminen en ",99€".
# grep 'Título: ': Filtra las líneas que contienen "Título: ".
grep -B 3 'Precio: .*,99€' libros.txt | grep 'Título: '

echo -e "\n8) Numero de libros con año de publicacion anterior a 2000:"
# sed -r -n -e: Igual que antes, usa expresiones regulares extendidas y no imprime líneas por defecto.
# 's/^Año: (.*)/\1/p': Busca líneas que comiencen con "Año: " y captura el resto de la línea.
# grep -E '^1[0-9]{3}$': Busca años que comiencen con "1" (siglo XX).
# wc -l: Cuenta el número de líneas.
sed -r -n -e 's/^Año: (.*)/\1/p' libros.txt | grep -E '^1[0-9]{3}$' | wc -l

echo -e "\n9) Lineas con dos o mas palabras con mayuscula consecutivas:"
# grep -E: Usa expresiones regulares extendidas.
# ': [A-Z][a-Z]*[[:space:]]+[A-Z][a-Z]*': Busca líneas con dos palabras consecutivas que comiencen con mayúscula.
grep -E ': [A-Z][a-Z]*[[:space:]]+[A-Z][a-Z]*' libros.txt

echo -e "\n10) Generos con una palabra compuesta:"
# grep -E: Usa expresiones regulares extendidas.
# 'Género: [a-Z]*-[a-Z]*': Busca líneas con "Género: " seguido de una palabra compuesta (separada por un guion "-").
grep -E 'Género: [a-Z]*-[a-Z]*' libros.txt