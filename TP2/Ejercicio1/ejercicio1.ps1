#APL N. 2
#SCRIPT 1
#INTEGRANTES:
#Carballo, Facundo Nicolas (DNI: 42774931)
#Garcia Burgio, Matias Nicolas (DNI: 42649117)
#Mottura, Agostina Micaela (DNI: 41898101)
#Povoli Olivera, Victor (DNI: 43103780)
#Szust, Ángel Elías (DNI: 43098495)

Param (
    [Parameter (Position = 1, Mandatory = $false)]
    [String] $pathSalida = ".\procesos.txt",
    [int] $cantidad = 3
)

$existe = Test-Path $pathSalida
if($existe -eq $true){
    $procesos = Get-Process | Where-Object { $_.WorkingSet -gt 100MB }

    $procesos | Format-List -Property Id, Name >> $pathSalida

    for ($i = 0; $i -lt $cantidad; $i++) {
        Write-Host $procesos[$i].Id - $procesos[$i].Name
    }
}
else {
    Write-Host "El path no existe"
}

Write-Host "
RESPUESTAS:"
Write-Host "1. El objetivo del script es:
    -Escribir en el archivo de texto (ya existente) los procesos que tienen un conjunto de trabajo (RAM utilizada) mayor a 100MB
    -De los procesos que superan los 100 MB, mostrar por pantalla los 3 con menos MB ocupados"
Write-Host '
2. En cuanto a las validaciones:
    -Haría que el path del txt salida fuera obligatorio [Parameter (Position = 1, Mandatory = $true)]
    -La cantidad de procesos a listar por pantalla tambien sea obligatorio en posicion 2 [Parameter (Position = 2, Mandatory = $true)]'
Write-Host '
3. Si se ejecuta el script sin parametros chequea la existencia del path e informa que el path "enviado" no existe'