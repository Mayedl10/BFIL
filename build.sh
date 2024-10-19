#!/bin/bash

ScriptPath="$(realpath "$0")"
ScriptDirectory="$(dirname "$ScriptPath")"
cd "$ScriptDirectory"

outFile="main"
binaryPath="./$outFile"
srcFolderA="src"
srcFolderB="src/instr"
includeFolder="include"

g++ $srcFolderA/*.cpp $srcFolderB/*.cpp -I $includeFolder -o $outFile && echo "[Compilation Successful]" || echo "[Compilation Failed]"