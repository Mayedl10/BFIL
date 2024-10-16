$ScriptPath = $MyInvocation.MyCommand.Path
$ScriptDirectory = Split-Path $ScriptPath
Set-Location -Path $ScriptDirectory

$outFile = "main.exe"
$binaryPath = "./$outFile"
$srcFolder = "src"
$includeFolder = "include"

g++ $srcFolder/*.cpp -I $includeFolder -o $outFile

if (!(Test-Path $binaryPath)) {
    Write-Host "[Compilation Failed]"

} else {
    Write-Host "[Compilation Successful]"
}