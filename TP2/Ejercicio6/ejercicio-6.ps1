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

function listar() { 
    Param (
        $registros
    )
    
    if ($registros.Count -gt 0) {
        $id=0
        "|{0,-4}|{1,-11}|{2,-22}|{3,-20}" -f "Id","Name", "Date Deleted", "Original Location" | Write-Host -ForegroundColor DarkGreen
        foreach ($registro in $registros) {
            $id++
            "|{0,-4}|{1,-11}|{2,-22}|{3,-20}" -f $id, $registro.FileName, $registro.dateDeleted, $registro.path | Write-Host
        }
    } else {
        Write-Host "La papelera esta vacia"
    }
}

function eliminar() {
    Param (
        [string] $rutaAEliminar,
        [string] $csvFile,
        [string] $recycleBinPath
    )

    if (-Not (Test-Path $rutaAEliminar)) {
        Write-Warning "Path invalido. $rutaAEliminar no encontrado"
        Exit
    }
    
    $fecha = Get-Date
    $randomNumber = [Math]::Round(($fecha).ToFileTimeUTC()/10000)
    $nombre = Get-Item $rutaAEliminar | % { $_.Name }
    $fullPath = Get-Item $rutaAEliminar | % { $_.FullName }
    $rutaSinNombre = $fullPath.substring(0, ($fullPath.length - $nombre.length -1))

    $nuevoNombre = $nombre+$randomNumber
    Rename-Item -Path $rutaAEliminar -NewName $nuevoNombre
    $newPath = "$rutaSinNombre/$nuevoNombre"

    $compress = @{
        Path = $newPath
        CompressionLevel = "Fastest"
        DestinationPath = $recycleBinPath
      }
      Compress-Archive @compress -Update

    Remove-Item -Path $newPath -Force -Recurse
    "{0},{1},{2},{3}" -f $nombre,$nuevoNombre,$fecha,$rutaSinNombre | Add-Content -path $csvFile

} 

function vaciar () {
    Param (
        [string] $registers,
        [string] $registerCsvFile,
        [string] $recycleBinPath
    )

    if ($registers.Count -gt 0) {
        Remove-Item $recycleBinPath -Recurse
        Set-Content -Value "" -path $registerCsvFile
    }
    else
    {
        Write-Host "La papelera ya esta vacia"
    }
}

function borrar() {
    Param (
        $registers,
        [string] $recycleBinPath,
        [string] $fileName,
        [string] $registerCsvFile
    )

    $registrosABorrar = $registers | Where { $fileName -eq $_.FileName }

    if(!$registrosABorrar) {
        Write-Warning "El archivo $fileName no se encuentra en la papelera"
        Exit
    } 

    if ($registrosABorrar.Count -gt 1) {
        listar $registrosABorrar
        $archivo = Read-Host "¿Qué archivo desea recuperar? (Ingrese Id)" 
        $registroABorrar = $registrosABorrar[$archivo-1]
    } else {
        $registroABorrar = $registrosABorrar[0]
    }

    $registers = $registers | Where { $registroABorrar.removedFileName -ne $_.removedFileName }
    Remove-Item $registerCsvFile
    foreach ($registro in $registers) {
        "{0},{1},{2},{3}" -f $registro.FileName, $registro.removedFileName, $registro.deleteDate, $registro.path | Add-Content -path $registerCsvFile
    }

    $update = 2
    $zip = [IO.Compression.ZipFile]::Open($recycleBinPath, $update)  
    if($zip) {
        $entries = $zip.Entries | where {$_.Name -like $registroABorrar.removedFileName} 
        if(!$entries) {
            $zip.Dispose()
            Write-Error "Error Borrando Archivo"
            Exit
        }
        $entries[0].Delete()
    }
    $zip.Dispose()

} 

function crearPapelera() {
    Param (
        [string] $archivoClave,
        [string] $recycleBinPath,
        [string] $registerCsvFile
    )

    New-Item $archivoClave -ItemType File | Out-Null
    Compress-Archive -Path $archivoClave -DestinationPath $recycleBinPath -CompressionLevel Optimal -Update
    Remove-Item $archivoClave
    
    if( Test-Path $registerCsvFile )
    {
        Remove-Item $registerCsvFile
    }
    New-Item $registerCsvFile | Out-Null
}

$registerCsvFile = './registros.csv'
$recycleBinPath = "$home/papelera.Zip"
$archivoClave = "AgosMatiVictorAngelFacu.txt"

try {
    $zip = [IO.Compression.ZipFile]::OpenRead($recycleBinPath)  
    if($zip) {
        $entries = $zip.Entries | where {$_.FullName -like $archivoClave} 
        if(!$entries) {
            $zip.Dispose()
            Remove-Item $recycleBinPath
            Write-Output "Borrando Papelera"
            crearPapelera $archivoClave $recycleBinPath $registerCsvFile
        }
    }
    $zip.Dispose()
}
catch [Exception] {
    crearPapelera $archivoClave $recycleBinPath
}

<#$entries = $zip.Entries | where {$_.FullName -like 'myzipdir/c/*'} 

New-Item -ItemType Directory -Path "c:\temp\c" -Force

$entries | foreach {[IO.Compression.ZipFileExtensions]::ExtractToFile( $_, "c:\temp\c\" + $_.Name) }

#>

$header = @('FileName','removedFileName','dateDeleted','path')
$registers = Import-Csv -Path $registerCsvFile -Header $header 

if($listar) {
    listar $registers
} elseif($eliminar) {
    eliminar $eliminar $registerCsvFile $recycleBinPath
} elseif($vaciar) {
    vaciar $registers $registerCsvFile $recycleBinPath
} elseif($borrar) {
    borrar $registers $recycleBinPath $borrar $registerCsvFile
}

