cmake --build .\build

if ($LASTEXITCODE -ne 0){
    exit $LASTEXITCODE
}

./main.exe @args