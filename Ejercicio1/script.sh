#!/bin/bash
ErrorS()  #desarrollo de la funcion ErrorS para indicar error de ingreso de parametros, con echo mostramos los mensajes por pantalla
{
 echo "Error. La sintaxis del script es la siguiente:"
 echo "Cantidad de lineas: script.sh nombre_archivo L"
 echo "Cantidad de caracteres: script.sh nombre_archivo C"
 echo "Cantidad maxima de caracteres en una linea: script.sh nombre_archivo M"
}
ErrorP() #desarrollo de la funcion ErrorP para indicar que el script no existe o no posee permisos de lectura
{
 echo "Error. $1 no es posible analizarlo ya que no se cuenta con permisos de lectura o no existe"
}
if test $# -lt 2; then #chequeamos que la cantidad de parametros sea la correcta, sino no es asi llamo a la funcion de Error
 ErrorS
fi
if ! test -r $1; then #chequeamos existencia y permiso de lectura del archivo enviado por parametreo, si no cumple llamamos a la funcion de error
 ErrorP
elif test -f $1 && (test $2 = "L" || test $2 = "C" || test $2 = "M"); then #si cumple chequeamos que el parametro de opcion sea VALIDO.
 if test $2 == "L"; then #si el parametro es valido y es 'L'
 res=`wc -l $1`  #guardamos en la variable res la cantidad de lineas del script y mostramos por pantalla el resultado
 echo "La cantidad de lineas del archivo es: $res"
 elif test $2 == "C"; then #si el parametro es valido y es 'C'
 res=`wc -m $1` #guardamos en la variable res la cantidad de caracteres del script y mostramos por pantalla el resultado
 echo "La cantidad de caracteres del archivo es: $res"
 elif test $2 == "M"; then #si el parametro es valido y es 'M'
 res=`wc -L $1` #guardamos en la variable res la cantidad maxima de caracteres en una linea del script y mostramos por pantalla el resultado
 echo "La cantidad maxima de caracteres en una linea es: $res"
 fi
else  #si el parametro no es valido, llamamos a la funcion ErrorS con todos los detalles para el uso del script
 ErrorS
fi

# Respuestas
# 1. El objetivo del script es calcular la cantidad de lineas, caracteres de un archivo o su maxima cantidad de caracteres en una linea.
# 2. Recibe el path del archivo a analizar en $1 y la accion a realizar (L, C o M) en $2.
# 3. Si la cantidad de parámetros del script es menor a 2 llama a la función ErrorS
    
#     Si el parametro 1 no posee permiso de lectura para analizar el archivo de texto se llama a la funcion ErrorP para indicarlo
    
#     Sino si el parametro 1 es un fichero regular y el parametro 2 es una L o una C o una M. Ejecuta lo siguiente:
    
#     Si el parametro 2 es una L entonces le asigna a res la cantidad de lineas del parametro1 y lo muestra
    
#     Si el parametro 2 es una C entonces le asigna a res la cantidad de caracteres del parametro 1 y lo muestra
    
#     Si el parametro 2 es una M entonces le asigna a res la cantidad maxima de caracteres en una linea del parametro 1 y lo muestra
    
#     Sino llama la funcion ErrorS para indicar que hay un error en la sintaxis del script y como debe ser
# 5. 
# | $# | Cantidad de parámetros enviados |
# | $1 $2 {$10} | Los parámetros enviados |
# | $@ o $* | La lista de todos los parámetros |
# | $$ | El Pid del proceso actual |
# | $! | El Pid del último proceso hijo ejecutado en segundo plano |
# | $? | Valor de ejecución del último comando |
# | $0 | nombre del archivo ejecutable |
# | $* | linea completa de la llamada |
    
# 6.
# “  “ comilla doble, texto debil.
# Permite que se interpreten ciertos caracteres de forma especial ($ sustitucion de variable, ` sustitucion de comando, \ caracter de escape)y ademas permite encerrar frases con espacios Por ejemplo, el caracter ($) se usa para indicar que se esta trabajando con una variable y queremos ver su valor, por lo tanto se visualizara el contenido; si variable=”hola”, al usar $hola dentro de estas comillas se va a visualizar lo siguiente: “hola”.
# Tambien se utilizan para representar cadenas con espacios. Por ejemplo, al pasar el nombre de un archivo con espacios a un script se debe hacer de la siguiente manera: “nombre del archivo.txt”
    
#  ‘   ‘ comilla simple, texto fuerte.
# No se realiza el reemplazo de variables. El texto encerrado es tomado de forma literal. Por ejemplo, si variable=”hola”, al usar $hola dentro de estas comillas se va a visualizar lo siguiente: $variable.
    
# `  `  acento grave
# Se utiliza para le ejecucion de comandos, ya que si con las comillas anteriores utilizamos un comando este sera interpretado como texto y no realizara ninguna accion. Por ejemplo, si variable=`ls`, luego cuando visualizemos el contenido de $variable no sera “ls” sino que va a ser el listado de los directorios mostrados por el comando ls.
