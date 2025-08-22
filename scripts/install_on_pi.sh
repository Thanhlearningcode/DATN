#!/usr/bin/env bash
set -euo pipefail

# install_on_pi.sh
# Run on Raspberry Pi (Raspbian / Raspberry Pi OS) to install dependencies and build native

PKGS=(build-essential cmake pkg-config qt6-base-dev qt6-base-dev-tools qt6-declarative-dev libqt6charts6 qml-module-qt6-charts qt6-quickcontrols2-plugin)

echo "Updating package lists..."
sudo apt update

echo "Installing build dependencies (may ask for sudo password)..."
sudo apt install -y "${PKGS[@]}" || {
  echo "Some Qt6 packages are not available via apt on your distro.\nYou may need to install Qt via the Qt Online Installer or adjust package names.\nRefer to README.md for details." >&2
}

# Build
BUILD_DIR="build"
mkdir -p "$BUILD_DIR"
cmake -S .. -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DENABLE_QML=OFF
cmake --build "$BUILD_DIR" -j $(nproc)

# Strip binary to reduce size
if [ -f "$BUILD_DIR/VibrationAnalyzerApp" ]; then
  strip --strip-unneeded "$BUILD_DIR/VibrationAnalyzerApp" || true
  echo "Built binary: $BUILD_DIR/VibrationAnalyzerApp"
else
  echo "Build finished but binary not found in $BUILD_DIR" >&2
fi

echo "Done. Run with: ./run_vibration.sh (or set QT env vars and run $BUILD_DIR/VibrationAnalyzerApp)"
