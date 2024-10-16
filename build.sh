#!/bin/bash

ScriptPath="$(realpath "$0")"
ScriptDirectory="$(dirname "$ScriptPath")"
cd "$ScriptDirectory"

outFile="main.exe"
binaryPath="./$outFile"
srcFolder="src"
includeFolder="include"

g++ $srcFolder/*.cpp -I $includeFolder -o $outFile

if [[ ! -f "$binaryPath" ]]; then
    echo "[Compilation Failed]"
else
    echo "[Compilation Successful]"
fi
