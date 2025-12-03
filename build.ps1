$ScriptPath = $MyInvocation.MyCommand.Path
$ScriptDirectory = Split-Path $ScriptPath
Set-Location -Path $ScriptDirectory

$binaryPath = "./main.exe"

if (Test-Path $binaryPath) {
    Remove-Item $binaryPath
}

g++ src/*.cpp src/*.cpp -I include -o $outFile

if (!(Test-Path $binaryPath)) {
    Write-Host "[Compilation Failed]"

} else {
    Write-Host "[Compilation Successful]"
}
