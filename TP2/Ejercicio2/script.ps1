#APL N. 2
#SCRIPT 2
#INTEGRANTES:
#Carballo, Facundo Nicolas (DNI: 42774931)
#Garcia Burgio, Matias Nicolas (DNI: 42649117)
#Mottura, Agostina Micaela (DNI: 41898101)
#Povoli Olivera, Victor (DNI: 43103780)
#Szust, Ángel Elías (DNI: 43098495)

<#
.SYNOPSIS
Mostrar por pantalla un analisis general de un set de registros de llamadas telefonicas.

.DESCRIPTION
Este script analiza registros de llamadas telefonicas y muestra en pantalla la siguiente informacion:
    - Promedio de tiempo de las llamadas realizadas por dia.
    - Promedio de tiempo y cantidad por usuario por dia.
    - Los 3 usuarios con mas llamadas en la semana.
    - Cuantas llamadas no superan la media de tiempo por dia.
    - El usuario que tiene mas cantidad de llamdas por debajo de la media en la semana.

.PARAMETER logs
    logs: ruta del directorio que contiene los registros de las llamadas telefonicas
    Formato: FECHA HORA USUARIO
    Ejemplo: 2022-08-25 15:50:55 facu
    
.EXAMPLE
    ./script --logs <ruta>"
    pwsh ./script --logs <ruta>"
#>

# Verificamos la cantidad de parametros
Param(
    [Parameter(Mandatory = $True, Position = 1)]
    [switch]$logs,
    [Parameter(Mandatory = $True, Position = 2)]
    [validateScript({
        if (-Not ($_ | Test-Path -PathType container))
        {
            throw "La ruta ingresada es invalida."
        }
        return $true
    })]
    [string]$path
)


# TODO Verificamos que tengan los permisos de lectura correspondientes

# Declaracion de variables a utilizar
$timestampInit = Get-Date -Date "01/01/1970"
$mapEpochs = @{}

# Punto 1
$cantidadLlamadasPorDia = @{}
$tiempoDeLlamadaPorDia = @{}
$mediaDeTiempoPorDia = @{}
$diaInicioLlamada = @{}

# Punto 2
$tiempoDeLlamadaPorDiaPorUsuario = @{}
$cantidadLlamadasPorDiaPorUsuario = @{}

# Punto 3
$cantidadLlamadasPorUsuario = @{}

# Punto 4
$cantidadLlamadasNoSuperanMediaDiaria = @{}

# Punto 5
$tiempoDeLLamadaPorUsuarioSemanal = @{}
$cantidadLlamadasSemanal = 0
$tiempoDeLlamadaSemanal = 0
$cantidadDias = 0

$diaActual = ""

$files = Get-ChildItem "$path"


