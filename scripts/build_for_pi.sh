#!/usr/bin/env bash
# Build for Raspberry Pi (aarch64). Tested for Pi 4 / Pi 5 (8GB) targets.
# Usage: RPI_SYSROOT=/path/to/sysroot ./build_for_pi.sh
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build-pi"
SYSROOT_ARG=${1:-}
SYSROOT=${RPI_SYSROOT:-"$SYSROOT_ARG"}

if [ -z "$SYSROOT" ]; then
  echo "Set RPI_SYSROOT environment variable or pass sysroot as first argument." >&2
  echo "Example: RPI_SYSROOT=/home/user/raspi-sysroot $0" >&2
  exit 1
fi

mkdir -p "$BUILD_DIR"
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$ROOT_DIR/cmake/toolchain_pi_aarch64.cmake" -DRPI_SYSROOT="$SYSROOT" -DCMAKE_BUILD_TYPE=Release -DENABLE_QML=OFF
cmake --build "$BUILD_DIR" -j $(nproc)

echo "Build finished in $BUILD_DIR"