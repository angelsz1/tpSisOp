#APL N. 2
#SCRIPT 3
#INTEGRANTES:
#Carballo, Facundo Nicolas (DNI: 42774931)
#Garcia Burgio, Matias Nicolas (DNI: 42649117)
#Mottura, Agostina Micaela (DNI: 41898101)
#Povoli Olivera, Victor (DNI: 43103780)
#Szust, Ángel Elías (DNI: 43098495)

<#

.SYNOPSIS
    Script que monitorea un directorio y guarda en un archivo de log los archivos que se crean, modifican o eliminan segun los parametros que se le pasen.
.DESCRIPTION
    Este script monitorea un directorio a la espera de cambios. Si se detecta algun cambio, se guarda o no determinada informacion dentro de un archivo de log, y se hacen o no distintas acciones.
.PARAMETER codigo
    Define la ruta del directorio a monitorear. Monitorea tambien los subdirectorios.
.PARAMETER acciones
    Una lista de acciones separadas con coma a ejecutar cada vez que se detecte un cambio en el directorio. Las acciones posibles son:
        - listar   : Muestra en un log los archivos que se crearon, modificaron, renombraron o eliminaron.
        - peso     : Muestra en un log el peso de los archivos que sufrieron cambios.
        - compilar : Concatena el contenido de los archivos dentro de "codigo" y lo guarda en un archivo llamado "con.out" en el directorio "bin", dentro del path del script."
        - publicar : Copia el archivo "con.out" del directorio "bin" a la carpeta "salida" pasada por parametro.
.PARAMETER salida
    Define la ruta de la carpeta donde se copiara el archivo "con.out" cuando se ejecute la accion "publicar".
.EXAMPLE
    Ejemplo de uso:
        .\ej3.ps1 -codigo "/dir a monitorear" -acciones "listar,peso,compilar,publicar" -salida "../salidas"
    Este ejemplo monitorea el directorio "/dir a monitorear" y ejecuta las acciones "listar", "peso", "compilar" y "publicar" cada vez que se detecte un cambio en el directorio.
    La accion "listar" guarda en un log los archivos que se crearon, modificaron, renombraron o eliminaron.
    La accion "peso" guarda en un log el peso de los archivos que sufrieron cambios.
    La accion "compilar" concatena el contenido de los archivos dentro de "/dir a monitorear" y lo guarda en "con.out". 
    La accion "publicar" copia el archivo "con.out" del directorio "bin" a la carpeta "../salidas" pasada por parametro.
#>

[CmdletBinding()]
Param(
  [ValidateScript({
      if (-Not ($_ | Test-Path) ) {
        throw "Archivo o carpeta inexistente" 
      }
      if (($_ | Test-Path -PathType Leaf) ) {
        throw "Las rutas deben ser de carpetas"
      }
      return $true
    })] [Parameter(Mandatory = $True)] [System.IO.FileInfo]$codigo,
  [Parameter(Mandatory = $True)]
  [ValidateSet("listar", "peso", "compilar", "publicar")]
  [string[]] $acciones,
  
  [Parameter(Mandatory = $False)] [System.IO.FileInfo] $salida

)

$global:accion = $acciones
$global:output = $salida

#Write-Host "Monitoreando $fullPath" -ForegroundColor Green

if ($accion.Contains("publicar") -eq $True -and $accion.Contains("compilar") -eq $False) {
  Write-Host "Error : No se puede publicar sin compilar primero" -ForegroundColor Red
  exit
}
if ( $accion.Contains("publicar") -eq $True -and ($salida -eq $False -or $null -eq $output) ) {
  Write-Host "Error : No se puede publicar sin especificar la salida" -ForegroundColor Red
  exit
}

if ( $accion.Contains("publicar") -eq $False -and ($salida -eq $True -or $null -ne $output)) {
  Write-Host "Error : No se puede especificar la salida sin publicar" -ForegroundColor Red
  exit
}