foreach ($f in $files) 
{
    # Ordenamos el archivo

    Sort-Object { [string]$_.split()[0] }

    foreach($line in Get-Content $f)
    {
        $dia = $line.SubString(0,10)
        $usuario = $line.SubString(20)

        if ("$diaActual" -ne "$dia")
        {
            $diaActual = $dia
            $cantidadDias++
        }

        if ($mapEpochs[$usuario])
        {
            # Finaliza una llamada de este usuario ($usuario)
            
            # Obtenemos la fecha desde el archivo de texto
            $fechaFin=$line.Substring(0,19)

            # Convertir la fecha a epoch
            $epochFin=(New-TimeSpan -Start $timestampInit -End $fechaFin).TotalSeconds
            
            # Obtener la diferencia entre epochs
            $difEpochs = $epochFin - $mapEpochs[$usuario]

            # Obtenemos el dia de inicio de la llamada (de timestamp a fecha)
            $diaInicioLargo = $diaInicioLlamada[$usuario]
            $diaInicio = $diaInicioLargo.Substring(0, 10)

            # Sumamos el tiempo de llamada por usuario en el dia que se inicio la llamada
            $tiempoDeLlamadaPorDiaPorUsuario["$diaInicio $usuario"] += $difEpochs

            # Aumentamos el tiempo de llamada por usuario en la semana
            $tiempoDeLLamadaPorUsuarioSemanal[$usuario]+=$difEpochs

            # Aumentamos los tiempos de llamada por dia
            $tiempoDeLlamadaPorDia[$diaInicio] += $difEpochs

            # Aumentamos los tiempos de llamada semanal
            $tiempoDeLlamadaSemanal+=$difEpochs

            # Volvemos a colocar el map del usuario en vacio
            $mapEpochs["$usuario"]=$null

        }
        else {
            
            # Comienza una llamada nueva de este usuario ($usuario)
            $cantidadLlamadasPorUsuario[$usuario]+=1

            # Aumentamos la cantidad de llamadas en ese dia
            $cantidadLlamadasPorDia[$dia]+=1

            # Aumentamos la cantidad de llamadas semanales
            $cantidadLlamadasSemanal+=1

            # Sumamos la cantidad de llamadas realizadas por el usuario en el dia
            $cantidadLlamadasPorDiaPorUsuario["$dia $usuario"]+=1

            # Obtenemos la fecha desde el archivo de texto
            $fechaInicio = $line.SubString(0,19)

            # Convertimos la fecha a epoch
            $epochInicio = (New-TimeSpan -Start $timestampInit -End $fechaInicio).TotalSeconds

            # Seteamos el dia inicio de la llamada
            $diaInicioLlamada[$usuario] = $fechaInicio

            # Actualizamos el map de este usuario
            $mapEpochs[$usuario]=$epochInicio

        }

    }

}

# foreach ($i in $mapEpochs.GetEnumerator())
# {
#     Write-Output "Name ($($i.Name)) => Value ($($i.Value))"
# }



# # Mostramos Punto 1
Write-Output "Punto 1. Promedio de tiempo de las llamadas realizadas por dia."
Write-Output ""

foreach ($i in $tiempoDeLlamadaPorDia.GetEnumerator())
{
    $promedio = [int]($($i.Value) / $cantidadLlamadasPorDia[$i.Name])

    # Obtenemos la media de tiempo por dia (Sirve para el Punto 4 y 5)
    $mediaDeTiempoPorDia[$i.Name] = $promedio

    $ts = [timespan]::fromseconds($promedio)
    $promedioFormateado = ("{0:hh\:mm\:ss}" -f $ts)

    Write-Output "Promedio el $($i.Name) : $promedioFormateado"

    Write-Output "----------------------------------"
}

Write-Output ""


# # # Mostramos Punto 2
Write-Output "Punto 2. Promedio de tiempo y cantidad por usuario por dia."
Write-Output ""

foreach ($i in $tiempoDeLlamadaPorDiaPorUsuario.GetEnumerator())
{
    $dia = $i.Name.ToString().Substring(0,10)
    $usuario = $i.Name.ToString().Substring(11)
    
    Write-Output "Dia: $dia | Usuario: $usuario"

    $promedio = [int] ($i.Value / $cantidadLlamadasPorDiaPorUsuario[$i.Name])
    
    $ts = [timespan]::fromseconds($promedio)
    $promedioFormateado = ("{0:hh\:mm\:ss}" -f $ts)

    Write-Output "Promedio de tiempo: $promedioFormateado"
    Write-Output "Cantidad de Llamadas: ${cantidadLlamadasPorDiaPorUsuario[$i]}"

    Write-Output "----------------------------------"
}
Write-Output ""


# # Mostramos Punto 3
Write-Output "Punto 3. Los 3 usuarios con mas llamadas en la semana."
Write-Output ""

$primerLugar=""
$segundoLugar=""
$tercerLugar=""

