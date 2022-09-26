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

<#
Param(
    [Parameter(ParameterSetName = --listar)]
    [Parameter(ParameterSetName = --recuperar)]
    [Parameter(ParameterSetName = --vaciar)]
    [Parameter(ParameterSetName = --eliminar)]
    [Parameter(ParameterSetName = --borrar)]
)
#>

function list() { 
    Param (
        $register
)
    if ($register.Count -gt 0) {
        "|{0,-11}|{1,20}|{2,14}|" -f "Name", "Original Location", "Date Deleted"
        foreach ($register in $registers) {
            "|{0,-11}|{1,20}|{2,14}|" -f $register.originaloriginalFileName, $register.path, $register.dateDeleted
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
    <#$zip = [IO.Compression.ZipFile]::OpenRead($recycleBinPath)
    $entries = $zip.Entries | where {$_.FullName -like 'myzipdir/c/*'} 

    #create dir for result of extraction
    New-Item -ItemType Directory -Path "c:\temp\c" -Force

    #extraction
    $entries | foreach {[IO.Compression.ZipFileExtensions]::ExtractToFile( $_, "c:\temp\c\" + $_.Name) }

    #free object
    $zip.Dispose()#>
} 

$registerCsvFile = './registros.csv'
$recycleBinPath = "$home/recycleBin.Zip"

$header = @('FileName','removedFileName','dateDeleted','path')
$registers = Import-Csv -Path $registerCsvFile -Header $header 
#$registers | Format-Table

#list $registers
#delete "Mati" $registerCsvFile $recycleBinPath
empty $registers $registerCsvFile $recycleBinPath

#FALTAAAA
#remove $register $recycleBinPath "Mati"

