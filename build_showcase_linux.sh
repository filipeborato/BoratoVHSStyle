#!/usr/bin/env bash
# ============================================================
#  BoratoVHSStyle - Build KnobShowcase (Linux / WSL)
#  Execute com: bash build_showcase_linux.sh
# ============================================================

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-showcase-linux"

# Cores
CYAN='\033[0;36m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo ""
echo -e "${CYAN}=======================================${NC}"
echo -e "${CYAN}  BoratoVHSStyle - KnobShowcase Build${NC}"
echo -e "${CYAN}  Linux / WSL${NC}"
echo -e "${CYAN}=======================================${NC}"
echo ""

# -----------------------------------------------------------
# 1. Verificar dependências
# -----------------------------------------------------------
echo -e "${YELLOW}[1/4] Verificando dependencias...${NC}"

check_cmd() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${RED}ERRO: '$1' nao encontrado.${NC}"
        echo "      Instale com: $2"
        exit 1
    fi
    echo "      $1: OK ($(command -v "$1"))"
}

check_cmd cmake  "sudo apt install cmake"
check_cmd g++    "sudo apt install g++"
check_cmd git    "sudo apt install git"

# Verificar libs de GUI necessárias para o JUCE
MISSING_LIBS=()

check_pkg() {
    if ! dpkg -s "$1" &>/dev/null 2>&1; then
        MISSING_LIBS+=("$1")
    fi
}

if command -v dpkg &>/dev/null; then
    check_pkg libx11-dev
    check_pkg libxext-dev
    check_pkg libxrandr-dev
    check_pkg libxinerama-dev
    check_pkg libxcursor-dev
    check_pkg libfreetype6-dev
    check_pkg libasound2-dev
    check_pkg libfontconfig1-dev
    check_pkg libgl1-mesa-dev
    check_pkg libgtk-3-dev
    check_pkg libwebkit2gtk-4.0-dev

    if [ ${#MISSING_LIBS[@]} -gt 0 ]; then
        echo ""
        echo -e "${YELLOW}Pacotes faltando. Instalando automaticamente...${NC}"
        echo "      ${MISSING_LIBS[*]}"
        sudo apt-get install -y "${MISSING_LIBS[@]}"
    else
        echo "      Libs de sistema: OK"
    fi
fi

# -----------------------------------------------------------
# 2. Configurar (CMake configure)
# -----------------------------------------------------------
echo ""
echo -e "${YELLOW}[2/4] Configurando projeto (CMake)...${NC}"

mkdir -p "$BUILD_DIR"

cmake \
    -S "$PROJECT_ROOT" \
    -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBVS_BUILD_EXAMPLE=OFF \
    -DBVS_BUILD_SHOWCASE=ON \
    -DBVS_FETCH_JUCE=ON \
    -DBVS_ENABLE_OPENGL=OFF

echo -e "${GREEN}      Configuracao concluida.${NC}"

# -----------------------------------------------------------
# 3. Compilar
# -----------------------------------------------------------
echo ""
echo -e "${YELLOW}[3/4] Compilando KnobShowcase...${NC}"

NPROC=$(nproc 2>/dev/null || echo 4)
cmake --build "$BUILD_DIR" --target KnobShowcase --parallel "$NPROC"

echo -e "${GREEN}      Build concluido.${NC}"

# -----------------------------------------------------------
# 4. Localizar e executar
# -----------------------------------------------------------
echo ""
echo -e "${GREEN}=======================================${NC}"
echo -e "${GREEN}  Build concluido com sucesso!${NC}"
echo -e "${GREEN}=======================================${NC}"
echo ""

EXE=$(find "$BUILD_DIR" -name "KnobShowcase" -type f ! -name "*.cpp" ! -name "*.h" 2>/dev/null | head -1)

if [ -n "$EXE" ]; then
    echo -e "${CYAN}Executavel gerado:${NC}"
    echo "  $EXE"
    echo ""

    # Em WSL, verificar se há display disponível
    if grep -qi microsoft /proc/version 2>/dev/null; then
        echo -e "${YELLOW}WSL detectado.${NC}"
        echo "  Para rodar a GUI voce precisa de um servidor X no Windows."
        echo "  Opcoes:"
        echo "    - WSL2 com WSLg (Windows 11): ja funciona, basta rodar abaixo"
        echo "    - WSL1 ou sem WSLg: instale VcXsrv ou X410 e exporte DISPLAY=:0"
        echo ""
    fi

    read -rp "Deseja abrir o KnobShowcase agora? (s/n): " LAUNCH
    if [[ "$LAUNCH" =~ ^[sS]$ ]]; then
        echo "Iniciando $EXE ..."
        "$EXE" &
    fi
else
    echo -e "${YELLOW}Executavel nao encontrado em $BUILD_DIR${NC}"
    echo "Verifique a pasta build-showcase-linux manualmente."
fi

echo ""
