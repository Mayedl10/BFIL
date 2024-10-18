#!/bin/bash

ScriptPath="$(realpath "$0")"
ScriptDirectory="$(dirname "$ScriptPath")"
cd "$ScriptDirectory"

outFile="main"
binaryPath="./$outFile"
srcFolder="src"
includeFolder="include"

g++ $srcFolder/*.cpp -I $includeFolder -o $outFile && echo "[Compilation Successful]" || echo "[Compilation Failed]"