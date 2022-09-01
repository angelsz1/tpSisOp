#!/bin/bash
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
    cd $1
    ls | grep -r '' | cut -d ":" -f 2- > $pathActual"/"$2
    cd $pathActual
}

#si la ruta es vacia, uso Desktop

ruta=$2

if [[ $2 = "" ]]; then
    ruta="/home/`whoami`/Desktop"
fi


#creo el demonio
> nohup.out
if [[ $7 = "" ]]; then
    nohup ./script.sh $1 $ruta $3 $4 $5 $6 "1" &
    exit 0
fi
echo $0


#codigo del script

#chequeo por bin
bintest=`ls | grep "bin" | wc -w`
if [[ $bintest -ne 1 ]]; then
    mkdir bin
fi
if [[ ! -f "bin/concat.bin" ]]; then
    touch bin/concat.bin
fi


rutaScript=`pwd`
cd $ruta
ruta=`pwd`
echo "Monitorizando $ruta"
cd $rutaScript


#chequeo permisos de lectura sobre la ruta
if [[ ! -r $ruta ]]; then 
    echo "No tienes permisos de lectura sobre la ruta $ruta"
    exit 1
fi

#manejo del -a
acciones=`echo $4 | tr "," " "`
echo $acciones

#chequeo restricciones de acciones
for i in $acciones; do
    if [[ $i != "listar" && $i != "peso" && $i != "compilar" && $i != "publicar" ]]; then
        echo "La accion $i no es valida"
        exit 1
    fi
done

if [[ $acciones =~ "publicar" ]];then
    if [[ $6 = "1" ]]; then
        echo "La accion publicar requiere un directorio de publicacion"
        exit 1
    fi
    if [[ ! -d $6 ]]; then
        mkdir $6
    fi
    if [[ ! $acciones =~ "compilar" ]]; then
        echo "La accion publicar requiere la accion compilar"
        exit 1
    fi
    if [[ $5 = "1" ]]; then
        echo "La accion publicar requiere un path de publicacion"
        exit 1
    fi

fi


#vacio el log
> monitor_log

inotifywait -q -m -r -e create,modify,delete,moved_to $ruta | while read DIRECTORIO EVENTO ARCHIVO; do
    #echo $EVENTO
    case $EVENTO in
        MODIFY*)
            if [[ $acciones =~ "listar" ]]; then
                archivo_modificado "$DIRECTORIO" "$ARCHIVO"
            fi
            if [[ $acciones =~ "peso" ]]; then
            fulDir="$DIRECTORIO$ARCHIVO"
                echo "Peso = `du --apparent-size -h $fulDir`" | awk '{print $1,$2,$3}' | uniq >> monitor_log
            fi
            if [[ $acciones =~ "compilar" ]]; then #concatenar los archivos en $ruta y guardar en bin/concat.bin
                compilar $ruta "bin/concat.bin"
            fi
            if [[ $acciones =~ "publicar" ]]; then #publicar el archivo bin/concat.bin en $6
                cp bin/concat.bin $6
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
            if [[ $acciones =~ "compilar" ]]; then #concatenar los archivos en $ruta y guardar en bin/concat.bin
                compilar $ruta "bin/concat.bin"
            fi
            if [[ $acciones =~ "publicar" ]]; then #publicar el archivo bin/concat.bin en $6
                cp bin/concat.bin $6
            fi
            ;;
        DELETE*)
            if [[ $acciones =~ "listar" ]]; then
                archivo_borrado "$DIRECTORIO" "$ARCHIVO"
            fi
            #no muestro peso porque no se puede calcular
            if [[ $acciones =~ "compilar" ]]; then #concatenar los archivos en $ruta y guardar en bin/concat.bin
                compilar $ruta "bin/concat.bin"
            fi
            if [[ $acciones =~ "publicar" ]]; then #publicar el archivo bin/concat.bin en $6
                cp bin/concat.bin $6
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
            if [[ $acciones =~ "compilar" ]]; then #concatenar los archivos en $ruta y guardar en bin/concat.bin
                compilar $ruta "bin/concat.bin"
            fi
            if [[ $acciones =~ "publicar" ]]; then #publicar el archivo bin/concat.bin en $6
                cp bin/concat.bin $6
            fi
            ;;
    esac
done
