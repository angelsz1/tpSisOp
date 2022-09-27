<#
.DESCRIPTION
Este script emula el comportamiento de una papelera de reciclaje.
Acciones : (solo se puede utilizar una en cada ejecucion)
    -listar los archivos
    -eliminar un archivo de un directorio
    -borrar archivo de la papelera
    -vaciar la papelera

Nota: si quiere hacer referencia a un archivo o directorio con espacios, debe enviar el parametro entre comillas (" ")
.PARAMETER listar
    listar: Muestra el contenido de la papelera de reciclaje

.PARAMETER eliminar
    eliminar [archivo/dir]: Envia a la papelera el archivo/directorio pasado por parametro

.PARAMETER borrar
    borrar [archivo/dir]: Elimina de la papelera (definitivamente) al archivo o directorio pasado por parametro. El mismo debe estar en la papelera

.PARAMETER vaciar
    vaciar: Vacia la papelera

.EXAMPLE
    .\ejercicio-4.ps1 --eliminar ../archivo
#>

Param(

  [Parameter(ParameterSetName='listar')]
  [Parameter(Position = 1)]
  [Switch]
  $listar,

  [Parameter(ParameterSetName='eliminar')]
  [Parameter(Position = 1)]
  [String]
  $eliminar,

  [Parameter(ParameterSetName='vaciar')]
  [Parameter(Position = 1)]
  [Switch]
  $vaciar,

  [Parameter(ParameterSetName='borrar')]
  [Parameter(Position = 1)]
  [String]
  $borrar,

  [Parameter(ParameterSetName='recuperar')]
  [Parameter(Position = 1)]
  [String]
  $recuperar

)

function seleccionarRegistro() {
    Param (
        [Object] $registros
    )

    $registrosASeleccionar = $registros | Where-Object { $fileName -eq $_.FileName }

    if(!$registrosASeleccionar) {
        Write-Warning "El archivo $fileName no se encuentra en la papelera"
        Exit
    } 

    if ($registrosASeleccionar.Count -gt 1) {
        listar $registrosASeleccionar
        $idArchivo = Read-Host "¿Qué archivo desea seleccionar? (Ingrese Id)" 
        while(($idArchivo -lt 1) -Or ($idArchivo -gt $registrosASeleccionar.Count))  {
            Write-Warning "El Id ingresado es invalido. Intente nuevamente"
            listar $registrosASeleccionar
            $idArchivo = Read-Host "¿Qué archivo desea seleccionar? (Ingrese Id)" 
        }

        $registroSeleccionado = $registrosASeleccionar[$idArchivo-1]
    } else {
        $registroSeleccionado = $registrosASeleccionar[0]
    }
    return $registroSeleccionado
}

function listar() { 
    Param (
        [Object] $registros
    )
    
    if ($registros.Count -gt 0) {
        $id=0
        "|{0,-4}|{1,-11}|{2,-22}|{3,-20}" -f "Id","Name", "Date Deleted", "Original Location" | Write-Host -ForegroundColor DarkGreen
        foreach ($registro in $registros) {
            $id++
            "|{0,-4}|{1,-11}|{2,-22}|{3,-20}" -f $id, $registro.FileName, $registro.DateDeleted, $registro.Path | Write-Host
        }
    } else {
        Write-Host "La papelera esta vacia"
    }
}

function eliminar() {
    Param (
        [String] $rutaAEliminar,
        [String] $csvFile,
        [String] $papelera
    )

    if (-Not (Test-Path $rutaAEliminar)) {
        Write-Warning "Path invalido. $rutaAEliminar no encontrado"
        Exit
    }
    
    $fecha = Get-Date
    $randomNumber = [Math]::Round(($fecha).ToFileTimeUTC()/10000)
    $nombre = Get-Item $rutaAEliminar | ForEach-Object { $_.Name }
    $fullPath = Get-Item $rutaAEliminar | ForEach-Object { $_.FullName }
    $rutaSinNombre = $fullPath.subString(0, ($fullPath.length - $nombre.length -1))

    $nuevoNombre = $nombre+$randomNumber
    Rename-Item -Path $rutaAEliminar -NewName $nuevoNombre
    $newPath = "$rutaSinNombre/$nuevoNombre"

    $compress = @{
        Path = $newPath
        CompressionLevel = "Fastest"
        DestinationPath = $papelera
      }
      Compress-Archive @compress -Update

    Remove-Item -Path $newPath -Force -Recurse
    "{0},{1},{2},{3}" -f $nombre,$nuevoNombre,$fecha,$rutaSinNombre | Add-Content -path $csvFile

} 

