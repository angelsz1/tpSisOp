#!/bin/bash

#Menu de ayuda
if [[ $1 = "-h" || $1 = "--help" || $1 = "-?" ]]; then
    #TODO ayuda
    echo "Sintaxis : $0 --notas ARCHIVO --materias ARCHIVO"
    echo "--notas ARCHIVO : Ruta del archivo con el registro de las notas de los alumnos a procesar"
    echo "Formato: DNI|IdMateria|PrimerParcial|SegundoParcial|Recuperatorio|Final"
    echo "--materias ARCHIVO: Archivo con los datos de las materias para poder vincularlos al archivo de notas"
    echo "Formato: IdMateria|Descripcion|Departamento"
    echo "Ejemplo : $0 --notas /home/`whoami`/proyecto --materias /home/`whoami`/publicacion"
    exit 0
fi

#Check cantidad de parametros
if [[ $# != 4 ]]
then
	echo "Cantidad de parametros incorrecta, deben ser 4"
	exit 1
fi

#Check de que los parametros --notas y --materias sean correctos
if [[ $1 != "--notas" || $3 != "--materias" ]]
then
	echo "Parametros invalidos"
	exit 1
fi

#Check de permisos de escritura para el archivo de salida json
if [[ ! -w  $0 ]]
then
	echo "No hay permiso de escritura para escribir el archivo de salida .json en `pwd` "
	exit 1
fi

#Check de que no exista un archivo de salida llamado igual que el que genera el programa
num=0
printf -v hoy '%(%Y-%m-%d)T'
printf -v nombre "%s-salida" $hoy
printf -v salida "%s-salida.json" $hoy

while [ -e "$salida" ]
do
    printf -v salida '%s-%02d.json' "$nombre" "$(( ++num ))"
done

#Elimino los espacios de los parametros donde estan las rutas
pathNotas=`echo $2 | tr -d " "`
pathMaterias=`echo $4 | tr -d " "`

#Check de que los archivos a procesar son ficheros
if [[ ! -f  $pathNotas ]]
then
	echo "El archivo de notas no es un fichero regular"
	exit 1
fi

if [[ ! -f  $pathMaterias ]]
then
	echo "El archivo de materias no es un fichero regular"
	exit 1
fi

#Check de permisos de lectura en los archivos a procesar
if [[ ! -r  $pathNotas ]]
then
	echo "No hay permiso de lectura para procesar el archivo de notas"
	exit 1
fi

if [[ ! -r  $pathMaterias ]]
then
	echo "No hay permiso de lectura para procesar el archivo de materias"
	exit 1
fi

#Procesamiento de las notas y materias
awk -F "|" 'FNR==NR {
		if(NR>1)
			departamentos[$1]=$3;
			descripciones[$1]=$2;
			next;
	}
	{
		if(FNR>1)
		{
			DNI=$1;
			MATERIA=$2;
			P1=$3;
			P2=$4;
			REC=$5;
			FIN=$6;
			printf "%s|%s|%s|%s|%s|%s|%s|%s\n", DNI, MATERIA, P1, P2, REC, FIN, departamentos[MATERIA], descripciones[MATERIA];
		}
		
	}
	' $pathMaterias $pathNotas | sort -t'|' -k7,7 -k2,2 | awk -F "|" '
function evaluarNota () {
	if(FIN == "")
		{
			if( (P1=="" && P2=="") || ( (P1=="" || P2=="") && REC=="" ) )
			{
				abandonos++;
			}
			else if ( ( P1>=7 && (REC>=7 || (P2>=7 && REC=="")) ) || ( (P1>=4 && P1<7) && (P2>=7 && REC>=7) ) || ( P1<4 && (P2>=7 && REC>=7) ) )
			{
				promocionados++;
			}
			else if ( ( (P1>=7 && ( (REC>=4 && REC<7) || (P2>=4 && P2<7 && REC==""))) ) || ( (P1>=4 && P1<7)  && ((REC>=4 && REC<7) || (P2>=7 && REC=="") || (P2>=4 && P2<7 && (REC>=4 || REC=="")) || (P2<4 && REC>=4)) ) || ( P1<4 && ((P2>=7 && REC>=4 && REC<7) || ((P2>=4 && P2<7) && (REC>=4 && REC<7))) ) )
			{
				finales++;
			}
			else if( ( P1>=4 && REC<4 ) || ( P1<4 && (REC<4 || P2<4)  ) )
			{
				recursantes++;
			}
		}
	else if(FIN < 4)
		recursantes++;
}

function mostrarResumenMaterias () {
	printf "\t\t\t\t{\n"
	printf "\t\t\t\t\t\"id_materia\": %d,\n", LASTMATERIA;
	printf "\t\t\t\t\t\"descripcion\": \"%s\",\n", LASTDESCRIPCION;
	printf "\t\t\t\t\t\"final\": %d,\n", finales;
	printf "\t\t\t\t\t\"recursan\": %d,\n", recursantes;
	printf "\t\t\t\t\t\"abandonaron\": %d,\n", abandonos;
	printf "\t\t\t\t\t\"promocionan\": %d\n", promocionados;
	printf "\t\t\t\t}"
}

function mostrarIDNotasTitulo () {
	printf "\t\t{\n";
	printf "\t\t\t\"id\": %d,\n", $7;
	printf "\t\t\t\"notas\": [\n";
}

function resetContadores () {
	abandonos=0;
	promocionados=0;
	finales=0;
	recursantes=0;
}

{
	if(NR==1)
	{
		printf "{\n";
		printf "\t\"departamentos\": [\n"
		mostrarIDNotasTitulo();
		
		LASTDEPTO=$7;
		LASTMATERIA=$2;
		LASTDESCRIPCION=$3;
		
		resetContadores();
	}
	
	DNI=$1;
	MATERIA=$2;
	P1=$3;
	P2=$4;
	REC=$5;
	FIN=$6;
	DEPTO=$7;
	NOMBRE=$8;
	
	if(DEPTO==LASTDEPTO)
	{
		if(MATERIA==LASTMATERIA)
		{
			evaluarNota();
		}
		else
		{
			mostrarResumenMaterias();
			printf ",\n";
			
			resetContadores();
			evaluarNota();
		}
	}
	else
	{
		mostrarResumenMaterias();
		printf "\n";
		printf "\t\t\t]\n";
		printf "\t\t},\n";
		
		resetContadores();
		evaluarNota();

		mostrarIDNotasTitulo();
	}
	LASTDEPTO=DEPTO;
	LASTMATERIA=MATERIA;
	LASTDESCRIPCION=NOMBRE;
}

END {
	mostrarResumenMaterias();
	printf "\n";
	printf "\t\t\t]\n"
	printf "\t\t}\n"
	
	printf "\t]\n"
	printf "}";
}
' > $salida

echo "Archivo de salida guardado en `pwd`/$salida"

#FINAL EJERCICIO 5