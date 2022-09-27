#APL N. 2
#SCRIPT 4
#INTEGRANTES:
#Carballo, Facundo Nicolas (DNI: 42774931)
#Garcia Burgio, Matias Nicolas (DNI: 42649117)
#Mottura, Agostina Micaela (DNI: 41898101)
#Povoli Olivera, Victor (DNI: 43103780)
#Szust, Ángel Elías (DNI: 43098495)
<#
.DESCRIPTION
Este script cuenta la cantidad de lineas de codigo y de comentarios que poseen 
los archivos en una ruta pasada por parametro controlando solo los archivos con cierta extension
Los archivos a analizar deben contar con fin de linea

.PARAMETER ruta
  ruta: ruta del directorio a analizar"

.PARAMETER ext
  extensiones: lista de las extensiones de los archivos a analizar, seguidas por coma"

.EXAMPLE
.\ejercicio-4 --ruta <ruta> --ext <extensiones>"
pwsh ./ejercicio-4 --ruta <ruta> --ext <extensiones>"
#>

Param(
    [Parameter(Mandatory = $True, Position = 1)]
    [validateScript({
        if (-Not ($_ | Test-Path -PathType container)){
            throw "La ruta ingresada es INVALIDA"
        } return $true })]
    [string]$ruta,

    [Parameter(Mandatory = $True, Position = 2)] [string]$ext
)

#----------------------------------------------------------------------------------------
#Funciones

function concatenarArchivos()
{
    Param (
            [string] $ruta,
            [string] $ext
    )

    $extensiones = "$ext".Split(",")
    $totalDeArchivos=0

    foreach($extension in $extensiones)
    {
        $contenido = Get-ChildItem -Path "$ruta" -Filter *.$extension -Recurse | Get-Content
        $totalDeArchivos += (Get-ChildItem -Path "$ruta" -Filter *.$extension -Recurse).count
        if($contenido)
        {
            $archivo += $contenido
        }
        else
        {
            Write-Warning "No se encontraron archivos para la extension $extension"
        }    
    }

    Write-Host "Cantidad de archivos analizados: $totalDeArchivos"
    return $archivo
}


#----------------------------------------------------------------------------------------
#main

$archivo = concatenarArchivos $ruta $ext

$estoyEnComentarioMultiple=0
$comentarios=0
$codigos=0

foreach($line in $archivo) {
    $line=$line.trim()
    
    if($estoyEnComentarioMultiple -eq 0) {
        $posicionAperturaComentarioMultiple = $line.IndexOf("/*")


        if($posicionAperturaComentarioMultiple -eq 0) {
            $estoyEnComentarioMultiple = 1
        } elseif ($posicionAperturaComentarioMultiple -gt 0) {
            $codigos++
            $estoyEnComentarioMultiple = 1
        }
    }

    if($estoyEnComentarioMultiple -eq 0) {
        $posicionComentarioSimple = $line.IndexOf("//") 

        if($posicionComentarioSimple -eq 0) {
            $comentarios++
        } elseif ($posicionComentarioSimple -gt 0) {
            $comentarios++
            $codigos++
        } else {
            $codigos++
        }
    }

    if($estoyEnComentarioMultiple -eq 1) {
        $posicionCierreComentarioMultiple = $line.IndexOf("*/")

        if($posicionCierreComentarioMultiple -ge 0) {
            $comentarios++
            $estoyEnComentarioMultiple =0
        } else {
            $comentarios++
        }

        if($posicionCierreComentarioMultiple -ne -1 -And $posicionCierreComentarioMultiple+2 -lt $line.Length)
        {
            $codigos++
        }
    }
}

$total=$comentarios+$codigos
$porcentajeComentario=$comentarios*100/$total
$porcentajeCodigo=$codigos*100/$total
"|{0,-11}|{1,9}|{2,11}|" -f "", "Cantidad", "Porcentaje"
"|{0,-11}|{1,9}|{2,10:n2}%|" -f "Comentarios", $comentarios, $porcentajeComentario
"|{0,-11}|{1,9}|{2,10:n2}%|" -f "Codigo", $codigos, $porcentajeCodigo
