#! /bin/bash
#crear mi propia papelera de reciclaje
if [[ $1 = "-h" || $1 = "--help" || $1 = "-?" ]]; then
    #TODO: mostrar ayuda
    exit 0
fi
> script.log
#funciones para cada opcion
listar(){ #listo
    cat /$HOME/papelera.zip/log.txt
}

vaciar(){ #listo
    rm -r /$HOME/papelera.zip
    mkdir /$HOME/papelera.zip
    > /$HOME/papelera.zip/log.txt
}

recuperar(){
    if [[ -f /$HOME/papelera.zip/$1 ]]; then
        cp /$HOME/papelera.zip/$1 `cat /$HOME/papelera.zip/log.txt | grep $1 | cut -d " " -f 2`
        rm /$HOME/papelera.zip/$1
        sed -i "/$1/d" /$HOME/papelera.zip/log.txt
    else
        echo "El archivo no existe en la papelera"
    fi
}

eliminar(){ #listo
    logPath="/$HOME/papelera.zip/log.txt"
    cp $1 /$HOME/papelera.zip
    pathArchivo=$(realpath $1);
    nombreArchivo=$(basename $1);
    echo "$nombreArchivo  $(dirname $pathArchivo)" >> $logPath
    rm $1
}

borrar(){
    echo TODOborrar
}



#si no existe la papelera, la creo
if [[ -d $HOME/papelera.zip ]]
then
    echo "La papelera ya existe" >> script.log
else
    mkdir $HOME/papelera.zip
    echo "La papelera se ha creado" >> script.log
fi

#si no existe el log, lo creo
if [[ -f $HOME/papelera.zip/log.txt ]]
then
    echo "El log ya existe" >> script.log
else
    touch $HOME/papelera.zip/log.txt
    echo "El log se ha creado" >> script.log
fi

#las opciones son --listar, --vaciar, --recuperar, --eliminar, --borrar

if ! [[ $1 = "--listar" || $1 = "--vaciar" || $1 = "--recuperar" || $1 = "--eliminar" || $1 = "--borrar" ]]; then
    echo "Opcion invalida" >> script.log
    exit 1
fi

#si la opcion es listar, vaciar, recuperar, eliminar o borrar, ejecuto la funcion correspondiente
if [[ $1 = "--listar" ]]; then #sin params
    listar
elif [[ $1 = "--vaciar" ]]; then #sin params
    vaciar
elif [[ $1 = "--recuperar" ]]; then #param: archivo
    recuperar $2
elif [[ $1 = "--eliminar" ]]; then #param: archivo
    eliminar $2
elif [[ $1 = "--borrar" ]]; then #param: archivo
    borrar $2
fi
