#!/bin/bash

#APL N. 1
#SCRIPT 1
#INTEGRANTES:
      #Carballo, Facundo Nicolas (DNI: 42774931)
      #Garcia Burgio, Matias Nicolas (DNI: 42649117)
      #Mottura, Agostina Micaela (DNI: 41898101)
      #Povoli Olivera, Victor (DNI: 43103780)
      #Szust, Ángel Elías (DNI: 43098495)

ErrorS()
{
 echo "Error. La sintaxis del script es la siguiente:"
 echo "Cantidad de lineas: script.sh nombre_archivo L"
 echo "Cantidad de caracteres: script.sh nombre_archivo C"
 echo "Cantidad maxima de caracteres en una linea: script.sh nombre_archivo M"
}
ErrorP()
{
 echo "Error. $1 no es posible analizarlo ya que no se cuenta con permisos de lectura o no existe"
}
if test $# -lt 2; then
 ErrorS
fi
if ! test -r $1; then
 ErrorP
elif test -f $1 && (test $2 = "L" || test $2 = "C" || test $2 = "M"); then
 if test $2 == "L"; then
 res=`wc -l $1`
 echo "La cantidad de lineas del archivo es: $res"
 elif test $2 == "C"; then
 res=`wc -m $1`
 echo "La cantidad de caracteres del archivo es: $res"
 elif test $2 == "M"; then
 res=`wc -L $1`
 echo "La cantidad maxima de caracteres en una linea es: $res"
 fi
else
 ErrorS
fi