#compilo y publico antes de empezar a monitorizar
if ($accion.Contains("compilar") -eq $True) {
  if (-Not ("bin" | Test-Path) ) {
    New-Item -ItemType Directory -Path "bin" | Out-Null
  }
  $files = Get-ChildItem $codigo -Recurse
  if ("$PWD\bin\con.out" | Test-Path) {
    Clear-Content $PWD\bin\con.out
  }
  foreach ($file in $files) {
    $target = get-item $file.FullName
    if($target.PSIsContainer -eq $False) {
      Get-Content $file.FullName | Out-File -FilePath $PWD\bin\con.out -Append 
    }
  }
}

if ($accion.Contains("publicar") -eq $True) {
  if (-Not ($output | Test-Path) ) {
    New-Item -ItemType Directory -Path $output | Out-Null
  }
  Copy-Item -Path $PWD\bin\con.out -Destination $output | Out-Null
}


#vacio el log
if (-Not ("log.txt" | Test-Path)) {
  New-Item -Path . -Name "log.txt" -ItemType File | Out-Null
}
else {
  Clear-Content ./log.txt
}


[string]$fullPath = (Convert-Path $codigo)
$watcher = New-Object -TypeName System.IO.FileSystemWatcher 
$watcher.Path = $fullPath
$watcher.Filter = ''
$watcher.IncludeSubdirectories = $true
$watcher.EnableRaisingEvents = $true

  
$action = {
  $details = $event.SourceEventArgs
  $Name = $details.Name
  $FullPath = $details.FullPath
  $codigo = Split-Path $FullPath -Parent
  $OldName = $details.OldName

  $ChangeType = $details.ChangeType

  if ($accion.Contains("listar") -eq $True) {
    switch ($ChangeType) {
      "Created" {
        Add-content "log.txt" -Value "El archivo $Name en el directorio $codigo ha sido creado."
      }
      "Changed" {
        Add-content "log.txt" -Value "El archivo $Name en el directorio $codigo ha sido modificado."
      }
      "Deleted" {
        Add-content "log.txt" -Value "El archivo $Name en el directorio $codigo ha sido eliminado.`n"
      }
      "Renamed" {
        Add-content "log.txt" -Value "El archivo $OldName en el directorio $codigo ha sido renombrado a $Name."
      }
    }
  }
  if ($accion.Contains("peso") -eq $True) {
    $size = (Get-Item $FullPath).Length
    if ($size -gt 1TB) { $tam = [string]::Format("{0:0.00} TB", $size / 1TB) }
    elseif ($size -gt 1GB) { $tam = [string]::Format("{0:0.00} GB", $size / 1GB) }
    elseif ($size -gt 1MB) { $tam = [string]::Format("{0:0.00} MB", $size / 1MB) }
    elseif ($size -gt 1KB) { $tam = [string]::Format("{0:0.00} kB", $size / 1KB) }
    elseif ($size -gt 0) { $tam = [string]::Format("{0:0.00} B", $size) }
    else { $tam = "0B" }
    if ($ChangeType -ne "Deleted") {
      Add-Content "log.txt" "El tamaño del archivo es $tam`n"
    }
  }


  if ($accion.Contains("compilar") -eq $True) {
    $files = Get-ChildItem $codigo -Recurse
    Clear-Content $PWD\bin\con.out
    foreach ($file in $files) {
      Get-Content $file.FullName | Out-File -FilePath $PWD\bin\con.out -Append
    }
  }


  #concatenate all files in $codigo and save it in con.out
  if ($accion.Contains("publicar") -eq $True) {
    Copy-Item -Path $PWD\bin\con.out -Destination $output
  }

}
Register-ObjectEvent $watcher "Created" -Action $action | Out-Null
Register-ObjectEvent $watcher "Changed" -Action $action | Out-Null
Register-ObjectEvent $watcher "Deleted" -Action $action | Out-Null
Register-ObjectEvent $watcher "Renamed" -Action $action | Out-Null

  
Write-Host "Monitoreando $fullPath" -ForegroundColor Green
# do
# {
#   Wait-Event -Timeout 1
# } while ($true)
  

#para matarlo
#Get-EventSubscriber | Unregister-Event
