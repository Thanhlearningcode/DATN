#!/usr/bin/env bash
# Build for Raspberry Pi 4 (aarch64) using cross toolchain
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build-pi"
SYSROOT=${RPI_SYSROOT:-"/path/to/raspi/sysroot"}

if [ "$SYSROOT" = "/path/to/raspi/sysroot" ]; then
  echo "Set RPI_SYSROOT environment variable to your Raspberry Pi sysroot path or edit this script." >&2
  exit 1
fi

mkdir -p "$BUILD_DIR"
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$ROOT_DIR/cmake/toolchain_pi_aarch64.cmake" -DRPI_SYSROOT="$SYSROOT" -DCMAKE_BUILD_TYPE=Release -DENABLE_QML=OFF
cmake --build "$BUILD_DIR" -j $(nproc)

echo "Build finished in $BUILD_DIR"