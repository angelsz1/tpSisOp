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
        Write-Output "The recycle bin is empty"
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

remove $register $recycleBinPath "Mati"