foreach ($i in $cantidadLlamadasPorUsuario.GetEnumerator())
{
    if ($primerLugar -eq "")
    {
        $primerLugar = $i.Name
    }
    elseif ($segundoLugar -eq "")
    {
        $segundoLugar = $i.Name
    }
    elseif ($tercerLugar -eq "")
    {
        $tercerLugar = $i.Name
    }
    elseif ($i.Value -gt $cantidadLlamadasPorUsuario[$primerLugar])
    {
        # Cambiamos el tercer lugar
        $tercerLugar=$segundoLugar
        
        # Cambiamos el segundo lugar
        $segundoLugar=$primerLugar
        
        # Cambiamos el primer lugar
        $primerLugar=$i.Name
    }
    elseif ($i.Value -gt $cantidadLlamadasPorUsuario[$segundoLugar])
    {
        # Cambiamos el tercer lugar
        $tercerLugar=$segundoLugar

        # Cambiamos el segundo lugar
        $segundoLugar=$i.Name
    }
    elseif ($i.Value -gt $cantidadLlamadasPorUsuario[$tercerLugar])
    {
        # Cambiamos el tercer lugar
        $tercerLugar=$i
    }
}

Write-Output "TOP 1 => $primerLugar con $($cantidadLlamadasPorUsuario[$primerLugar]) llamadas en la semana"

if ($segundoLugar -eq "")
{
    Write-Output "No existe TOP 2."
}
else
{
    Write-Output "TOP 2 => $segundoLugar con $($cantidadLlamadasPorUsuario[$segundoLugar]) llamadas en la semana"
}

if ($tercerLugar -eq "")
{
    Write-Output "No existe TOP 3."
}
else 
{
    Write-Output "TOP 3 => $tercerLugar con $($cantidadLlamadasPorUsuario[$tercerLugar]) llamadas en la semana"
}


Write-Output ""

foreach ($f in $files)
{
    # Ordenar Archivo en variable

    foreach ($line in Get-Content $f)
    {
        $dia = $line.SubString(0,10)
        $usuario = $line.SubString(20)

        if ($mapEpochs[$usuario])
        {
            # Finaliza una llamada de este usuario ($usuario)
            
            # Obtenemos la fecha desde el archivo de texto
            $fechaFin=$line.Substring(0,19)

            # Convertir la fecha a epoch
            $epochFin=(New-TimeSpan -Start $timestampInit -End $fechaFin).TotalSeconds
            
            # Obtener la diferencia entre epochs
            $difEpochs = $epochFin - $mapEpochs[$usuario]

            # Obtenemos el dia de inicio de la llamada (de timestamp a fecha)
            $diaInicioLargo = $diaInicioLlamada[$usuario]
            $diaInicio = $diaInicioLargo.Substring(0, 10)

            # Nos fijamos si tenemos que incrementar la cantidad de llamadas que no superan la media diaria
            if ($difEpochs -lt $mediaDeTiempoPorDia[$diaInicio])
            {
                $cantidadLlamadasNoSuperanMediaDiaria[$diaInicio]++
            }

            # Volvemos a colocar el map del usuario en vacio
            $mapEpochs["$usuario"]=$null
        }
        else 
        {
            # Obtenemos la fecha desde el archivo de texto
            $fechaInicio = $line.SubString(0,19)

            # Convertimos la fecha a epoch
            $epochInicio = (New-TimeSpan -Start $timestampInit -End $fechaInicio).TotalSeconds

            # Actualizamos el map de este usuario
            $mapEpochs[$usuario]=$epochInicio
        }
    }
}

# # # Mostramos Punto 4
Write-Output "Punto 4. Cuantas llamadas no superan la media de tiempo por dia"
Write-Output ""

foreach ($i in $cantidadLlamadasNoSuperanMediaDiaria.GetEnumerator())
{
    Write-Output "Dia: $($i.Name) => $($cantidadLlamadasNoSuperanMediaDiaria[$i.Name]) llamadas que no superan la media de ese dia."
}


Write-Output ""

# # Mostramos Punto 5
Write-Output "Punto 5. El usuario que tiene mas cantidad de llamadas por debajo de la media en la semana"
Write-Output ""

$maximo="0"
$usuario=""
$promedioCantidadLLamadasSemanal= [int] ($cantidadLlamadasSemanal / $cantidadDias)

foreach ($i in $cantidadLlamadasPorUsuario.GetEnumerator())
{
    if ($i.Value -le $promedioCantidadLLamadasSemanal -and $i.Value -gt $maximo)
    {
        $maximo = $i.Value
        $usuario = $i.Name
    }
}

Write-Output "$usuario con $maximo llamadas en la semana"

Write-Output ""