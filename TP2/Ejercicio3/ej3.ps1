#APL N. 2
#SCRIPT 3
#INTEGRANTES:
#Carballo, Facundo Nicolas (DNI: 42774931)
#Garcia Burgio, Matias Nicolas (DNI: 42649117)
#Mottura, Agostina Micaela (DNI: 41898101)
#Povoli Olivera, Victor (DNI: 43103780)
#Szust, Ángel Elías (DNI: 43098495)
[CmdletBinding()]
Param(
    [Parameter(Mandatory=$True,Position=0)]
    [switch]$codigo,
    
	[ValidateScript({
        if(-Not ($_ | Test-Path) ){
            throw "Archivo o carpeta inexistente" 
        }
        if(($_ | Test-Path -PathType Leaf) ){
            throw "Las rutas deben ser de carpetas"
        }
        return $true
    })] [Parameter(Mandatory = $True,Position = 1)] [System.IO.FileInfo]$path,

    [Parameter(Mandatory=$True,Position=2)]
    [switch] $acciones,
    [Parameter(Mandatory=$true,Position=3)]
    [ValidateSet("listar","peso","compilar","publicar")]
    [string[]] $actions,
    [Parameter(Mandatory=$False,Position=4)]
    [switch] $salida,
    [Parameter(Mandatory=$false,Position=5)]
    [System.IO.FileInfo] $output

  )

$global:accion = $actions
$global:output = $output

if ($accion.Contains("publicar") -eq $True -and $accion.Contains("compilar") -eq $False) {
    Write-Error "No se puede publicar sin compilar primero"
    exit
}
if ( $accion.Contains("publicar") -eq $True -and ($salida -eq $False -or $null -eq $output) ) {
    Write-Error "No se puede publicar sin especificar la salida"
    exit
}

if ( $accion.Contains("publicar") -eq $False -and ($salida -eq $True -or $null -ne $output)) {
	Write-Error "No se puede especificar la salida sin publicar"
	exit
}

#compilo y publico antes de empezar a monitorizar
if ($accion.Contains("compilar") -eq $True){
	if(-Not ("bin" | Test-Path) ){
		New-Item -ItemType Directory -Path "bin" | Out-Null
	}
	$files = Get-ChildItem $path -Recurse
	if ("$PWD\bin\con.out" | Test-Path){
		Clear-Content $PWD\bin\con.out
	}
	foreach ($file in $files) {
		Get-Content $file.FullName | Out-File -FilePath $PWD\bin\con.out -Append 
	}
}

if ($accion.Contains("publicar") -eq $True){
	if(-Not ($output | Test-Path) ){
		New-Item -ItemType Directory -Path $output | Out-Null
	}
	Copy-Item -Path $PWD\bin\con.out -Destination $output | Out-Null
}


#vacio el log
if (-Not ("log.txt" | Test-Path)) {
	New-Item -Path . -Name "log.txt" -ItemType File | Out-Null
}
else{
	Clear-Content ./log.txt
}


[string]$fullPath = (Convert-Path $Path)
$watcher = New-Object -TypeName System.IO.FileSystemWatcher 
$watcher.Path = $fullPath
$watcher.Filter = ''
$watcher.IncludeSubdirectories = $true
$watcher.EnableRaisingEvents = $true

  
    $action = {
      $details = $event.SourceEventArgs
      $Name = $details.Name
      $FullPath = $details.FullPath
      $path = Split-Path $FullPath -Parent
      $OldName = $details.OldName

      $ChangeType = $details.ChangeType

      if($accion.Contains("listar") -eq $True) {
        switch ($ChangeType) {
          "Created" {
            Add-content "log.txt" -Value "El archivo $Name en el directorio $path ha sido creado."
          }
          "Changed" {
            Add-content "log.txt" -Value "El archivo $Name en el directorio $path ha sido modificado."
          }
          "Deleted" {
            Add-content "log.txt" -Value "El archivo $Name en el directorio $path ha sido eliminado.`n"
          }
          "Renamed" {
            Add-content "log.txt" -Value "El archivo $OldName en el directorio $path ha sido renombrado a $Name."
          }
        }
      }
      if($accion.Contains("peso") -eq $True) {
        $size = (Get-Item $FullPath).Length
        if ($size -gt 1TB) {$tam = [string]::Format("{0:0.00} TB", $size / 1TB)}
        elseif ($size -gt 1GB) {$tam = [string]::Format("{0:0.00} GB", $size / 1GB)}
        elseif ($size -gt 1MB) {$tam = [string]::Format("{0:0.00} MB", $size / 1MB)}
        elseif ($size -gt 1KB) {$tam = [string]::Format("{0:0.00} kB", $size / 1KB)}
        elseif ($size -gt 0) {$tam = [string]::Format("{0:0.00} B", $size)}
        else {$tam = "0B"}
        if($ChangeType -ne "Deleted"){
          Add-Content "log.txt" "El tamaño del archivo es $tam`n"
        }
      }


      if($accion.Contains("compilar") -eq $True) {
		$files = Get-ChildItem $path -Recurse
		Clear-Content $PWD\bin\con.out
		foreach ($file in $files) {
			Get-Content $file.FullName | Out-File -FilePath $PWD\bin\con.out -Append
		}
      }


	  #concatenate all files in $path and save it in con.out
	  if($accion.Contains("publicar") -eq $True) {
		Copy-Item -Path $PWD\bin\con.out -Destination $output
	  }

    }
    Register-ObjectEvent $watcher "Created" -Action $action | Out-Null
    Register-ObjectEvent $watcher "Changed" -Action $action | Out-Null
    Register-ObjectEvent $watcher "Deleted" -Action $action | Out-Null
    Register-ObjectEvent $watcher "Renamed" -Action $action | Out-Null

  
    Write-Host "Monitoreando $fullPath" -ForegroundColor Green
    do
    {
      Wait-Event -Timeout 1
    } while ($true)
  
