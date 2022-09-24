#!/bin/bash

#APL N. 1
#SCRIPT 3
#INTEGRANTES:
      #Carballo, Facundo Nicolas (DNI: 42774931)
      #Garcia Burgio, Matias Nicolas (DNI: 42649117)
      #Mottura, Agostina Micaela (DNI: 41898101)
      #Povoli Olivera, Victor (DNI: 43103780)
      #Szust, Ángel Elías (DNI: 43098495)


if [[ $1 = "-h" || $1 = "--help" || $1 = "-?" ]]; then
    echo "Es necesario instalar inotify-tools poder ejecutar este script"
    echo "Para instalarlo, ejecute el siguiente comando:"
    echo "sudo apt-get install inotify-tools"
    echo "Sintaxis : $0 -c DIRECTORIO -a ACCION,... [-s DIRECTORIO]"
    echo "-c DIRECTORIO : Directorio a monitorizar"
    echo "-a ACCION,... : Accion(es) a realizar. Las acciones posibles son: listar, peso, compilar, publicar"
    echo "-s DIRECTORIO : Path donde se publica el archivo compilado si la accion es publicar"
    echo "Ejemplo : $0 -c /home/`whoami`/proyecto -a listar,compilar,publicar -s /home/`whoami`/publicacion"
    echo "Nota : Si la accion \"publicar\" es seleccionada, la accion \"compilar\" debe ser seleccionada, y se debe proveer un directorio para publicar el archivo compilado"
    exit 0
fi

#funciones para parsear la salida del inotifywait

archivo_borrado() {
    echo "$2 fue eliminado de $1" | uniq  >> monitor_log
}

archivo_modificado() {
    echo "El archivo $1$2 fue modificado"  | uniq >> monitor_log
}

archivo_creado() {
    echo "El archivo $1$2 fue creado"  | uniq >> monitor_log
}
archivo_renombrado(){
    echo "Un archivo cambio de nombre a $1" | uniq >> monitor_log
}

#compilar los archivos de un directorio
compilar(){
    pathActual=`pwd`
    cd "$1"
    echo `pwd`
    ls | grep -r '' | cut -d ":" -f 2- > "$pathActual"/"$2"
    cd "$pathActual"
}
if [[ $7 = "" ]];then

    if [[ $1 != "-c" ]]; then
        echo "Error: debe utilizar -c para especificar el directorio a monitorizar"
        echo "Para mas informacion, ejecute $0 -h"
        exit 1
    elif [[ "$2" = "" || ! -d "$2" ]]; then
        echo "Error: no se especifico un directorio valido"
        echo "Para mas informacion, ejecute $0 -h"
        exit 1
    fi

    if [[ $3 != "-a" ]]; then
        echo "Error: debe utilizar -a para listar las acciones a realizar"
        echo "Para mas informacion, ejecute $0 -h"
        exit 1
    elif [[ $4 = "" ]]; then
        echo "Error: No se especifico ninguna accion a realizar"
        echo "Para mas informacion, ejecute $0 -h"
        exit 1
    fi

    #manejo del -a
    acciones=`echo $4 | tr "," " "`

    #chequeo restricciones de acciones
    for i in $acciones; do
        if [[ $i != "listar" && $i != "peso" && $i != "compilar" && $i != "publicar" ]]; then
            echo "La accion $i no es valida"
            exit 1
        fi
    done

    if [[ $acciones =~ "publicar" ]];then
        if [[ $5 != "-s" ]]; then
            echo "Si la accion \"publicar\" es seleccionada, la accion \"compilar\" debe ser seleccionada, y se debe proveer un directorio con el comando -s para publicar el archivo compilado"
            exit 1
        else
            if [[ $6 = "" ]]; then
                echo "La accion publicar requiere un directorio de publicacion"
                exit 1
            else
                if [[ ! -d "$6" ]]; then
                    mkdir "$6"
                fi
                if [[ ! -r "$6" ]]; then
                    echo "No tienes permisos de lectura sobre la ruta $6"
                    exit 1
                fi
            fi
            if [[ ! $acciones =~ "compilar" ]]; then
                echo "La accion publicar requiere la accion compilar"
                exit 1
            fi
        fi

        
    elif [[ $5 = "-s" ]]; then
        echo "La accion publicar no fue seleccionada"
        exit 1
    fi
