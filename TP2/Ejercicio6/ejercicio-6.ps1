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

function list() { 
    Param (
        $register
    )
    
    if ($register.Count -gt 0) {
        "|{0,-11}|{1,-22}|{2,-20}" -f "Name", "Date Deleted", "Original Location"
        foreach ($register in $registers) {
            "|{0,-11}|{1,-22}|{2,-20}" -f $register.FileName, $register.dateDeleted, $register.path
        }
    } else {
        Write-Host "La papelera esta vacia"
    }
}

function delete() {
    Param (
        [string] $path,
        [string] $csvFile,
        [string] $recycleBinPath
    )

    $deleteDate = Get-Date
    $deleteFileTime = [Math]::Round(($deleteDate).ToFileTimeUTC()/10000)
    $directory = Get-ChildItem $path | % { $_.Directory.FullName }
    $originalFileName = Split-Path $path -leaf
    $newFileName = $originalFileName+$deleteFileTime

    Rename-Item -Path $path -NewName $newFileName
    $newPath = "$directory/$newFileName"

    $compress = @{
        Path = $newPath
        CompressionLevel = "Fastest"
        DestinationPath = $recycleBinPath
      }
      Compress-Archive @compress -Update

    Remove-Item -Path $newPath
    "{0},{1},{2},{3}" -f $originalFileName,$newFileName,$deleteDate,$directory | Add-Content -path $csvFile
} 

function empty () {
    Param (
        [string] $registers,
        [string] $registerCsvFile,
        [string] $recycleBinPath
    )

    if ($registers.Count -gt 0) {
        Get-ChildItem $recycleBinPath -Recurse | Remove-Item -Force #vacia la papelera pero no la elimina
        Set-Content -Value "" -path $registerCsvFile    #sobreescribe con NADA el arch csv
    }
    else
    {
        Write-Host "La papelera ya esta vacia"
    }
}

function remove() {
    Param (
        [string] $registers,
        [string] $recycleBinPath,
        [string] $fileName
    )

    $coincidencias = $registers | Select-String "$fileName" 
} 

function crearPapelera() {
    Param (
        [string] $archivoClave,
        [string] $recycleBinPath
    )

    New-Item $archivoClave -ItemType File
    Compress-Archive -Path $archivoClave -DestinationPath $recycleBinPath -CompressionLevel Optimal -Update
    Remove-Item $archivoClave
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
            crearPapelera $archivoClave $recycleBinPath
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
    list $registers
} elseif($eliminar) {
    delete $eliminar $registerCsvFile $recycleBinPath
} elseif($vaciar) {
    Write-Output "Quiero vaciar"
}

#empty $registers $registerCsvFile $recycleBinPath

#FALTAAAA
#remove $register $recycleBinPath "Mati"

