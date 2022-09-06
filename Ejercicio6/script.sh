#! /bin/bash

#APL N. 1
#SCRIPT 6
#INTEGRANTES:
      #Carballo, Facundo Nicolas (DNI: 42774931)
      #Garcia Burgio, Matias Nicolas (DNI: 42649117)
      #Mottura, Agostina Micaela (DNI: 41898101)
      #Povoli Olivera, Victor (DNI: 43103780)
      #Szust, Ángel Elías (DNI: 43098495)



#crear mi propia papelera de reciclaje
if [[ $1 = "-h" || $1 = "--help" || $1 = "-?" ]]; then
    ayuda
    exit 0
fi
> script.log
logPath="$HOME/papelera.zip/logPapeleraAMAVF.txt" #iniciales del grupo, para chequear que sea mi papelera
papeleraPath="$HOME/papelera.zip"
#funciones para cada opcion
listar(){ #listo
    cat $logPath | cut -d " " -f -2 
}

vaciar(){ #listo
    rm -r $papeleraPath
    mkdir $papeleraPath
    > $logPath
}
ayuda(){
    echo "Sintaxis : $0 ACCION [archivo/dir]"
    echo "Acciones : "
    echo "--listar: Muestra el contenido de la papelera de reciclaje"
    echo "--eliminar [archivo/dir]: Envia a la papelera el archivo/directorio pasado por parametro"
    echo "--borrar [archivo/dir]: Elimina de la papelera (definitivamente) al archivo o directorio pasado por parametro. El mismo debe estar en la papelera"
    echo "--vaciar: Vacia la papelera"
    echo "Ejemplo: $0 --eliminar ../archivo"
}

recuperar(){ #listo

    if [[ `cat $logPath | grep $1 | awk 'END{print NR}'` -gt 1 ]]; then
        grep $1 < $logPath | awk '{printf "%d - %s %s\n", NR, $1, $2}'
        echo "Cual queres recuperar?"
        read archNum
        nuevoNombre=`grep $1 < $logPath | awk -v var=$archNum '{if(NR == var) printf "%s|%d" , $1, $3}'` 
        IDarch=`grep $1 < $logPath | awk -v var=$archNum '{if(NR == var) print $3}'`
        mv $papeleraPath/$nuevoNombre $papeleraPath/$1 

        #si es un file
        if [[ -f $papeleraPath/$1 ]]; then
            cp $papeleraPath/$1 `grep $1 < $logPath | awk -v var=$archNum '{if(NR == var) print $2}'`
            rm $papeleraPath/$1 
        #si es un dir
        elif [[ -d $papeleraPath/$1 ]]; then 
            cp -r $papeleraPath/$1 `grep $1 < $logPath | awk -v var=$archNum '{if(NR == var) print $2}'` 
            rm -r $papeleraPath/$1 
        fi

        sed -i "/$IDarch/d" $logPath


    elif [[ `cat $logPath | grep $1 | awk 'END{print NR}'` -eq 1 ]]; then
        nuevoNombre=`grep $1 < $logPath | awk '{printf "%s|%d" , $1, $3}'` #consigo el nuevo nombre del archivo dentro de la papelera
        mv $papeleraPath/$nuevoNombre $papeleraPath/$1 #le cambio el nombre al original

        #si es un file
        if [[ -f $papeleraPath/$1 ]]; then
            cp $papeleraPath/$1 `grep $1 < $logPath | awk '{print $2}'` 
            rm $papeleraPath/$1 
        #si es un dir
        elif [[ -d $papeleraPath/$1 ]]; then 
            cp -r $papeleraPath/$1 `grep $1 < $logPath | awk '{print $2}'` 
            rm -r $papeleraPath/$1 
        fi

        sed -i "/$1/d" $logPath    #borro la linea del log
    else
        echo "El archivo $1 no se encuentra en la papelera de reciclaje"
        exit 1;
    fi
}

eliminar(){ #listo
    if [[ -d $1 ]]; then
        cp -r $1 $papeleraPath
    else
        cp $1 $papeleraPath
    fi
    pathArchivo=$(realpath $1);
    nombreArchivo=$(basename $1);
    dateRand=$(date +%s)$RANDOM
    mv $papeleraPath/$nombreArchivo "$papeleraPath/$nombreArchivo|$dateRand" #le cambio el nombre al archivo para que no se repita
    echo "$nombreArchivo $(dirname $pathArchivo) $dateRand" >> $logPath
    if [[ -d $1 ]]; then
        rm -r $1
    else
        rm $1
    fi
}

borrar(){
    if [[ `cat $logPath | grep $1 | awk 'END{print NR}'` -gt 1 ]]; then
        grep $1 < $logPath | awk '{printf "%d - %s %s\n", NR, $1, $2}'
        echo "Cual queres borrar?"
        read archNum
        nuevoNombre=`cat $logPath | grep $1 | awk -v var=$archNum '{if(NR == var) printf "%s|%d" , $1, $3}'` 
        IDarch=`grep $1 < $logPath | awk -v var=$archNum '{if(NR == var) print $3}'`
        mv $papeleraPath/$nuevoNombre $papeleraPath/$1 

        #si es un file
        if [[ -f $papeleraPath/$1 ]]; then
            rm $papeleraPath/$1 
        #si es un dir
        elif [[ -d $papeleraPath/$1 ]]; then 
            rm -r $papeleraPath/$1 
        fi

        sed -i "/$IDarch/d" $logPath


    elif [[ `cat $logPath | grep $1 | awk 'END{print NR}'` -eq 1 ]]; then
        nuevoNombre=`cat $logPath | grep $1 | awk '{printf "%s|%d" , $1, $3}'` 
        mv $papeleraPath/$nuevoNombre $papeleraPath/$1 

        #si es un file
        if [[ -f $papeleraPath/$1 ]]; then
            rm $papeleraPath/$1 
        #si es un dir
        elif [[ -d $papeleraPath/$1 ]]; then 
            rm -r $papeleraPath/$1 
        fi

        sed -i "/$1/d" $logPath    #borro la linea del log
    else
        echo "El archivo $1 no se encuentra en la papelera de reciclaje"
        exit 1;
    fi
}



#si no existe la papelera, la creo
if [[ -d $papeleraPath ]]
then
    if [[ -f $logPath ]]; then
        echo "La papelera ya existe" >> script.log
    else
        echo "La papelera existente no fue creada con este script"
        rm -r $papeleraPath
        mkdir $papeleraPath
    fi
else
    mkdir $papeleraPath
    echo "La papelera se ha creado" >> script.log
fi

#si no existe el log, lo creo
if [[ -f $logPath ]]
then
    echo "El log ya existe" >> script.log
else
    touch $logPath
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
    if [[ $2 != "" ]]; then
        recuperar $2
    else
        echo "Debe proporcionar un archivo/directorio a recuperar"
        ayuda
        exit 1;
    fi
elif [[ $1 = "--eliminar" ]]; then #param: archivo
    if [[ -f $2 || -d $2 ]]; then
        eliminar $2
    else
        echo "$2 no es un parametro valido"
        ayuda
        exit 1;
    fi
elif [[ $1 = "--borrar" ]]; then #param: archivo
    if [[ $2 != "" ]]; then
        borrar $2
    else
        echo "Debe proporcionar un archivo/directorio a borrar"
        ayuda
        exit 1;
    fi
fi