fi

#creo el demonio
> nohup.out
if [[ $7 = "" ]]; then
    nohup ./script.sh $1 "$2" $3 $4 $5 "$6" "1" &
    exit 0; 
fi


#manejo del -a
acciones=`echo $4 | tr "," " "`

#chequeo por bin
bintest=`ls | grep "bin" | wc -w`
if [[ $acciones =~ "compilar" ]]; then
    if [[ $bintest -ne 1 ]]; then
        mkdir bin
    fi
    if [[ ! -f "bin/concat.con" ]]; then
        touch bin/concat.con
    fi

    compilar "$2" "bin/concat.con"
    if [[ $acciones =~ "publicar" ]]; then
        rutaDestino="$6"
        cp "bin/concat.con" "$rutaDestino"
    fi

fi

echo "Monitorizando "$2"..."
#chequeo permisos de lectura sobre la ruta
if [[ ! -r "$2" ]]; then 
    echo "No tienes permisos de lectura sobre la ruta $2"
    exit 1
fi


#vacio el log
> monitor_log

inotifywait -q -m -r -e create,modify,delete,moved_to "$2" | while read DIRECTORIO EVENTO ARCHIVO; do
    case $EVENTO in
        MODIFY*)
            if [[ $acciones =~ "listar" ]]; then
                archivo_modificado "$DIRECTORIO" "$ARCHIVO"
            fi
            if [[ $acciones =~ "peso" ]]; then
            fulDir="$DIRECTORIO$ARCHIVO"
                echo "Peso = `du --apparent-size -h "$fulDir"`" | awk '{print $1,$2,$3}' | uniq >> monitor_log
            fi
            if [[ $acciones =~ "compilar" ]]; then #concatenar los archivos en $2 y guardar en bin/concat.con
                compilar "$2" "bin/concat.con"
            fi
            if [[ $acciones =~ "publicar" ]]; then #publicar el archivo bin/concat.con en $6
                cp "bin/concat.con" "$rutaDestino"
            fi
                
            ;;
        CREATE*)
            if [[ $acciones =~ "listar" ]]; then
                archivo_creado "$DIRECTORIO" "$ARCHIVO"
            fi
            if [[ $acciones =~ "peso" ]]; then
                fulDir="$DIRECTORIO$ARCHIVO"
                echo "Peso = `du --apparent-size -h $fulDir`" | awk '{print $1,$2,$3}' | uniq>> monitor_log
            fi
            if [[ $acciones =~ "compilar" ]]; then #concatenar los archivos en $2 y guardar en bin/concat.con
                compilar "$2" "bin/concat.con"
            fi
            if [[ $acciones =~ "publicar" ]]; then #publicar el archivo bin/concat.con en $6
                cp "bin/concat.con" "$rutaDestino"
            fi
            ;;
        DELETE*)
            if [[ $acciones =~ "listar" ]]; then
                archivo_borrado "$DIRECTORIO" "$ARCHIVO"
            fi
            #no muestro peso porque no se puede calcular
            if [[ $acciones =~ "compilar" ]]; then #concatenar los archivos en $2 y guardar en bin/concat.con
                compilar "$2" "bin/concat.con"
            fi
            if [[ $acciones =~ "publicar" ]]; then #publicar el archivo bin/concat.con en $6
                cp "bin/concat.con" "$rutaDestino"
            fi
            ;;
        #RENAMED
        MOVED_TO*)
            if [[ $acciones =~ "listar" ]]; then
                archivo_renombrado "$ARCHIVO"
            fi
            if [[ $acciones =~ "peso" ]]; then
                fulDir="$DIRECTORIO$ARCHIVO"
                echo "Peso = `du --apparent-size -h $fulDir`" | awk '{print $1,$2,$3}' | uniq >> monitor_log
            fi
            if [[ $acciones =~ "compilar" ]]; then #concatenar los archivos en $2 y guardar en bin/concat.con
                compilar "$2" "bin/concat.con"
            fi
            if [[ $acciones =~ "publicar" ]]; then #publicar el archivo bin/concat.con en $6
                cp "bin/concat.con" "$rutaDestino"
            fi
            ;;
    esac
done
