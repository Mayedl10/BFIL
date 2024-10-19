$ScriptPath = $MyInvocation.MyCommand.Path
$ScriptDirectory = Split-Path $ScriptPath
Set-Location -Path $ScriptDirectory

$outFile = "main.exe"
$binaryPath = "./$outFile"
$srcFolderA = "src"
$srcFolderB = "src/instr"
$includeFolder = "include"

if (Test-Path $binaryPath) {
    Remove-Item $binaryPath
}

g++ $srcFolderA/*.cpp $srcFolderB/*.cpp -I $includeFolder -o $outFile

if (!(Test-Path $binaryPath)) {
    Write-Host "[Compilation Failed]"

} else {
    Write-Host "[Compilation Successful]"
}
