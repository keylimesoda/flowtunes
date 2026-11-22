param(
    [Parameter(Mandatory=$true)][string]$QtDir,
    [string]$BuildDir = "build-win",
    [string]$OutputDir = "dist/Flowtunes"
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
Push-Location $root
try {
    $env:Path = "$QtDir/bin;$env:Path"
    cmake -S . -B $BuildDir -G Ninja -DCMAKE_BUILD_TYPE=Release "-DCMAKE_PREFIX_PATH=$QtDir"
    if ($LASTEXITCODE) { throw 'CMake configuration failed' }
    cmake --build $BuildDir
    if ($LASTEXITCODE) { throw 'Build failed' }
    ctest --test-dir $BuildDir --output-on-failure
    if ($LASTEXITCODE) { throw 'Tests failed' }
    New-Item -ItemType Directory -Force $OutputDir | Out-Null
    & "$QtDir/bin/windeployqt.exe" --release --qmldir src --dir $OutputDir "$BuildDir/flowtunes.exe"
    if ($LASTEXITCODE) { throw 'Qt deployment failed' }
    Copy-Item "$BuildDir/flowtunes.exe" "$OutputDir/flowtunes.exe" -Force
    Copy-Item LICENSE "$OutputDir/LICENSE.txt" -Force
    if ($env:VCToolsRedistDir) {
        $crt = Join-Path $env:VCToolsRedistDir 'x64/Microsoft.VC143.CRT'
        if (Test-Path $crt) { Copy-Item "$crt/*.dll" $OutputDir -Force }
    }
    Write-Host "Ready: $OutputDir/flowtunes.exe"
} finally { Pop-Location }
