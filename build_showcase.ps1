# ============================================================
#  BoratoVHSStyle - Build KnobShowcase
#  Execute no PowerShell como: .\build_showcase.ps1
# ============================================================

$ErrorActionPreference = "Stop"

$ProjectRoot = $PSScriptRoot
$BuildDir    = Join-Path $ProjectRoot "build-showcase"

Write-Host ""
Write-Host "=======================================" -ForegroundColor Cyan
Write-Host "  BoratoVHSStyle - KnobShowcase Build" -ForegroundColor Cyan
Write-Host "=======================================" -ForegroundColor Cyan
Write-Host ""

# -----------------------------------------------------------
# 1. Verificar CMake
# -----------------------------------------------------------
Write-Host "[1/3] Verificando CMake..." -ForegroundColor Yellow
try {
    $cmakeVer = cmake --version | Select-Object -First 1
    Write-Host "      $cmakeVer" -ForegroundColor Green
} catch {
    Write-Host "ERRO: CMake nao encontrado. Instale em https://cmake.org/download/" -ForegroundColor Red
    exit 1
}

# -----------------------------------------------------------
# 2. Configurar (CMake configure)
# -----------------------------------------------------------
Write-Host ""
Write-Host "[2/3] Configurando projeto (CMake)..." -ForegroundColor Yellow

if (-Not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

cmake `
    -S "$ProjectRoot" `
    -B "$BuildDir" `
    -DBVS_BUILD_EXAMPLE=OFF `
    -DBVS_BUILD_SHOWCASE=ON `
    -DBVS_FETCH_JUCE=ON `
    -DBVS_ENABLE_OPENGL=OFF

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERRO: CMake configure falhou." -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "      Configuracao concluida." -ForegroundColor Green

# -----------------------------------------------------------
# 3. Compilar
# -----------------------------------------------------------
Write-Host ""
Write-Host "[3/3] Compilando KnobShowcase (Debug)..." -ForegroundColor Yellow

cmake --build "$BuildDir" --config Debug --target KnobShowcase --parallel

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERRO: Build falhou. Veja o log acima." -ForegroundColor Red
    exit $LASTEXITCODE
}

# -----------------------------------------------------------
# 4. Localizar o executavel gerado
# -----------------------------------------------------------
Write-Host ""
Write-Host "=======================================" -ForegroundColor Green
Write-Host "  Build concluido com sucesso!" -ForegroundColor Green
Write-Host "=======================================" -ForegroundColor Green
Write-Host ""

$Exe = Get-ChildItem -Path $BuildDir -Filter "KnobShowcase.exe" -Recurse -ErrorAction SilentlyContinue | Select-Object -First 1
if ($Exe) {
    Write-Host "Executavel gerado:" -ForegroundColor Cyan
    Write-Host "  $($Exe.FullName)" -ForegroundColor White
    Write-Host ""

    $Launch = Read-Host "Deseja abrir o KnobShowcase agora? (s/n)"
    if ($Launch -eq "s") {
        Start-Process $Exe.FullName
    }
} else {
    Write-Host "Executavel nao encontrado em $BuildDir" -ForegroundColor Yellow
    Write-Host "Verifique a pasta build-showcase manualmente." -ForegroundColor Yellow
}

Write-Host ""