function vaciar () {
    Param (
        [Object] $registros,
        [String] $archivoCsv,
        [String] $papelera
    )

    if ($registros.Count -gt 0) {
        Remove-Item $papelera -Recurse
        Set-Content -Value "" -path $archivoCsv
    }
    else
    {
        Write-Host "La papelera ya esta vacia"
    }
}

function borrar() {
    Param (
        [String] $fileName,
        [Object] $registros,
        [String] $papelera,
        [String] $archivoCsv
    )

    $registroABorrar = seleccionarRegistro $registros

    $registros = $registros | Where-Object { $registroABorrar.RemovedFileName -ne $_.RemovedFileName }
    Remove-Item $archivoCsv

    if($registros.Count -eq 0) {
        Set-Content -Value "" -path $archivoCsv
    } else {
        foreach ($registro in $registros) {
            "{0},{1},{2},{3}" -f $registro.FileName, $registro.RemovedFileName, $registro.DateDeleted, $registro.Path | Add-Content -path $archivoCsv
        }
    }

    $update = 2
    $zip = [IO.Compression.ZipFile]::Open($papelera, $update)  
    if($zip) {
        $entries = $zip.Entries | Where-Object { $_.FullName.StartsWith($registroABorrar.RemovedFileName) }
        if(!$entries) {
            $zip.Dispose()
            Write-Error "Error Borrando Archivo"
            Exit
        }
        foreach($entry in $entries) {
            $entry.Delete()
        }
    }
    $zip.Dispose()

    Write-Host "$fileName borrado de la papelera" -ForegroundColor DarkGreen
} 

function recuperar() {
    Param (
        [String] $fileName,
        [String] $papelera,
        [Object] $registros
    )

    registroARecuperar = seleccionarRegistro $registros
}

function crearPapelera() {
    Param (
        [String] $archivoClave,
        [String] $papelera,
        [String] $archivoCsv
    )

    New-Item $archivoClave -ItemType File | Out-Null
    Compress-Archive -Path $archivoClave -DestinationPath $papelera -CompressionLevel Optimal -Update
    Remove-Item $archivoClave
    
    if( Test-Path $archivoCsv )
    {
        Remove-Item $archivoCsv
    }
    New-Item $archivoCsv | Out-Null
}

$archivoCsv = './registros.csv'
$papelera = "$home/papelera.Zip"
$archivoClave = "AgosMatiVictorAngelFacu.txt"

try {
    $zip = [IO.Compression.ZipFile]::OpenRead($papelera)  
    if($zip) {
        $entries = $zip.Entries | Where-Object {$_.FullName -like $archivoClave} 
        if(!$entries) {
            $zip.Dispose()
            Remove-Item $papelera
            Write-Output "Borrando Papelera"
            crearPapelera $archivoClave $papelera $archivoCsv
        }
    }
    $zip.Dispose()
}
catch [Exception] {
    crearPapelera $archivoClave $papelera $archivoCsv
}

$header = @('FileName','RemovedFileName','DateDeleted','Path')
$registros = Import-Csv -Path $archivoCsv -Header $header 

if($listar) {
    listar $registros
} elseif($eliminar) {
    eliminar $eliminar $archivoCsv $papelera
} elseif($vaciar) {
    vaciar $registros $archivoCsv $papelera
} elseif($borrar) {
    borrar $borrar $registros $papelera $archivoCsv
} elseif($recuperar) {
    recuperar $recuperar $papelera $registros
}

